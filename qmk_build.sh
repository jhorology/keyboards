#!/bin/zsh -eu

# PROJECT
# -----------------------------------
PROJECT=$(dirname "$(realpath $0)")
cd $PROJECT

zparseopts -D -E -F -- \
           {h,-help}=help  \
           -setup-qmk=setup_qmk  \
           -setup-via=setup_via  \
           -setup-project=setup_project  \
           -setup=setup \
           -pip-upgrade=pip_upgrade \
           -clean=clean \
           -clean-qmk=clean_qmk \
           -clean-via=clean_via \
           -clean-all=clean_all \
           -format=format \
           -scp-secure-config=scp_secure_config \
           -via_json=via_json \
           -via-app=via_app \
           {w,-without-update}=without_update \
           {p,-without-patch}=without_patch \
           {-without-via-json}=without_via_json \
           {d,-with-compile-db}=with_compile_db \
           {f,-with-flash}=with_flash \
           -without-emacs=without_emacs \
  || return


# configuration
# -----------------------------------
# <make target name>:<keymap>:<extension of firmware file>:DFU Storage Volume

# configurable
local -A KEYBOARDS=(
  n60        bakeneko60:default:hex:03eb:2ff4
  c60        ciel60:default:hex:03eb:2ff0
  b60        dropbear60_solder:default:hex:03eb:2ff4
  d60        dz60rgb_wkl_v2_1_atmel_dfu:hhkb:hex:03eb:2ff4
  fk68       fk680pro_v2:default:uf2:"ZhaQian DFU"
  i68        ikki68_aurora:default:hex:03eb:2ff4
  k6         k6_pro_ansi_rgb:default:bin:0483:df11
  libra      libra_mini:default:hex:2341:0036
  prime_e    prime_e_rgb:default:hex:03eb:2ff4
  q60        q60_ansi:default:bin:0483:df11
  qk60       qk60:default:bin:1688:2220
  qk65       qk65_solder:default:hex:03eb:2ff4
  tx60       tetrix60:default:hex:03eb:2ff4
  tf60       tofu60_2_0:default:uf2:"RPI-RP2"
  w60        dz60rgb_wkl_v2_1_atmel_dfu:tsangan:hex:03eb:2ff4
  z65        zoom65:default:hex:03eb:2ff4
)
TARGETS=(n60 c60 b60 d60 fk68 i68 libra prime_e q60 qk60 qk65 tx60 tf60 w60 z65)

WITH_UPDATE=true
WITH_PATCH=true
WITH_VIA_JSON=true
WITH_EMACS=true
VIA_APP_BRANCH=main
MAKE_JOBS=$(nproc)

MAIN_DEV_HOST=$(uname -n)

# .config
#  override configuration
# -----------------------------------
[ -s .qmk_config ] &&  source .qmk_config


VIA_APP_BRANCH="main"
PLATFORM=$(uname)
ARCHITECTURE=$(uname -m)

error_exit() {
  print -r "Error: $2" >&2
  exit $1
}

# prefix for platform spcific functions
case $PLATFORM in
  Darwin )
    os=macos
    ;;
  Linux )
    if [[ -f /etc/fedora-release ]] && env | grep -q WSL_INTEROP; then
      # fedora on WSL2
      os=fedora
    else
      # TODO
      error_exit 1 'unsupported platform.'
    fi
    ;;
  * )
    error_exit 1 'unsupported platform.'
    ;;
esac

_this_script=$0
help_usage() {
  print -rC1 -- \
        "" \
        "usage:" \
        "   $_this_script:t <-h|--help>                       show this help" \
        "   $_this_script:t --setup-qmk                       setup qmk_firmware & build tools" \
        "   $_this_script:t --setup-via                       setup via/app" \
        "   $_this_script:t --setup-project                   setup project node_modules" \
        "   $_this_script:t --setup                           setup all of above" \
        "   $_this_script:t --pip-upgrade                     update python packages" \
        "   $_this_script:t --clean                           clean generated files and temporary files" \
        "   $_this_script:t --clean-qmk                       clean qmk_firmware and build tools" \
        "   $_this_script:t --clean-via                       clean via/app" \
        "   $_this_script:t --clean-all                       clean all of abopve" \
        "   $_this_script:t --format                          apply formatter" \
        "   $_this_script:t --scp-secure-config               copy secure_config.h from MAIN_DEV_HOST" \
        "   $_this_script:t --via-json                        generate VIA JSON files" \
        "   $_this_script:t --via-app [options...] TARGET     launch via/app" \
        "   $_this_script:t [options...] [<TARGET...>]        build firmwares" \
        "" \
        "via/app options:" \
        "  -w,--without-update             don't sync remote repository" \
        "  -p,--without-patch              don't apply patches" \
        "  --without-via_json              don't generate via JSON, use JSON file in dist folder" \
        "" \
        "build options:" \
        "  -w,--without-update             don't sync remote repository" \
        "  -p,--without-patch              don't apply patches" \
        "  --without-via-json              don't generate via JSON, use JSON file in dist folder" \
        "  -d,--with-compile-db            generate compile_command.json" \
        "  -f,--with-flash                 post build flash firmware" \
        "  --without-emacs                 don't generate emacs settings when --with-comile-db" \
        "" \
        "available targets:"
  for target in ${(k)KEYBOARDS}; do
    print -rC2 -- "   ${target}:"  "${KEYBOARDS[$target]}"
  done
}


# setup build environment & qmk_firmware
# -----------------------------------
macos_setup_qmk() {
  brew update
  # install 'qmk' in .venv
  packages=$(brew info qmk/qmk/qmk | grep "Required:")
  packages=${packages//Required: /}
  packages=(${(s[, ])packages})
  brew install $packages[*]
  brew cleanup
}

fedora_setup_qmk() {
  sudo dnf -y install \
       clang diffutils git gcc glibc-headers kernel-devel kernel-headers \
       make unzip wget zip python3 avr-binutils avr-gcc avr-gcc-c++ avr-libc \
       arm-none-eabi-binutils-cs arm-none-eabi-gcc-cs arm-none-eabi-gcc-cs-c++ \
       arm-none-eabi-newlib avrdude dfu-programmer dfu-util hidapi \
       usbip hwdata

  sudo dnf -y install libusb-devel \
    || sudo dnf -y install libusb1-devel libusb-compat-0.1-devel \
    || sudo dnf -y install libusb0-devel

  sudo dnf autoremove
  sudo dnf clean all

  # on windows:
  # see https://learn.microsoft.com/en-us/windows/wsl/connect-usb
  winget=$(/mnt/c/Windows/System32/cmd.exe /C "where winget" 2> /dev/null || true)
  if [[ ! -z $winget ]]; then
    winget=${winget%$'\r'}
    winget=$(wslpath -u $winget)
    $winget install dorssel.usbipd-win || true
    $winget install gerardog.gsudo || true
  fi
}

setup_qmk() {
  cd $PROJECT
  if [[ ! -d qmk_firmware ]]; then
    git clone --depth=1 -b master --recurse-submodules --shallow-submodules --sparse https://github.com/qmk/qmk_firmware.git
    for f in $(find $PROJECT/qmk_firmware -name .gitmodules); do
      cd $(dirname $f)
      for m in $(grep "path = " .gitmodules | awk '{print $3}');
      do
        git config -f .gitmodules submodule.lib/$m.shallow true
      done
    done
    cd $PROJECT/qmk_firmware
    cat <<EOF >> .git/info/sparse-checkout
/builddefs/
/data/
/drivers/
/lib/
/layouts/
/lib/
/platforms/
/quantum/
/tmk_core/
/util/
EOF
    git sparse-checkout reapply
    make git-submodules
  fi

  # ignore aliases
  echo "{}" > $PROJECT/qmk_firmware/data/mappings/keyboard_aliases.hjson

  cd $PROJECT
  ${os}_setup_qmk

  if [[ ! -d .venv ]]; then
    python3 -m venv .venv
  fi
  source .venv/bin/activate
  pip3 install qmk
  pip3 install -r qmk_firmware/requirements.txt
  pip3 install -r qmk_firmware/requirements-dev.txt
  pip3 cache purge
}

setup_project() {
  cd $PROJECT
  npm install
}

setup_via() {
  cd $PROJECT
  if [[ ! -d via_app ]]; then
    git clone --depth 1 -b $VIA_APP_BRANCH https://github.com/the-via/app.git via_app
    cd via_app
    for patch in $(ls -v $PROJECT/patches/via_app_${VIA_APP_BRANCH}_*.patch); do
      git apply -3 --verbose $patch
    done
    npx yarn install
  fi
}

pip_upgrade() {
  cd $PROJECT
  source .venv/bin/activate
  pip3 --disable-pip-version-check list --outdated --format=json | \
    python3 -c "import json, sys; print('\n'.join([x['name'] for x in json.load(sys.stdin)]))" | \
    xargs -n1 pip install -U
}

revert_qmk_changes() {
  cd $PROJECT/qmk_firmware
  rm -rf keyboards
  make clean
  git reset --hard HEAD
  git clean -dfx
  git submodule foreach --recursive git reset --hard HEAD
  git submodule foreach --recursive git clean -dfx
}

clean() {
  cd $PROJECT
  rm -rf dist
  find qmk_keyboards -name '*~' -exec rm -f {} \;
  find qmk_keyboards -name '.DS_Store' -exec rm -f {} \;
  revert_qmk_changes
}

clean_qmk() {
  cd $PROJECT
  rm -rf .venv
  rm -rf qmk_firmware
  # brewq uninstall qmk/qmk/qmk
}

clean_via() {
  cd $PROJECT
  rm -rf via_app
}

clean_all() {
  clean
  clean_qmk
  clean_via
}

format() {
  cd $PROJECT
  find qmk_keyboards -name "*.h" -exec clang-format -i {} \;
  find qmk_keyboards -name "*.c" -exec clang-format -i {} \;
  npx prettier --write qmk_keyboards/**/*.{json,js} dist/**/*.json
}

# return bool: whether should apply patches or not
should_apply_qmk_patch() {
  cd $PROJECT/qmk_firmware
  if $KEYCHRON_BT; then
    if [[ ! -f keychron_bluetooth_playground ]]; then
      true; return
    fi
  else
    if [[ -f keychron_bluetooth_playground ]]; then
      true; return
    fi
  fi
  for patch in $(ls -v $PROJECT/patches/qmk_*.patch); do
    patched=${${patch##*/}%.*}
    if [[ ! -f $patched ]]; then
      true; return
    fi
  done
  false
}

apply_qmk_patch() {
  cd $PROJECT/qmk_firmware
  if $KEYCHRON_BT; then
    if [[ ! -f keychron_bluetooth_playground ]]; then
      git apply -3 --verbose $PROJECT/patches/keychron_bluetooth_playground.patch
      touch keychron_bluetooth_playground
    fi
    if [[ $TARGETS[(I)$TARGETS[-1]] -ge 2 ]]; then
      error_exit 1 "Error: Can't compile k6 together with other keyboards."
    fi
  fi
  for patch in $(ls -v $PROJECT/patches/qmk_*.patch); do
    patched=${${patch##*/}%.*}
    if [[ ! -f $patched ]]; then
      git apply -3 --verbose $patch
      touch $patched
    fi
  done
}

update_qmk() {
  revert_qmk_changes
  cd $PROJECT/qmk_firmware
  git pull
  # ignore aliases
  echo "{}" > $PROJECT/qmk_firmware/data/mappings/keyboard_aliases.hjson
  make git-submodules
}

macos_uf2_flash() {
  firmware=$1
  volume_name=$2

  dfu_volume=/Volumes/$volume_name
  if [[ -d $dfu_volume  ]]; then
    echo
    echo "copying firmware [${firmware}] to volume [${dfu_volume}]..."
    sleep 1
    cp $firmware $dfu_volume
    true
  else
    false
  fi
}

fedora_uf2_flash() {
  firmware=$1
  volume_name=$2

  dfu_drive=$(/mnt/c/Windows/System32/wbem/WMIC.exe logicaldisk get deviceid, volumename | grep $volume_name | awk '{print $1}')
  if [[ ! -z $dfu_drive ]]; then
    echo
    echo "copying firmware [${firmware}] to drive [${dfu_drive}]..."
    sleep 1
    /mnt/c/Program\ Files/gsudo/Current/gsudo.exe  c:\\Windows\\System32\\xcopy.exe $(wslpath -w $firmware) $dfu_drive\\
    true
  else
    false
  fi
}

# $1 target
build_firmware() {
  target=$1

  kbd=(${(@s/:/)KEYBOARDS[$target]})
  kb=$kbd[1]
  km=$kbd[2]
  ext=$kbd[3]
  make_target=$kb:$km
  cd $PROJECT/qmk_firmware
  if [[ $ext != "uf2" ]] && (( $#with_flash )); then
    vid=$kbd[4]
    pid=$kbd[5]
    make_target=${make_target}:flash
    if [[ $os = "fedora" ]]; then
      # sudo for later use
      sudo echo -n
      DFU_HARDWARE_ID=$vid:$pid make -j $MAKE_JOBS $make_target \
                     DFU_UTIL=$PROJECT/util/dfu_util_wsl_helper \
                     DFU_PROGRAMMER=$PROJECT/util/dfu_programmer_wsl_helper
    else
      make -j $MAKE_JOBS $make_target
    fi
  else
    make -j $MAKE_JOBS $make_target
  fi

  # <build date>_qmk_<qmk version>_<qnk revision>
  # version="$(date +"%Y%m%d")_qmk_$(git describe --abbrev=0 --tags)_$(git rev-parse --short HEAD)"
  # <build date>_qmk_<qnk revision>
  version=$(date +"%Y%m%d")_qmk_$(git rev-parse --short HEAD)
  src=${kb//\//_}_${km}.${ext}
  firmware=$PROJECT/dist/${kb//\//_}
  if [ $km != "default" ]; then
    firmware=${firmware}_$km
  fi

  firmware=${firmware}_$version.$ext
  mkdir -p $PROJECT/dist
  cp $src $firmware

  if (( $#with_flash )) && [[ $ext == "uf2" ]]; then
    volume_name=$kbd[4]
    echo -n "waiting for DFU volume to be mounted..."
    for ((i=0; i < 20; i+=1)); do
      if ${os}_uf2_flash $firmware $volume_name; then
        echo "flashing firmware finished successfully."
        break
      else
        echo -n "."
        sleep 1
      fi
    done
  fi
}

dot_clangd() {
  cat <<EOS > $PROJECT/.clangd
CompileFlags:
  Remove: [-mcall-prologues]
EOS
  rm -rf $PROJECT/.cache
}

dot_dir_locals() {
  cat <<EOS > $PROJECT/.dir-locals.el
((nil . ((projectile-git-use-fd . t)
         (projectile-git-fd-args . "--hidden --no-ignore -0 --exclude '\.*' --type f --strip-cwd-prefix")
         (counsel-rg-base-command . ("rg" "--no-ignore" "--max-columns" "240" "--with-filename" "--no-heading" "--line-number" "--color" "never" "%s")))))
EOS
}

dot_projectile() {
  cat <<EOS > $PROJECT/.projectile
-/dist
-/build
-/modules
-/node_modules
-/via_app
-/zephyr
-/zmk
-/zmk_keyboards
EOS
}

compile_db() {
  target=$1
  kbd=(${(@s/:/)KEYBOARDS[$target]})
  kb=$kbd[1]
  km=$kbd[2]
  cd $PROJECT
  qmk generate-compilation-database -kb $kb -km $km
  cat qmk_firmware/compile_commands.json | sed -E "s|(-I\|-include )keyboards(/.)?|\1$PROJECT/qmk_keyboards|g" > compile_commands.json
  rm -f qmk_firmware/compile_commands.json
  npx prettier --write compile_commands.json

  # make_target=$kb:$km
  # cd $PROJECT/qmk_firmware
  # compile_commands=$(make -j --dry-run $make_target | sed -n -r 's/^LOG=\$\(([a-z\-]+gcc .* -o [^ ]*).*$/\1/p')
  # echo "["
  # for c in ${(f)compile_commands}; do
  #   # c=$(eval print $c)
  #   c=$(echo $c | sed -r "s|keyboards|${PROJECT}/qmk_keyboards|g")
  #   f=$(echo $c | sed -r 's/.* ([^ ]*\.[cS]) -o .*$/\1/')
  #   o=$(echo $c | sed -n -r 's/.* -o (.*)$/\1/p')
  #   node -e "let v=process.argv;console.log(JSON.stringify({directory:v[1],command:v[2],file:v[3],output:v[4]}, null, 2)+',')" \
  #        "${PROJECT}/qmk_firmware" $c $f $o
  # done
  # echo "]"
}

scp_secure_config() {
  cd $PROJECT
  project=$(realpath --relative-to="$HOME" .)

  [[ -s .qmk_config ]] || \
    error_exit 1 "Missing .config file."

  [[ ${MAIN_DEV_HOST-} ]] || \
    error_exit 1 "MAIN_DEV_HOST is not defined."

  [[ $(uname -n) = $MAIN_DEV_HOST ]] && \
    error_exit 1 "this is MAIN_DEV_HOST."

  for target in qmk_keyboards/*/; do
    if [ $target != "qmk_keyboards/lib/" ]; then
      scp $MAIN_DEV_HOST:"~/${PROJECT}/${target}secure_config.h" "${target}"
    fi
  done
}

build_via_json_files() {
  cd $PROJECT
  targets=()
  for target in $TARGETS; do
    kbd=(${(@s/:/)KEYBOARDS[$target]})
    kb=$kbd[1]
    km=$kbd[2]
    targets=($targets $kb:$km)
  done
  # generate via json file
  util/generate_via_json $targets[*]
  npx prettier --write dist/**/*.json
}

# $1 target
build_via_json() {
  target=$1
  kbd=(${(@s/:/)KEYBOARDS[$target]})
  kb=$kbd[1]
  km=$kbd[2]

  # generate via json file
  $PROJECT/util/generate_via_json $kb:$km
}

run_via_app() {
  target=$TARGETS[1]
  kbd=(${(@s/:/)KEYBOARDS[$target]})
  kb=$kbd[1]
  km=$kbd[2]

  setup_via

  cd $PROJECT/via_app
  local_rev=$(git rev-parse $VIA_APP_BRANCH)
  remote_rev=$(git ls-remote --heads origin $VIA_APP_BRANCH | awk '{print $1}')

  # update via/app
  #______________________________________
  if [[ $local_rev != $remote_rev ]]; then
    git reset --hard HEAD
    cp package.json package.json.old
    git pull
    for patch in $(ls -v $PROJECT/patches/via_app_${VIA_APP_BRANCH}_*.patch); do
      git apply -3 --verbose $patch
    done
    if [[ ! -z $(diff package.json package.json.old) ]]; then
      npx yarn install
    fi
    rm package.json.old
  fi

  # generate JSON
  #______________________________________
  cd $PROJECT
  if $WITH_VIA_JSON; then
    mkdir -p dist
    build_via_json $target
    npx prettier --write dist/**/*.json
  fi

  # clean JSON files in via-keyboards
  #______________________________________
  cd $PROJECT/via_app
  rm -rf public/definitions
  rm -rf node_modules/via-keyboards/src
  rm -rf node_modules/via-keyboards/v3
  mkdir node_modules/via-keyboards/src
  mkdir node_modules/via-keyboards/v3

  # copy JSON files into via-keyboards
  #______________________________________
  cp $PROJECT/dist/${kb}_*.json node_modules/via-keyboards/v3

  # start VIA
  #______________________________________
  npx yarn dev
}

sub_commands() {
  if (( $#help )); then
    help_usage; exit 0
  elif (( $#setup_qmk )); then
    setup_qmk; exit 0
  elif (( $#setup_via )); then
    setup_via; exit 0
  elif (( $#setup_project )); then
    setup_project; exit 0
  elif (( $#setup )); then
    setup_qmk
    setup_via
    setup_project
    exit 0
  elif (( $#pip_upgrade )); then
    pip_upgrade; exit 0
  elif (( $#clean )); then
    clean; exit 0
  elif (( $#clean_qmk )); then
    clean_qmk; exit 0
  elif (( $#clean_via )); then
    clean_via; exit 0
  elif (( $#clean_all )); then
    clean
    clean_via
    clean_qmk
    exit 0
  elif (( $#format )); then
    format; exit 0
  elif (( $#scp_secure_config )); then
    scp_secure_config; exit 0
  elif (( $#via_json )); then
    build_via_json_files; exit 0
  elif (( $#via_app )); then
    run_via_app; exit 0
  fi
}

# check & apply arguments
# -----------------------------------
(( $#@ )) && TARGETS=("$@")

[[ ${TARGETS[(I)k6]} != 0 ]] && KEYCHRON_BT=true || KEYCHRON_BT=false

if (( $#via_app )) || (( $#with_flash )); then
  [[ $# = 0 ]] && error_exit 1 "Missing target argument."
  [[ $# != 1 ]] && error_exit 1 "Only one target is allowed."
fi
# keychron bluetooth playground
if $KEYCHRON_BT; then
  [[ $# -ge 2 ]] && \
    error_exit 1 "Can't compile k6 together with other keyboards."
fi
(( $#without_update )) && WITH_UPDATE=false
(( $#without_patch )) && WITH_PATCH=false
(( $#without_via_json )) && WITH_VIA_JSON=false
(( $#without_emacs )) && WITH_EMACS=false

# auto setup
# -----------------------------------
if [[ ! -d $PROJECT/qmk_firmware ]]; then
  setup_qmk
  WITH_UPDATE=true
fi

if [[ ! -d $PROJECT/node_modules ]]; then
  setup_project
fi

if [[ $(which python3) != $PROJECT/.venv/bin/python3 ]]; then
  source .venv/bin/activate
fi

# some qmk command are called from make process.
qmk config user.qmk_home=${PROJECT}/qmk_firmware

sub_commands

$WITH_PATCH && \
  should_apply_qmk_patch && \
  WITH_UPDATE=true

$WITH_UPDATE && update_qmk
$WITH_PATCH && apply_qmk_patch

rm -f $PROJECT/qmk_firmware/keyboards
ln -s $PROJECT/qmk_keyboards $PROJECT/qmk_firmware/keyboards

for target in $TARGETS; do
  if (( $#with_compile_db )); then
    compile_db $target
    dot_clangd
    if $WITH_EMACS; then
      dot_dir_locals
      dot_projectile
    fi
  fi
  build_firmware $target
done

$WITH_VIA_JSON && build_via_json_files
