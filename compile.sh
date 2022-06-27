#!/bin/bash -eu

QMK_HOME="$HOME/Documents/Sources/vial-qmk"
KEYMAP="vial"
# QMK_HOME="$HOME/Documents/Sources/qmk_firmware"
# KEYMAP="via"


cd "$(dirname $0)"
PROJECT=$(pwd)
SUFFIX="$(date +"%Y%m%d")_applefn_${KEYMAP}"
mkdir -p dist

cd "$QMK_HOME"
# checkout to revert changes.
git checkout .
qmk setup -H "$QMK_HOME"
git pull
make git-submodule
make clean

# Apple Fn/Globe key patch
# see https://gist.github.com/fauxpark/010dcf5d6377c3a71ac98ce37414c6c4
patch -p1 < "${PROJECT}/patches/applefn.patch"

if [ ! -L ${QMK_HOME}/keyboards/my_keyboards ]; then
  ln -s "${PROJECT}/keyboards" keyboards/my_keyboards
fi
cd "${PROJECT}/dist"

qmk compile -j 4 -kb my_keyboards/bakeneko60 -km "$KEYMAP"
mv "$QMK_HOME/my_keyboards_bakeneko60_$KEYMAP.hex" bakeneko60_${SUFFIX}.hex

qmk compile -j 4 -kb my_keyboards/ciel60 -km "$KEYMAP"
mv "$QMK_HOME/my_keyboards_ciel60_$KEYMAP.hex" "ciel60_${SUFFIX}.hex"

qmk compile -j 4 -kb my_keyboards/qk65/solder -km "$KEYMAP"
mv "$QMK_HOME/my_keyboards_qk65_solder_$KEYMAP.hex" qk65_solder_${SUFFIX}.hex

qmk compile -j 4 -kb my_keyboards/prime_e/rgb -km "$KEYMAP"
mv "$QMK_HOME/my_keyboards_prime_e_rgb_$KEYMAP.hex" prime_e_rgb_${SUFFIX}.hex

qmk compile -j 4 -kb my_keyboards/dz60rgb_wkl/v2_1 -km "$KEYMAP"
mv "$QMK_HOME/my_keyboards_dz60rgb_wkl_v2_1_$KEYMAP.bin" dz60rgb_wkl_v2_1_${SUFFIX}.bin
