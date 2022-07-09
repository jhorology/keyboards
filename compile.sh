#!/bin/zsh -eu

local -A KEYBOARDS=(
  bakeneko60 bakeneko60:hex
  ciel60     ciel60:hex
  qk65       qk65/solder:hex
  prime_e    prime_e/rgb:hex
  d60        dz60rgb_wkl/v2_1:bin
  fk680      fk680pro_v2:uf2
)

# configuration
# -----------------------------------
# configuration defaults
TARGETS=(bakeneko60 ciel60 qk65 prime_e d60 fk680)
VIAL_QMK_HOME="$HOME/Documents/Sources/vial-qmk"
QMK_HOME="$HOME/Documents/Sources/qmk_firmware"
VIAL_ENABLE=yes
APPLE_FN_ENABLE=yes
UPDATE_QMK=false

# PROJECT
# -----------------------------------
cd "$(dirname $0)"

# .config can override configuration
[ -s .config ] &&  source .config

# arguments can override TARGETS
[ $# -ge 1 ] && TARGETS=("$@")

[ $VIAL_ENABLE = "yes" ] && QMK_HOME="$VIAL_QMK_HOME"

PROJECT=$(pwd)
mkdir -p dist

MAKE_TARGETS=()
local -A BUILD_FIRMWARES=()
for target in $TARGETS; do
  # split ":" [1]=make target [2]=extension
  kbd=(${(@s/:/)KEYBOARDS[$target]})
  make_target=my_keyboards/$kbd[1]
  firmware="$QMK_HOME/${make_target//\//_}_default.$kbd[2]"
  MAKE_TARGETS=($MAKE_TARGETS $make_target)
  BUILD_FIRMWARES[$target]=$firmware
done

# QMK_HOME
# -----------------------------------
cd "$QMK_HOME"

[ ! -L keyboards/my_keyboards ] && ln -s "${PROJECT}/keyboards" keyboards/my_keyboards

if $UPDATE_QMK; then
  # checkout to revert changes.
  git checkout --recurse-submodules .
  git pull
  make git-submodule
  make clean
  if [ $APPLE_FN_ENABLE = "yes" ]; then
      # Apple Fn/Globe key patch
      # see https://gist.github.com/fauxpark/010dcf5d6377c3a71ac98ce37414c6c4
      patch -p1 < "${PROJECT}/patches/applefn.patch"
  fi
fi

make -j 8 $MAKE_TARGETS[*] VIAL_ENABLE=${VIAL_ENABLE} APPLE_FN_ENABLE=${APPLE_FN_ENABLE}


VERSION="$(date +"%Y%m%d")_$(git rev-parse --short HEAD)"
if [ $VIAL_ENABLE = "yes" ]; then
  VERSION="vial_$VERSION"
else
  VERSION="via_$VERSION"
fi
if [ $APPLE_FN_ENABLE = "yes" ]; then
  VERSION="applefn_$VERSION"
fi

# dist
# -----------------------------------
cd "$PROJECT/dist"

for target in $TARGETS; do
  # split ":" [1]=make target [2]=extension
  kbd=(${(@s/:/)KEYBOARDS[$target]})
  mv $BUILD_FIRMWARES[$target] ${target}_$VERSION.$kbd[2]
done
