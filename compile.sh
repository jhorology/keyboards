#!/bin/bash -eu

QMK_HOME="$HOME/Documents/Sources/vial-qmk"
KEYMAP="vial"
# QMK_HOME="$HOME/Documents/Sources/qmk_firmware"
# KEYMAP="via"


cd "$(dirname $0)"
PROJECT=$(pwd)
mkdir -p dist

cd "$QMK_HOME"
# checkout to revert changes.
git checkout .
git pull
QMK_REV=$(git rev-parse --short HEAD)
make git-submodule
make clean

VERSION="applefn_${KEYMAP}_$(date +"%Y%m%d")_$(git rev-parse --short HEAD)"

qmk setup -H "$QMK_HOME"

# Apple Fn/Globe key patch
# see https://gist.github.com/fauxpark/010dcf5d6377c3a71ac98ce37414c6c4
patch -p1 < "${PROJECT}/patches/applefn.patch"

if [ ! -L ${QMK_HOME}/keyboards/my_keyboards ]; then
  ln -s "${PROJECT}/keyboards" keyboards/my_keyboards
fi
cd "${PROJECT}/dist"

qmk compile -j 4 -kb my_keyboards/bakeneko60 -km "$KEYMAP"
mv "$QMK_HOME/my_keyboards_bakeneko60_$KEYMAP.hex" bakeneko60_${VERSION}.hex

qmk compile -j 4 -kb my_keyboards/ciel60 -km "$KEYMAP"
mv "$QMK_HOME/my_keyboards_ciel60_$KEYMAP.hex" ciel60_${VERSION}.hex

qmk compile -j 4 -kb my_keyboards/qk65/solder -km "$KEYMAP"
mv "$QMK_HOME/my_keyboards_qk65_solder_$KEYMAP.hex" qk65_solder_${VERSION}.hex

qmk compile -j 4 -kb my_keyboards/prime_e/rgb -km "$KEYMAP"
mv "$QMK_HOME/my_keyboards_prime_e_rgb_$KEYMAP.hex" prime_e_rgb_${VERSION}.hex

qmk compile -j 4 -kb my_keyboards/dz60rgb_wkl/v2_1 -km "$KEYMAP"
mv "$QMK_HOME/my_keyboards_dz60rgb_wkl_v2_1_$KEYMAP.bin" dz60rgb_wkl_v2_1_${VERSION}.bin
