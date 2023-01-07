#!/bin/zsh -eu

# PROJECT
# -----------------------------------
PROJECT=$(realpath $0:h)
cd "$PROJECT"
zparseopts -D -E -F -- \
           {h,-help}=help  \
           {c,-clean}=clean \
           {f,-flash}=flash \
           {H,-qmk-home}:=qmk_home \
           {w,-without-update-qmk}=without_update_qmk \
  || return


# configuration
# -----------------------------------
# <make target name>:<keymap>:<extension of firmware file>
local -A KEYBOARDS=(
  neko60     bakeneko60:default:hex
  ciel60     ciel60:default:hex
  d60        dz60rgb_wkl_v2_1_atmel_dfu:hhkb:hex
  fk680      fk680pro_v2:default:uf2
  ikki68     ikki68_aurora:default:hex
  k6         k6_pro_ansi_rgb:default:bin
  libra      libra_mini:default:hex
  prime_e    prime_e_rgb:default:hex
  q60        q60_ansi:default:bin
  qk60       qk60:default:bin
  qk65       qk65_solder:default:hex
  t60        tetrix60:default:hex
  wood60     dz60rgb_wkl_v2_1_atmel_dfu:tsangan:hex
  zoom65     zoom65:default:hex
)
TARGETS=(neko60 ciel60 d60 fk680 ikki68 libra prime_e q60 qk60 qk65 t60 wood60 zoom65)
QMK_HOME="$HOME/Documents/Sources/qmk_firmware"
UPDATE_QMK=true
MAKE_JOBS=8
[ $(uname) = "Darwin" ] && MAKE_JOBS=$(sysctl -n hw.ncpu)

# .config
#  override configuration
# -----------------------------------
[ -s .config ] &&  source .config

# help usage
# -----------------------------------
if (( $#help )); then
  print -rC1 --      \
        "" \
        "usage:" \
        "   $0:t [-h|--help]                            show this help" \
        "   $0:t [-c|--clean] [--qmk-home <QMK_HOME>]   clean build env" \
        "   $0:t [options...] [<TARGET...>]             build firmwares" \
        "   $0:t [-f|--flash] [options...] <TARGET>     build and flash firmware" \
        "" \
        "options:" \
        "  -H,--qmk-home <QMK_HOME>            location for local qmk_firmware repository" \
        "  -w,--without-update-qmk             don't sync remote repository" \
        "" \
        "available targets:"
  for target in $TARGETS; do
    print -rC2 -- "   ${target}:"  "${KEYBOARDS[$target]}"
  done
  return
fi



# targets
# -----------------------------------
(( $#@ )) && TARGETS=("$@")
MAKE_TARGETS=()
local -A VIA_JSON_TARGETS=()
TARGET_COUNT=$TARGETS[(I)$TARGETS[-1]]

if (( $#flash )); then
  if [[ $# = 0 ]]; then
    print -r "Error: Missing target argument." >&2
    exit 1
  fi
  if [[ $# != 1 ]]; then
    print -r "Error: Only one target is allowed." >&2
    exit 1
  fi
fi

KEYCHRON_BT=false
for target in $TARGETS; do
  kbd=(${(@s/:/)KEYBOARDS[$target]})
  MAKE_TARGET="my_keyboards/${kbd[1]}:${kbd[2]}"
  if (( $#flash )); then
    MAKE_TARGET="${MAKE_TARGET}:flash"
  fi
  MAKE_TARGETS=($MAKE_TARGETS $MAKE_TARGET)
  VIA_JSON_TARGETS[$kbd[1]]=$kbd[1]
  if [[ $target = "k6" ]]; then
    if [[ $TARGET_COUNT -ge 2 ]]; then
      print -r "Error: Can't compile k6 together with other keyboards." >&2
      exit 1
    fi
    KEYCHRON_BT=true
  fi
done



# option parameters
# -----------------------------------
(( $#qmk_home )) && QMK_HOME=${qmk_home[-1]##=}
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
  [[ ! -f keychron_bluetooth_playground ]] && UPDATE_QMK=true
else
  [[ -f keychron_bluetooth_playground ]] && UPDATE_QMK=true
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
  if [[ ! -f keychron_bluetooth_playground ]]; then
    patch --verbose -p1 < "${PROJECT}/patches/keychron_bluetooth_playground.patch"
    touch keychron_bluetooth_playground
  fi
fi

for patch in $(ls -v "${PROJECT}/patches/"qmk_*.patch); do
  patched=${${patch##*/}%.*}
  if [[ ! -f "${patched}" ]]; then
    patch --verbose -p1 < "${patch}"
    touch "${patched}"
  fi
done

# make
# -----------------------------------
rm -f keyboards/my_keyboards
ln -s "${PROJECT}/qmk_keyboards" keyboards/my_keyboards

make -j $MAKE_JOBS $MAKE_TARGETS[*]

VERSION="$(date +"%Y%m%d")_qmk_$(git describe --abbrev=0 --tags)_$(git rev-parse --short HEAD)_via"

# generate via json file
QMK_HOME="$QMK_HOME" \
  "$PROJECT/util/generate_via_json.js" $VIA_JSON_TARGETS[*]

# dist
# -----------------------------------
cd "$PROJECT/dist"

for target in $TARGETS; do
  # split ":" [1]=make target [2]=keymap [3]=extension
  kbd=(${(@s/:/)KEYBOARDS[$target]})
  board=${kbd[1]//\//_}
  firmware_name=$board
  if [ $kbd[2] != "default" ]; then
    firmware_name=${firmware_name}_${kbd[2]}
  fi
  mv "$QMK_HOME/my_keyboards_${board}_${kbd[2]}.${kbd[3]}" ${firmware_name}_$VERSION.$kbd[3]
done

# formatter
# -----------------------------------
cd "$PROJECT"
./format.sh
