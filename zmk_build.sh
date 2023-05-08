#!/bin/zsh -eu

PROJECT=$(dirname "$(realpath $0)")
cd $PROJECT

# configuration
# -----------------------------------
HOST_OS=$(uname)
[[ $HOST_OS = Darwin ]] && HOST_OS=macos
[[ $HOST_OS = Linux ]] && HOST_OS=linux
HOST_ARCHITECTURE=$(uname -m)
[[ $HOST_OS = macos ]] && [[ $HOST_ARCHITECTURE = arm64 ]] && HOST_ARCHITECTURE=aarch64
ZEPHYR_VERSION=3.2.0
ZEPHYR_SDK_VERSION=0.15.2

# it is recommended to extract the Zephyr SDK bundle at one of the following default locations:
#
# $HOME
# $HOME/.local
# $HOME/.local/opt
# $HOME/bin
# /opt
# /usr/local
ZEPHYR_SDK_INSTALL_DIR=$HOME/.local

# Supported Toolchains:
#
# aarch64-zephyr-elf
# arc64-zephyr-elf
# arc-zephyr-elf
# arm-zephyr-eabi
# mips-zephyr-elf
# nios2-zephyr-elf
# riscv64-zephyr-elf
# sparc-zephyr-elf
# x86_64-zephyr-elf
# xtensa-espressif_esp32_zephyr-elf
# xtensa-espressif_esp32s2_zephyr-elf
# xtensa-intel_apl_adsp_zephyr-elf
# xtensa-intel_bdw_adsp_zephyr-elf
# xtensa-intel_byt_adsp_zephyr-elf
# xtensa-intel_s1000_zephyr-elf
# xtensa-nxp_imx_adsp_zephyr-elf
# xtensa-nxp_imx8m_adsp_zephyr-elf
# xtensa-sample_controller_zephyr-elf
TARGET_TOOLCHAINS=(arm-zephyr-eabi)

DOCKERFILE=$PROJECT/resources/Dockerfile.zmk-dev-arm.$(uname)
DOCKER_IMAGE=my/zmk-dev-arm:stable
CONTAINER_NAME=zmk-build
WITH_UPDATE=true
WITH_PATCH=true
WITH_EMACS=true
CONTAINER_WORKSPACE_DIR=/workspace
DOCSETS_DIR=$HOME/.docsets
# DOCSETS_DIR="$HOME/Library/Application Support/Dash/DockSets"

# key: target name [1]=board:[2]=firmwre_name:[3]=DFU volume name
local -A KEYBOARDS=(
  bt60       bt60:bt60_hhkb_ec11:CKP
)
TARGETS=(bt60)


cd $PROJECT
#  override configuration
# -----------------------------------
[ -s .zmk_config ] &&  source .zmk_config


THIS_SCRIPT=$0

# options
# -----------------------------------
zparseopts -D -E -F -- \
           {h,-help}=help  \
           -clean=clean \
           -clean-modules=clean_modules \
           -clean-tools=clean_tools \
           -clean-all=clean_all \
           -setup=setup \
           -setup-docker=setup_docker \
           -pip-upgrade=pip_upgrade \
           -zephyr-doc2dash=zephyr_doc2dash \
           {s,-docker-shell}=docker_shell \
           {d,-with-docker}=with_docker \
           {c,-with-clean}=with_clean \
           {g,-with-compile-db}=with_compile_db \
           {w,-without-update}=without_update \
           {p,-without-patch}=without_patch \
           {f,-with-flash}=with_flash \
           {l,-with-logging}=with_logging \
           -without-emacs=without_emacs \
  || return


# functions
# -----------------------------------
help_usage() {
  print -rC1 -- \
        "" \
        "Usage:" \
        "    $THIS_SCRIPT:t <-h|--help>                     help" \
        "    $THIS_SCRIPT:t --clean                         clean generated files" \
        "    $THIS_SCRIPT:t --clean-modules                 clean source moudules & generated files" \
        "    $THIS_SCRIPT:t --clean-tools                   clean zephyr sdk & build tools" \
        "    $THIS_SCRIPT:t --clean-all                     clean all" \
        "    $THIS_SCRIPT:t --setup                         setup zephyr sdk & projtect build tools" \
        "    $THIS_SCRIPT:t --setup-docker                  create docker image" \
        "    $THIS_SCRIPT:t --pip-upgrade                   upgrade python packages" \
        "    $THIS_SCRIPT:t --zephyr-doc2dash               generate zephyr docsets" \
        "    $THIS_SCRIPT:t <-s|--docker-shell>             enter docker container shell" \
        "    $THIS_SCRIPT:t [build options...] [TARGETS..]  build firmwares" \
        "" \
        "build options:" \
        "    -c,--with-clean                  clean up generated files" \
        "    -d,--with-docker                 build with docker" \
        "    -g,--with-compile-db             generate compile_command.json" \
        "    -w,--without-update              don't sync remote repository" \
        "    -p,--without-patch               don't apply patches" \
        "    -f,--with-flash                  post build copy firmware to DFU drive" \
        "    -l,--with-logging                Enable USB logging" \
        "    --without-emacs                  don't generate emacs settings when --with-comile-db" \
       "" \
        "available targets:"
  for target in ${(k)KEYBOARDS}; do
    print -rC2 -- "   ${target}:"  "${KEYBOARDS[$target]}"
  done
}

error_exit() {
  print -r "Error: $2" >&2
  exit $1
}


# prefix for platform spcific functions
case $HOST_OS in
  macos )
    os=macos
    ;;
  linux )
    if [[ -f /etc/fedora-release ]]; then
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

clean() {
  cd $PROJECT
  rm -rf build
  rm -f compile_commands.json
  rm -f .dir_loccals.el
  rm -f .projectile
  rm -f .clangd
  rm -f .ccls
  rm -rf .cache
  rm -rf .ccls-chache
  rm -rf dist
  if [[ -d $PROJECT/zmk ]]; then
    cd $PROJECT/zmk
    git reset --hard HEAD
    git clean -dfx
  fi
  if [[ -d $PROJECT/zephyr ]]; then
    cd $PROJECT/zephyr
    git reset --hard HEAD
    git clean -dfx
  fi
}

clean_modules() {
  cd $PROJECT
  rm -rf modules
  rm -rf zmk
  rm -rf zephyr
  rm -rf .west
  clean
}

clean_tools() {
  cd $PROJECT
  rm -rf $PROJECT/.venv
  rm -rf ${ZEPHYR_SDK_INSTALL_DIR}/${ZEPHYR_SDK_VERSION}
  if [ ! -z $(docker ps -q -a -f name=$CONTAINER_NAME) ]; then
    docker rm -f  $CONTAINER_NAME $(docker ps -q -a -f name=$CONTAINER_NAME)
    sleep 5
  fi
  if [ ! -z "$(docker images -q $DOCKER_IMAGE)" ]; then
    docker rmi $DOCKER_IMAGE
  fi
}

clean_all() {
  clean_modules
  clean_tools
}

fedora_install_docker() {
  # TODO
  echo "see https://learn.microsoft.com/en-us/windows/wsl/tutorials/wsl-containers"
}

macos_install_docker() {
  brew update
  brew install --cask docker
  brew cleanup
}

setup_docker() {
  if ! which docker &> /dev/null; then
    ${os}_install_docker
    which docker &> /dev/null || \
      error_exit 1 "'docker' command not found. Check Docker.app cli setting."
  fi

  if [ -z "$(docker images -q $DOCKER_IMAGE)" ]; then
    docker build \
           --build-arg HOST_UID=$(id -u) \
           --build-arg HOST_GID=$(id -g) \
           --build-arg WORKSPACE_DIR=$CONTAINER_WORKSPACE_DIR \
           -t my/zmk-dev-arm:stable -f $DOCKERFILE .
  fi
}

docker_exec() {
  local container_id_state=$(docker ps -q -a -f name=$CONTAINER_NAME --format "{{.ID}}:{{.State}}")
  # create container
  if [[ -z $container_id_state ]]; then
    docker run -dit --init \
           -v $PROJECT:$CONTAINER_WORKSPACE_DIR \
           --name $CONTAINER_NAME \
           -w $CONTAINER_WORKSPACE_DIR \
           $DOCKER_IMAGE
  else
    local array_id_state=(${(@s/:/)container_id_state})
    local container_id=$array_id_state[1]
    local container_state=$array_id_state[2]
    if [[ $container_state != "running" ]]; then
      docker start $container_id
    fi
  fi
  # exec
  docker exec $1 \
         -w $CONTAINER_WORKSPACE_DIR \
         $CONTAINER_NAME \
         bash
}

fedora_install_packages() {
  # https://docs.zephyrproject.org/3.2.0/develop/getting_started/index.html#select-and-update-os
  sudo dnf update
  sudo dnf install wget git cmake ninja-build gperf python3 ccache dtc wget xz file \
       make gcc SDL2-devel file-libs \
       tio fd-find ripgrep
  # gcc-multilib g++-multilib
  sudo dnf autoremove
  sudo dnf clean all

  winget=$(/mnt/c/Windows/System32/cmd.exe /C "where winget" 2> /dev/null || true)
  if [[ ! -z $winget ]]; then
    winget=${winget%$'\r'}
    winget=$(wslpath -u $winget)
    $winget install gerardog.gsudo || true
  fi
}

macos_install_packages() {
  # https://docs.zephyrproject.org/3.2.0/develop/getting_started/index.html#select-and-update-os
  # https://docs.zephyrproject.org/3.2.0/contribute/documentation/generation.html
  brew update
  brew install wget git cmake ninja gperf python3 ccache qemu dtc libmagic \
       doxygen graphviz librsvg \
       tio fd rg

  # if PDF is needed
  # brew install mactex
  # eval "$(/usr/libexec/path_helper)"
  # sudo tlmgr update --self --all
  # sudo tlmgr install latexmk
  # sudo tlmgr install collection-fontsrecommended

  brew cleanup
}

pip_install() {
  cd $PROJECT
  if [[ ! -d .venv ]]; then
    python3 -m venv .venv
  fi
  source .venv/bin/activate
  pip3 install west
  pip3 install -r https://raw.githubusercontent.com/zephyrproject-rtos/zephyr/v$ZEPHYR_VERSION/scripts/requirements-base.txt
  pip3 install -r https://raw.githubusercontent.com/zephyrproject-rtos/zephyr/v$ZEPHYR_VERSION/scripts/requirements-build-test.txt
  pip3 install -r https://raw.githubusercontent.com/zephyrproject-rtos/zephyr/v$ZEPHYR_VERSION/scripts/requirements-run-test.txt
  pip3 install -r https://raw.githubusercontent.com/zephyrproject-rtos/zephyr/v$ZEPHYR_VERSION/scripts/requirements-doc.txt
  pip3 install doc2dash
  pip3 cache purge
}

pip_upgrade() {
  pip_install
  cd $PROJECT
  pip3 --disable-pip-version-check list --outdated --format=json | \
    python3 -c "import json, sys; print('\n'.join([x['name'] for x in json.load(sys.stdin)]))" | \
    xargs -n1 pip install -U
  pip3 cache purge
}

setup() {
  cd $PROJECT
  ${os}_install_packages
  if [[ ! -d "${ZEPHYR_SDK_INSTALL_DIR}/zephyr-sdk-${ZEPHYR_SDK_VERSION}" ]]; then
    mkdir -p "$ZEPHYR_SDK_INSTALL_DIR"
    cd "$ZEPHYR_SDK_INSTALL_DIR"
    sdk_minimal_file_name="zephyr-sdk-${ZEPHYR_SDK_VERSION}_${HOST_OS}-${HOST_ARCHITECTURE}_minimal.tar.gz"
    wget "https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v${ZEPHYR_SDK_VERSION}/${sdk_minimal_file_name}"
    tar xvf ${sdk_minimal_file_name}
    rm ${sdk_minimal_file_name}
  fi
  cd "${ZEPHYR_SDK_INSTALL_DIR}/zephyr-sdk-${ZEPHYR_SDK_VERSION}"
  for toolchain in $TARGET_TOOLCHAINS; do
    if [[ ! -d $toolchain ]]; then
      ./setup.sh -h -c -t $toolchain
    fi
  done
  cd $PROJECT

  pip_install
}

update() {
  cd $PROJECT
  if [ ! -d .west/ ]; then
    west init -l zmk_keyboards
  fi

  if $WITH_UPDATE; then
    rm -rf build
    if [ -d zmk ]; then
      # revert changes
      cd zmk
      git reset --hard HEAD
      git clean -dfx
      cd ..
    fi
    west update -n
    if $WITH_PATCH; then
      cd zmk
      git apply -3 --verbose ../patches/zmk_*.patch
      cd ..
    fi
    west zephyr-export
  fi
}

update_with_docker() {
  docker_exec -i <<-EOF
    if [ ! -d .west/ ]; then
        west init -l zmk_keyboards
        west config build.cmake-args -- -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    fi
    if $WITH_UPDATE; then
        rm -rf build
        if [ -d zmk ]; then
            # revert changes
            cd zmk
            git reset --hard HEAD
            git clean -dfx
            cd ..
        fi
        west update -n
        if $WITH_PPATCH; then
            cd zmk
            git apply -3 --verbose ../patches/zmk_*.patch
            cd ..
        fi
        west zephyr-export
    fi
EOF
}

# $1 board
build() {
  local board=$1
  local opts=()
  (( $#with_logging )) && opts=($opts "-DCONFIG_ZMK_USB_LOGGING=y")
  (( $#with_compile_db )) && opts=($opts "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON")
  west build --pristine --board $board --build-dir build/$board zmk/app -- \
       -DZMK_CONFIG=$PROJECT/zmk_keyboards $opts[*]
  if (( $#with_compile_db )); then
    mv $PROJECT/build/$board/compile_commands.json $PROJECT
    dot_clangd
    if $WITH_EMACS; then
      dot_dir_locals
      dot_projectile
    fi
  fi
}

# $1 board
build_with_docker() {
  local board=$1
  local opts=()
  (( $#with_logging )) && opts=($opts "-DCONFIG_ZMK_USB_LOGGING=y")
  docker_exec -i <<-EOF
    west build --pristine --board $board --build-dir build/$board zmk/app -- -DZMK_CONFIG="$CONTAINER_WORKSPACE_DIR/zmk_keyboards" $opts[*]
EOF
}

dot_clangd() {
  cat <<EOS > $PROJECT/.clangd
CompileFlags:
  Remove: [-mfp16-format*, -fno-reorder-functions]
EOS
  rm -rf "${PROJECT}/.cache"
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
-/node_modules
-/via_app
-/qmk_firmware
-/qmk_keyboards
EOS
}

# copy & rename firmware
# $1 board
# $2 firmware name
# return echo path or firmware
# -----------------------------------
dist_firmware() {
  local board=$1
  local firmware_name=$2

  cd $PROJECT
  cd zmk
  local version=$(date +"%Y%m%d")_zmk_$(git rev-parse --short HEAD)
  cd ..
  mkdir -p dist
  src=build/$board/zephyr/zmk.uf2
  variant=""
  (( $#with_logging )) && variant="_logging"
  dst=dist/${firmware_name}_${version}$variant.uf2
  cp $src $dst
  echo $dst
}


macos_uf2_flash() {
  local firmware=$1
  local volume_name=$2

  local dfu_volume=/Volumes/$volume_name
  if [[ -d $dfu_volume  ]]; then
    echo ""
    echo "copying firmware [$firmware] to volume [$dfu_volume]..."
    sleep 1
    cp $firmware $dfu_volume
    true
  else
    false
  fi
}

fedora_uf2_flash() {
  local firmware=$1
  local volume_name=$2

  dfu_drive=$(/mnt/c/Windows/System32/wbem/WMIC.exe logicaldisk get deviceid, volumename | grep $volume_name | awk '{print $1}')
  if [[ ! -z $dfu_drive ]]; then
    echo ""
    echo "copying firmware [$firmware] to drive [$dfu_drive]..."
    sleep 1
    /mnt/c/Program\ Files/gsudo/Current/gsudo.exe  c:\\Windows\\System32\\xcopy.exe "$(wslpath -w $firmware)" $dfu_drive\\
    true
  else
    false
  fi
}

flash_firmware() {
  local firmware=$1
  local volume_name=$2

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
}

macos_log_console() {
  local firmware=$1

  echo -n "waiting for debug output device to be connected..."
  for ((i=0; i < 5; i+=1)); do
    for tty_dev in /dev/tty.usbmodem*; do
      if [[ $tty_dev -nt $firmware ]]; then
        sudo tio $tty_dev
        return
      fi
      echo -n "."
      sleep 1
    done
  done
}

fedora_log_console() {
  local firmware=$1

  #TODO uidbipd-win
}

zephyr_doc2dash() {
  cd $PROJECT
  if [ ! -d zephyr ]; then
    update
  fi
  cd zephyr/doc
  make html
  mkdir -p "$DOCSETS_DIR"
  doc2dash --name "Zephyr" \
           --icon "$PROJECT/zephyr/doc/_static/images/kite.png" \
           --index-page index.html \
           --destination "$DOCSETS_DIR" \
           --enable-js \
           --online-redirect-url "https://docs.zephyrproject.org/$ZEPHYR_VERSION" \
           _build/html
  git clean -dfx .
}

#  sub commands
# -----------------------------------
if (( $#help )); then
  help_usage
  return
elif (( $#clean )); then
  clean
  return
elif (( $#clean_all )); then
  clean_all
  return
elif (( $#clean_tools )); then
  clean_tools
  return
elif (( $#clean_modules )); then
  clean_modules
  return
elif (( $#docker_shell )); then
  setup_docker
  docker_exec -it
  return
elif (( $#setup )); then
  setup
  return
elif (( $#setup_docker )); then
  setup_docker
  return
elif (( $#docker_shell )); then
  setup_docker
  docker_exec -it
  return
fi

if (( $#with_docker )); then
  #  sub commands
  # -----------------------------------
  if (( $#pip_upgrade )); then
    error_exit 1 "Unsupported operation."
  elif (( $#zephyr_doc2dash )); then
    error_exit 1 "Unsupported operation."
  fi
else
  if [[ ! -d .venv ]]; then
    error_exit 1 "Not found .venv, execute the --setup command."
  fi
  if [[ $(which west) != $PROJECT/.venv/bin/west ]]; then
    error_exit 1 "Not found west command, execute the --setup command"
  fi

  #  activate virtual env
  # -----------------------------------
  if [[ $(which python3) != $PROJECT/.venv/bin/python3 ]]; then
    source .venv/bin/activate
  fi

  #  sub commands
  # -----------------------------------
  if (( $#pip_upgrade )); then
    pip_upgrade
    return
  elif (( $#zephyr_doc2dash )); then
    zephyr_doc2dash
    return
  fi
fi

# build option parameters
# -----------------------------------
(( $#without_update )) && WITH_UPDATE=false
(( $#without_patch )) && WITH_PATCH=false
(( $#without_emacs )) && WITH_EMACS=false
(( $#@ )) && TARGETS=("$@")

[[ -d modules ]] || WITH_UPDATE=true
[[ -d zephyr ]] || WITH_UPDATE=true
[[ -d zmk ]] || WITH_UPDATE=true
[[ -d .west ]] || WITH_UPDATE=true

#  clean build
# -----------------------------------
if $WITH_UPDATE || (( $#with_clean )); then
  clean
fi

cd $PROJECT

# build
# -----------------------------------
if (( $#with_docker )); then
  update_with_docker
else
  update
fi
for target in $TARGETS; do
  kbd=(${(@s/:/)KEYBOARDS[$target]})
  board=$kbd[1]
  firmware_name=$kbd[2]
  dfu_volume_name=$kbd[3]
  if (( $#with_docker )); then
    build_with_docker $board
  else
    build $board
  fi
  firmware_file=$(dist_firmware $board $firmware_name)
  if (( $#with_flash )); then
    (( $#with_logging )) && sudo echo -n
    flash_firmware $firmware_file $dfu_volume_name
    (( $#with_logging )) && ${os}_log_console $firmware_file
  fi
done
