#!/bin/zsh -eu

# PROJECT
# -----------------------------------
PROJECT=$(realpath $0:h)
cd "$PROJECT"

zparseopts -D -E -F -- \
           {h,-help}=help \
           -qmk-home:=qmk_home \
           -via-app-home:=via_app_home \
           {u,-update-via-app}=update_via_app \
           {f,-fiber}=fiber \
           {w,-without-generate}=without_generate \
  || return


local -A KEYBOARDS=(
  n60      bakeneko60
  c60      ciel60
  d60      dz60rgb_wkl_v2_1_atmel_dfu
  fk68     fk680pro_v2
  i68      ikki68_aurora
  k6       k6_pro_ansi_rgb
  libra    libra_mini
  prime_e  prime_e_rgb
  q60      q60_ansi
  qk60     qk60
  qk65     qk65_solder
  t60      tetrix60
  w60      dz60rgb_wkl_v2_1_atmel_dfu
  z65      zoom65
)

# configuration
# -----------------------------------
QMK_HOME="$HOME/Documents/Sources/qmk_firmware"
VIA_APP_HOME="$HOME/Documents/Sources/via/app"
BUILD_JSON=true

# .config
#  override configuration
# -----------------------------------
[ -s .config ] &&  source .config

# help usage
# -----------------------------------
if (( $#help )); then
  print -rC1 --      \
        "Usage:" \
        "$0:t <-h|--help>                                                    help" \
        "$0:t <-u|--update-via-app> [--via-app-home VIA_APP_HOME] [--fiber]  update & setup via/app" \
        "$0:t [options...] TARGET                                            run via/app" \
        "" \
        "options:" \
        "  --qmk-home <QMK_HOME>            location for local qmk_firmware repository" \
        "  --via-app-home <VIA_APP_HOME>    location for local via/app repository" \
        "  -w,--without-generate            Use JSON file in dist folder without running builder" \
        "" \
        "available targets:"
  for target in ${(k)KEYBOARDS}; do
    print -rC2 -- "   ${target}:"  "${KEYBOARDS[$target]}"
  done
  return
fi

# option parameters
# -----------------------------------
(( $#qmk_home )) && QMK_HOME=${qmk_home[-1]##=}
(( $#via_app_home )) && VIA_APP_HOME=${via_app_home[-1]##=}
(( $#without_generate )) && BUILD_JSON=false

cd "$VIA_APP_HOME"
CURRENT_APP_BRANCH=$(git rev-parse --abbrev-ref HEAD)
LOCAL_APP_REV=$(git rev-parse $CURRENT_APP_BRANCH)
REMOTE_APP_REV=$(git ls-remote --heads origin $CURRENT_APP_BRANCH | awk '{print $1}')

# update via/app
#______________________________________
if (( $#update_via_app )); then
  git reset --hard HEAD
  git clean -dfx
  CURRENT_APP_BRANCH=$(git rev-parse --abbrev-ref HEAD)
  if (( $#fiber )) && [[ "$CURRENT_APP_BRANCH" != "fiber" ]]; then
    echo "main to fiber"
    git checkout fiber
    git pull --rebase
    CURRENT_APP_BRANCH="fiber"
  elif ! (( $#fiber )) && [[ "$CURRENT_APP_BRANCH" != "main" ]]; then
    echo "fiber to main"
    git checkout main
    git pull --rebase
    CURRENT_APP_BRANCH="main"
  else
    echo "normal"
    git pull
  fi

  for patch in $(ls -v "${PROJECT}/patches/via_app_${CURRENT_APP_BRANCH}_"*.patch); do
    patch --verbose -p1 < $patch
  done
  yarn install
  if (( $#fiber )); then
    yarn add @fortawesome/fontawesome-svg-core
  fi
  return
fi


if [[ $# = 0 ]]; then
  print -r "Error: Missing target argument." >&2
  exit 1
fi
if [[ $# != 1 ]]; then
  # becuase all my keyboards are assigned same Product/Vendor ID in mac mode.
  print -r "Error: Only one target is allowed." >&2
  exit 1
fi

# update via/app
#______________________________________
if [[ $LOCAL_APP_REV != $REMOTE_APP_REV ]]; then
  git reset --hard HEAD
  cp package.json package.json.old
  git pull
  for patch in $(ls -v "${PROJECT}/patches/via_app_${CURRENT_APP_BRANCH}_"*.patch); do
    patch --verbose -p1 < $patch
  done
  if [[ ! -z $(diff package.json package.json.old) ]]; then
    yarn install
  fi
  rm package.json.old
fi

# generate JSON
#______________________________________
cd "$PROJECT"

TARGET=$@
MAKE_TARGET=$KEYBOARDS[$TARGET]

if $BUILD_JSON; then
  mkdir -p dist
  # generate via json file
  QMK_HOME="$QMK_HOME" \
    "$PROJECT/util/generate_via_json.js" $MAKE_TARGET
fi

# clean JSON files in via-keyboards
#______________________________________
rm -rf node_modules/via-keyboards/src
rm -rf node_modules/via-keyboards/v3
mkdir node_modules/via-keyboards/src
mkdir node_modules/via-keyboards/v3

# copy JSON files into via-keyboards
#______________________________________
cp "dist/${MAKE_TARGET//\//_}_via"*.json node_modules/via-keyboards/v3

# convert JSON to via/app definition
#______________________________________
rm -rf "${VIA_APP_HOME}/public/definitions"

# TODO via-keybopards command rimraf dist folder
mv dist evacuate_dist
npx via-keyboards "${VIA_APP_HOME}/public/definitions"
mv evacuate_dist dist

# start VIA
#______________________________________
cd "$VIA_APP_HOME"

yarn start
