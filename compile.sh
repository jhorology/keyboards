#!/bin/zsh -eu

# PROJECT
# -----------------------------------
PROJECT=$(realpath $0:h)
cd "$PROJECT"
zparseopts -D -E -F -- \
           {h,-help}=help  \
           {c,-clean}=clean \
           -qmk-home:=qmk_home \
           -vial-qmk-home:=vial_qmk_home \
           -without-update-qmk=without_update_qmk \
           -without-vial=without_vial \
           -without-apple_fn=without_apple_fn \
           -apple-fake-layout:=apple_fake_layout \
  || return


if (( $#help )); then
  print -rC1 --      \
        "$0:t [-h|--help]" \
        "$0:t [-c|--clean] [--qmk-home <QMK_HOME>] [--vial-qmk-home <VIAL_QMK_HOME>]" \
        "$0:t [options...] [<TARGET...>]" \
        "" \
        "options:" \
        "  --qmk-home <QMK_HOME>            location for local qmk_firmware repository" \
        "  --vial-qmk-home <VIAL_QMK_HOME>  location for local vial-qmk repository" \
        "  --without-update-qmk             don't sync remote repository" \
        "  --without-vial                   build without VIAL, use QMK_HOME with enabling VIA." \
        "  --without-apple-fn               don't apply apple_fn.patch" \
        "  --apple-fake-layout <layout>     choice fake apple product id 0:ANSI 1:ISO 2:JIS, default 0:ANSI"
  return
fi

local -A KEYBOARDS=(
  bakeneko60 bakeneko60:hex
  ciel60     ciel60:hex
  qk65       qk65_solder:hex
  prime_e    prime_e_rgb:hex
  d60        dz60rgb_wkl_v2_1:bin
  fk680      fk680pro_v2:uf2
  zoom65     zoom65:hex
)

# configuration
# -----------------------------------

# defaults
TARGETS=(bakeneko60 ciel60 qk65 prime_e d60 fk680 zoom65)
VIAL_QMK_HOME="$HOME/Documents/Sources/vial-qmk"
QMK_HOME="$HOME/Documents/Sources/qmk_firmware"
VIAL_ENABLE=yes
APPLE_FN_ENABLE=yes
APPLE_FAKE_LAYOUT=0 # fake apple product 0:ANSI 1:ISO 2:JIS
UPDATE_QMK=true

# .config
#  override configuration
# -----------------------------------
[ -s .config ] &&  source .config

# option parameters
# -----------------------------------
(( $#qmk_home )) && QMK_HOME=${qmk_home[-1]##=}
(( $#vial_qmk_home )) && VIAL_QMK_HOME=${vial_qmk_home[-1]##=}
(( $#without_update_qmk )) && UPDATE_QMK=false
(( $#without_vial )) && VIAL_ENABLE=no
(( $#without_apple_fn )) && APPLE_FN_ENABLE=no
(( $#apple_fake_layout )) && APPLE_FAKE_LAYOUT=${apple_fake_layout[-1]##=}
(( $#@ )) && TARGETS=("$@")



if (( $#clean )); then
  rm -rf dist

  find . -name '*~' -exec rm -f {} \;
  find . -name '.DS_Store' -exec rm -f {} \;
  find . -name 'vial.json' -exec rm -f {} \;

  cd "$QMK_HOME"
  rm -rf keyboards/my_keyboards
  make clean
  # checkout to revert changes.
  git checkout --recurse-submodules .
  git clean -dfx

  cd "$VIAL_QMK_HOME"
  rm -rf keyboards/my_keyboards
  make clean
  # checkout to revert changes.
  git checkout --recurse-submodules .
  git clean -dfx
  return
fi

MAKE_TARGETS=()
for target in $TARGETS; do
  kbd=(${(@s/:/)KEYBOARDS[$target]})
  MAKE_TARGETS=($MAKE_TARGETS my_keyboards/$kbd[1])
done

mkdir -p dist

# QMK_HOME
# -----------------------------------
[ $VIAL_ENABLE = "yes" ] && QMK_HOME="$VIAL_QMK_HOME"
cd "$QMK_HOME"

if $UPDATE_QMK; then
  # checkout to revert changes.
  git checkout --recurse-submodules .
  git clean -dfx
  git pull
  make git-submodule
fi

if [ $APPLE_FN_ENABLE = "yes" ]; then
   [ -z "$(rg APPLE_FN_ENABLE quantum/keymap_common.c)" ] && patch -p1 < "${PROJECT}/patches/applefn.patch"
   [ -z "$(rg get_usb_device_descriptor_ptr tmk_core/protocol/usb_descriptor.h)" ] && patch -p1 < "${PROJECT}/patches/device_descriptor.patch"
fi
if [ $VIAL_ENABLE = "yes" ]; then
  [ -z "$(rg vial_tap_dance_reset_user quantum/dynamic_keymap.h)" ] && patch -p1 < "${PROJECT}/patches/vial_eeprom_reset_user.patch"
  [ -z "$(rg FIX_VIAL_TAP_DANCE_BEHAVIOR quantum/vial.c)" ] && patch -p1 < "${PROJECT}/patches/fix_vial_tap_dance_behavior.patch"
fi

[ -z "$(rg radial_controller_task quantum/keyboard.c)" ] && patch -p1 < "${PROJECT}/patches/radial_controller.patch"
[ -z "$(rg ENCODER_LOOKUP_TABLE quantum/encoder.c)" ] && patch -p1 < "${PROJECT}/patches/encoder_lookup_table.patch"

[ ! -L keyboards/my_keyboards ] && ln -s "${PROJECT}/keyboards" keyboards/my_keyboards

# generate vial json file
if [ $VIAL_ENABLE = "yes" ]; then
  QMK_HOME=$QMK_HOME "$PROJECT/util/generate_vial_json.js" $MAKE_TARGETS[*]
fi

make -j 10 $MAKE_TARGETS[*] VIAL_ENABLE=$VIAL_ENABLE APPLE_FN_ENABLE=$APPLE_FN_ENABLE APPLE_FAKE_LAYOUT=$APPLE_FAKE_LAYOUT

VERSION="$(date +"%Y%m%d")_$(git rev-parse --short HEAD)"
if [ $VIAL_ENABLE = "yes" ]; then
  VERSION="vial_$VERSION"
else
  VERSION="via_$VERSION"
fi
[ $APPLE_FN_ENABLE = "yes" ] && VERSION="applefn_$VERSION"

# dist
# -----------------------------------
cd "$PROJECT/dist"

# generate via json file
if [ $VIAL_ENABLE = "no" ]; then
  QMK_HOME=$QMK_HOME $PROJECT/util/generate_via_json.js $MAKE_TARGETS[*]
fi

for target in $TARGETS; do
  # split ":" [1]=make target [2]=extension
  kbd=(${(@s/:/)KEYBOARDS[$target]})
  mv "$QMK_HOME/my_keyboards_${kbd[1]//\//_}_default.$kbd[2]" ${target}_$VERSION.$kbd[2]
done
