#!/bin/zsh -eu

# PROJECT
# -----------------------------------
PROJECT=$(realpath $0:h)
cd "$PROJECT"
zparseopts -D -E -F -- \
           {h,-help}=help  \
           -clean=clean \
           -qmk-home:=qmk_home \
           -vial-qmk-home:=vial_qmk_home \
           -without-update-qmk=without_update_qmk \
           -without-vial=without_vial \
           -without-apple_fn=without_apple_fn \
  || return


if (( $#help )); then
  print -rC1 --      \
        "$0:t [-h|--help]" \
        "$0:t [-c|--clean]" \
        "$0:t [--qmk-home <QMK_HOME>] [--vial-qmk-home <VIAL_QMK_HOME>] [--without-update-qmk] [--without-vial] [--without-apple-fn] [<TARGET...>]"
  return
fi

local -A KEYBOARDS=(
  bakeneko60 bakeneko60:hex
  ciel60     ciel60:hex
  qk65       qk65_solder:hex
  prime_e    prime_e_rgb:hex
  d60        dz60rgb_wkl_v2_1:bin
  fk680      fk680pro_v2:uf2
)

# configuration
# -----------------------------------

# defaults
TARGETS=(bakeneko60 ciel60 qk65 prime_e d60 fk680)
CLEAN=false
VIAL_QMK_HOME="$HOME/Documents/Sources/vial-qmk"
QMK_HOME="$HOME/Documents/Sources/qmk_firmware"
VIAL_ENABLE=yes
APPLE_FN_ENABLE=yes
UPDATE_QMK=true

# .config
#  override configuration
# -----------------------------------
[ -s .config ] &&  source .config

# option parameters
# -----------------------------------
(( $#clean )) && CLEAN=true
(( $#qmk_home )) && QMK_HOME=${qmk_home[-1]##=}
(( $#vial_qmk_home )) && VIAL_QMK_HOME=${vial_qmk_home[-1]##=}
(( $#without_update_qmk )) && UPDATE_QMK=false
(( $#without_vial )) && VIAL_ENABLE=no
(( $#without_apple_fn )) && APPLE_FN_ENABLE=no
(( $#@ )) && TARGETS=("$@")
[ $VIAL_ENABLE = "yes" ] && QMK_HOME="$VIAL_QMK_HOME"

MAKE_TARGETS=()
for target in $TARGETS; do
  kbd=(${(@s/:/)KEYBOARDS[$target]})
  MAKE_TARGETS=($MAKE_TARGETS my_keyboards/$kbd[1])
done

mkdir -p dist

# QMK_HOME
# -----------------------------------
cd "$QMK_HOME"

if $CLEAN; then
  make clean
  # checkout to revert changes.
  git checkout --recurse-submodules .
  git clean -dfx
  return
fi

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

make -j 10 $MAKE_TARGETS[*] VIAL_ENABLE=$VIAL_ENABLE APPLE_FN_ENABLE=$APPLE_FN_ENABLE

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
