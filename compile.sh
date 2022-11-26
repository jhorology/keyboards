#!/bin/zsh -eu

# PROJECT
# -----------------------------------
PROJECT=$(realpath $0:h)
cd "$PROJECT"
zparseopts -D -E -F -- \
           {h,-help}=help  \
           {c,-clean}=clean \
           {H,-qmk-home}:=qmk_home \
           {v,-via-version}:=via_version \
           {w,-without-update-qmk}=without_update_qmk \
  || return


if (( $#help )); then
  print -rC1 --      \
        "$0:t [-h|--help]" \
        "$0:t [-c|--clean] [--qmk-home <QMK_HOME>]" \
        "$0:t [options...] [<TARGET...>]" \
        "" \
        "options:" \
        "  -H,--qmk-home <QMK_HOME>            location for local qmk_firmware repository" \
        "  -v,--via-version <2|3>              VIA version 2 or 3 default: 3" \
        "  -w,--without-update-qmk             don't sync remote repository"
  return
fi

local -A KEYBOARDS=(
  neko60     bakeneko60:hex
  ciel60     ciel60:hex
  d60        dz60rgb_wkl_v2_1:bin
  fk680      fk680pro_v2:uf2
  ikki68     ikki68_aurora:hex
  k6         k6_pro_ansi_rgb:bin
  libra      libra_mini:hex
  prime_e    prime_e_rgb:hex
  qk60       qk60:bin
  qk65       qk65_solder:hex
  zoom65     zoom65:hex
)

# configuration
# -----------------------------------

# defaults

TARGETS=(neko60 ciel60 d60 fk680 ikki68 libra prime_e qk60 qk65 zoom65)
QMK_HOME="$HOME/Documents/Sources/qmk_firmware"
VIA_VERSION=3
UPDATE_QMK=true
MAKE_JOBS=8
[ $(uname) = "Darwin" ] && MAKE_JOBS=$(sysctl -n hw.ncpu)

# .config
#  override configuration
# -----------------------------------
[ -s .config ] &&  source .config

# targets
# -----------------------------------
(( $#@ )) && TARGETS=("$@")
MAKE_TARGETS=()
TARGET_COUNT=$TARGETS[(I)$TARGETS[-1]]

KEYCHRON_BT=false
for target in $TARGETS; do
  kbd=(${(@s/:/)KEYBOARDS[$target]})
  MAKE_TARGETS=($MAKE_TARGETS my_keyboards/$kbd[1])
  if [ $target = "k6" ]; then
    if [ $TARGET_COUNT -ge 2 ]; then
      print -r "Error: Can't compile k6 together with other keyboards." >&2
      exit 1
    fi
    KEYCHRON_BT=true
  fi
done

# option parameters
# -----------------------------------
(( $#qmk_home )) && QMK_HOME=${qmk_home[-1]##=}
(( $#via_version )) && VIA_VERSION=${via_version[-1]##=}
(( $#without_update_qmk )) && UPDATE_QMK=false

if (( $#clean )); then
  rm -rf dist

  find . -name '*~' -exec rm -f {} \;
  find . -name '.DS_Store' -exec rm -f {} \;
  find . -name 'vial.json' -exec rm -f {} \;

  cd "$QMK_HOME"
  rm -f keyboards/my_keyboards
  make clean
  # checkout to revert changes.
  git checkout --recurse-submodules .
  git reset --hard HEAD
  git clean -dfx

  return
fi

mkdir -p dist

# QMK_HOME
# -----------------------------------

cd "$QMK_HOME"

if $KEYCHRON_BT; then
  [ ! -s keychron_bluetooth_playground ] && UPDATE_QMK=true
else
  [ -s keychron_bluetooth_playground ] && UPDATE_QMK=true
fi

if $UPDATE_QMK; then
  # checkout to revert changes.
  git checkout --recurse-submodules .
  git clean -dfx
  git pull
  make git-submodule
fi

# patch
# -----------------------------------
if $KEYCHRON_BT; then
  [ ! -s keychron_bluetooth_playground ] && \
    patch --verbose -p1 < "${PROJECT}/patches/keychron_bluetooth_playground.patch"
fi

[ -z "$(rg APPLE_FN_ENABLE builddefs/common_features.mk)" ] && \
  patch --verbose -p1 < "${PROJECT}/patches/applefn.patch"

[ -z "$(rg get_usb_device_descriptor_ptr tmk_core/protocol/usb_descriptor.h)" ] && \
  patch --verbose -p1 < "${PROJECT}/patches/device_descriptor.patch"

[ -z "$(rg RADIAL_CONTROLLER_ENABLE builddefs/common_features.mk)" ] && \
  patch --verbose -p1 < "${PROJECT}/patches/radial_controller.patch"

[ -z "$(rg ENCODER_LOOKUP_TABLE quantum/encoder.c)" ] && \
  patch --verbose -p1 < "${PROJECT}/patches/encoder_lookup_table.patch"

[ -z "$(rg normal_true quantum/process_keycode/process_magic.c)" ] && \
  patch --verbose -p1 < "${PROJECT}/patches/qmk_magic_shift_reverse.patch"

if [ $VIA_VERSION = "3" ]; then
  [ -z "$(rg via_raw_hid_receive quantum/via.h)" ] && \
    patch --verbose -p1 < "${PROJECT}/patches/via_v3.patch"
else
  [ -z "$(rg via_raw_hid_receive quantum/via.h)" ] && \
    patch --verbose -p1 < "${PROJECT}/patches/via_V2.patch"
fi
[ -z "$(rg TAP_DANCE_IGNORE_COMBO quantum/process_keycode/process_tap_dance.c)" ] && \
  patch --verbose -p1 < "${PROJECT}/patches/tap_dance_ignore_combo.patch"

# make
# -----------------------------------
rm -f keyboards/my_keyboards
ln -s "${PROJECT}/qmk_keyboards" keyboards/my_keyboards

make -j $MAKE_JOBS $MAKE_TARGETS[*] VIA_VERSION=$VIA_VERSION

VERSION="$(date +"%Y%m%d")_qmk_$(git describe --abbrev=0 --tags)_$(git rev-parse --short HEAD)_via_v$VIA_VERSION"

# generate via json file
QMK_HOME="$QMK_HOME" VIA_VERSION=$VIA_VERSION \
  "$PROJECT/util/generate_via_json.js" $MAKE_TARGETS[*]

# dist
# -----------------------------------
cd "$PROJECT/dist"

for target in $TARGETS; do
  # split ":" [1]=make target [2]=extension
  kbd=(${(@s/:/)KEYBOARDS[$target]})
  firmware_name="${kbd[1]//\//_}"
  mv "$QMK_HOME/my_keyboards_${firmware_name}_default.$kbd[2]" ${firmware_name}_$VERSION.$kbd[2]
done

# formatter
# -----------------------------------
cd "$PROJECT"
./format.sh
