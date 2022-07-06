#!/bin/bash -eu

# configuration
KEYBOARDS=(bakeneko60 ciel60 qk65 prime_e d60 fk680)
QMK_HOME="$HOME/Documents/Sources/vial-qmk"
KEYMAP="vial"
# QMK_HOME="$HOME/Documents/Sources/qmk_firmware"
# KEYMAP="via"
QMK_UPDATE=true

# PROJECT
# -----------------------------------
cd "$(dirname $0)"

# .config can override QMK_HOME, KEYMAP, KEYBOARDS, QMK_UPDATE
if [ -s .config ]; then
  source .config
fi

# argument can override KEYBOARDS
[ $# -ge 1 ] && KEYBOARDS=("$@")

PROJECT=$(pwd)
mkdir -p dist

# QMK_HOME
# -----------------------------------
cd "$QMK_HOME"
if $QMK_UPDATE; then
  # checkout to revert changes.
  git checkout --recurse-submodules .
  git pull
  make git-submodule
  make clean
  # Apple Fn/Globe key patch
  # see https://gist.github.com/fauxpark/010dcf5d6377c3a71ac98ce37414c6c4
  patch -p1 < "${PROJECT}/patches/applefn.patch"
fi

# change QMK_HOME directory
CURRENT_QMK_HOME=$(qmk config -ro user.qmk_home)
CURRENT_QMK_HOME=${CURRENT_QMK_HOME/"user.qmk_home="/}
if [ $QMK_HOME != $CURRENT_QMK_HOME ]; then
  qmk --verbose setup -y -H .
fi

# filename format  KEYMAP   BUILD DATE         QMK/VIAL-QMK REVISION
VERSION="applefn_${KEYMAP}_$(date +"%Y%m%d")_$(git rev-parse --short HEAD)"

if [ ! -L "$QMK_HOME/keyboards/my_keyboards" ]; then
  ln -s "${PROJECT}/keyboards" keyboards/my_keyboards
fi

# dist
# -----------------------------------
cd "$PROJECT/dist"

if [[ " ${KEYBOARDS[*]} " =~ " bakeneko60 " ]]; then
  qmk compile -j 8 -kb my_keyboards/bakeneko60 -km $KEYMAP
  mv "$QMK_HOME/my_keyboards_bakeneko60_$KEYMAP.hex" bakeneko60_$VERSION.hex
fi

if [[ " ${KEYBOARDS[*]} " =~ " ciel60 " ]]; then
  qmk compile -j 8 -kb my_keyboards/ciel60 -km $KEYMAP
  mv "$QMK_HOME/my_keyboards_ciel60_$KEYMAP.hex" ciel60_$VERSION.hex
fi

if [[ " ${KEYBOARDS[*]} " =~ " qk65 " ]]; then
  qmk compile -j 8 -kb my_keyboards/qk65/solder -km $KEYMAP
  mv "$QMK_HOME/my_keyboards_qk65_solder_$KEYMAP.hex" qk65_solder_$VERSION.hex
fi

if [[ " ${KEYBOARDS[*]} " =~ " prime_e " ]]; then
  qmk compile -j 8 -kb my_keyboards/prime_e/rgb -km $KEYMAP
  mv "$QMK_HOME/my_keyboards_prime_e_rgb_$KEYMAP.hex" prime_e_rgb_$VERSION.hex
fi

if [[ " ${KEYBOARDS[*]} " =~ " d60 " ]]; then
  qmk compile -j 8 -kb my_keyboards/dz60rgb_wkl/v2_1 -km $KEYMAP
  mv "$QMK_HOME/my_keyboards_dz60rgb_wkl_v2_1_$KEYMAP.bin" d60_hhkb_$VERSION.bin
fi

if [[ " ${KEYBOARDS[*]} " =~ " fk680 " ]]; then
  qmk compile -j 8 -kb my_keyboards/fk680pro_v2 -km $KEYMAP
  mv "$QMK_HOME/my_keyboards_fk680pro_v2_$KEYMAP.uf2" fk680pro_v2_$VERSION.uf2
fi
