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
  || return


if (( $#help )); then
  print -rC1 --      \
        "$0:t [-h|--help]" \
        "$0:t [-c|--clean] [--qmk-home <QMK_HOME>] [--vial-qmk-home <VIAL_QMK_HOME>]" \
        "$0:t [--qmk-home <QMK_HOME>] [--vial-qmk-home <VIAL_QMK_HOME>] [--apple-kb-layout <ansi|iso|jis>] [--without-update-qmk] [--without-vial] [<TARGET...>]"
  return
fi

local -A KEYBOARDS=(
  bakeneko60 bakeneko60:hex:0x021d
  ciel60     ciel60:hex:0x021d
  qk65       qk65_solder:hex:0x024f
  prime_e    prime_e_rgb:hex:0x0220
  d60        dz60rgb_wkl_v2_1:bin:0x021d
  fk680      fk680pro_v2:uf2:0x024f
)

# configuration
# -----------------------------------

# defaults
TARGETS=(bakeneko60 ciel60 qk65 prime_e d60 fk680)
# need forked repository https://github.com/jhorology/vial-qmk
VIAL_QMK_HOME="$HOME/Documents/Sources/vial-qmk-dev"
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
(( $#@ )) && TARGETS=("$@")

if (( $#clean )); then
  rm -rf dist

  find . -name '*~' -exec rm -f {} \;
  find . -name '.DS_Store' -exec rm -f {} \;

  cd "$QMK_HOME"
  make clean
  # checkout to revert changes.
  git checkout --recurse-submodules .
  git clean -dfx

  cd "$VIAL_QMK_HOME"
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
  git pull
  make git-submodule
fi

if [ $APPLE_FN_ENABLE = "yes" ] && [ -z "$(rg APPLE_FN_ENABLE $QMK_HOME/quantum/keymap_common.c)" ]; then
  # checkout to revert changes.
  git checkout --recurse-submodules .
  patch -p1 < "${PROJECT}/patches/applefn.patch"
fi

[ ! -L keyboards/my_keyboards ] && ln -s "${PROJECT}/keyboards" keyboards/my_keyboards

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

for target in $TARGETS; do
  # split ":" [1]=make target [2]=extension
  kbd=(${(@s/:/)KEYBOARDS[$target]})
  mv "$QMK_HOME/my_keyboards_${kbd[1]//\//_}_default.$kbd[2]" ${target}_$VERSION.$kbd[2]
done
