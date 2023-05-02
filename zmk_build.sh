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
UPDATE_BUILD=true
APPLY_PATCHES=true
CONTAINER_WORKSPACE_DIR=/workspace

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
           {s,-docker-shell}=docker_shell \
           {d,-with-docker}=with_docker \
           -with-setup=with_setup \
           {c,-with-clean}=with_clean \
           -with-compile-db=with_compile_db \
           {w,-without-update}=without_update \
           {p,-without-patch}=without_patch \
           {f,-with-flash}=with_flash \
  || return


# functions
# -----------------------------------
help_usage() {
  print -rC1 -- \
        "" \
        "Usage:" \
        "    $THIS_SCRIPT:t <-h|--help>                     help" \
        "    $THIS_SCRIPT:t --clean                         clean build folder" \
        "    $THIS_SCRIPT:t --clean-modules                 clean source moudules & build files" \
        "    $THIS_SCRIPT:t --clean-tools                   clean zephyr sdk & project build tools" \
        "    $THIS_SCRIPT:t --clean-all                     clean build environment" \
        "    $THIS_SCRIPT:t --setup                         setup zephyr sdk & projtect build tools" \
        "    $THIS_SCRIPT:t --setup-docker                  create docker image" \
        "    $THIS_SCRIPT:t --pip-upgrade                   upgrade python packages" \
        "    $THIS_SCRIPT:t <-s|--docker-shell>             enter docker container shell" \
        "    $THIS_SCRIPT:t [build options...] [TARGETS..]  build firmwares" \
        "" \
        "build options:" \
        "    -c,--with-clean                  pre build clean up build & temporary files" \
        "    -d,--with-docker                 build with docker" \
        "    --with-compile-db             copy compile_command.json" \
        "    --with-setup                     pre build automatic setup" \
        "    -w,--without-update              don't sync remote repository" \
        "    -p,--without-patch               don't apply patches" \
        "    -f,--with-flash                  post build copy firmware to DFU drive" \
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
  find . -name "*~" -exec rm -f {} \;
  find . -name ".DS_Store" -exec rm -f {} \;
}

clean_modules() {
  cd $PROJECT
  clean()
  rm -rf dist
  rm -rf build
  rm -rf modules
  rm -rf zmk
  rm -rf zephyr
  rm -rf .west
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
  cd $PROJECT
  clean_modules
  clean_tools
}

fedora_setup_docker() {
  # TODO
  echo "see https://learn.microsoft.com/en-us/windows/wsl/tutorials/wsl-containers"
}

macos_setup_docker() {
  brew update
  brew install --cask docker
  brew cleanup
}

setup_docker() {
  if ! which docker &> /dev/null; then
    ${os}_setup_docker
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

  container_id_state=$(docker ps -q -a -f name=$CONTAINER_NAME --format "{{.ID}}:{{.State}}")
  # create container
  if [[ -z $container_id_state ]]; then
    docker run -dit --init \
           -v $PROJECT:$CONTAINER_WORKSPACE_DIR \
           --name $CONTAINER_NAME \
           -w $CONTAINER_WORKSPACE_DIR \
           $DOCKER_IMAGE
  else
    array_id_state=(${(@s/:/)container_id_state})
    container_id=$array_id_state[1]
    container_state=$array_id_state[2]
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

fedora_setup() {
  # https://docs.zephyrproject.org/3.2.0/develop/getting_started/index.html#select-and-update-os
  sudo dnf update
  sudo dnf install wget git cmake ninja-build gperf python3 ccache dtc wget xz file \
       make gcc SDL2-devel file-libs
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

macos_setup() {
  # https://docs.zephyrproject.org/3.2.0/develop/getting_started/index.html#select-and-update-os
  brew update
  brew install wget git cmake ninja gperf python3 ccache qemu dtc libmagic ccls
  brew cleanup
}

setup() {
  cd $PROJECT
  ${os}_setup
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

  # zinit setting
  #
  # https://github.com/MichaelAquilina/zsh-autoswitch-virtualenv
  # export AUTOSWITCH_DEFAULT_PYTHON=python3
  # zinit load MichaelAquilina/zsh-autoswitch-virtualenv

  if [[ ! -d .venv ]]; then
    python3 -m venv .venv
  fi
  source .venv/bin/activate
  pip3 install west
  pip3 install -r https://raw.githubusercontent.com/zephyrproject-rtos/zephyr/v${ZEPHYR_VERSION}/scripts/requirements-base.txt
  pip3 install -r https://raw.githubusercontent.com/zephyrproject-rtos/zephyr/v${ZEPHYR_VERSION}/scripts/requirements-build-test.txt
  pip3 install -r https://raw.githubusercontent.com/zephyrproject-rtos/zephyr/v${ZEPHYR_VERSION}/scripts/requirements-run-test.txt
  pip3 cache purge
}

pip_upgrade() {
  cd $PROJECT
  source .venv/bin/activate
  pip3 --disable-pip-version-check list --outdated --format=json | \
    python3 -c "import json, sys; print('\n'.join([x['name'] for x in json.load(sys.stdin)]))" | \
    xargs -n1 pip install -U
}



update() {
  cd $PROJECT
  if [ ! -d .west/ ]; then
    west init -l zmk_keyboards
    west config build.cmake-args -- -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
  fi

  if $UPDATE_BUILD; then
    rm -rf build
    if [ -d zmk ]; then
      # revert changes
      cd zmk
      git reset --hard HEAD
      git clean -dfx
      cd ..
    fi
    west update -n
    if $APPLY_PATCHES; then
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
    if $UPDATE_BUILD; then
        rm -rf build
        if [ -d zmk ]; then
            # revert changes
            cd zmk
            git reset --hard HEAD
            git clean -dfx
            cd ..
        fi
        west update -n
        if $APPLY_PATCHES; then
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
  board=$1

  west build --pristine --board $board --build-dir build/$board zmk/app -- -DZMK_CONFIG=$PROJECT/zmk_keyboards
}

# $1 board
build_with_docker() {
  board=$1

  docker_exec -i <<-EOF
    west build --pristine --board $board --build-dir build/$board zmk/app -- -DZMK_CONFIG="$CONTAINER_WORKSPACE_DIR/zmk_keyboards"
EOF
}

clangd_setting() {
  cat <<EOS > "${PROJECT}/.clangd"
CompileFlags:
  Remove: [-mfp16-format*, -fno-reorder-functions]
EOS
  rm -rf "${PROJECT}/.cache"
}

ccls_setting() {
  cat <<EOS > "${PROJECT}/.ccls"
{
  "clang": {
     "resourceDir": "${ZEPHYR_SDK_INSTALL_DIR}/zephyr-sdk-${ZEPHYR_SDK_VERSION}/${TARGET_TOOLCHAIN}/${TARGET_TOOLCHAIN}",
     "clang.extraArgs": [
       "-gcc-toolchain=${ZEPHYR_SDK_INSTALL_DIR}/zephyr-sdk-${ZEPHYR_SDK_VERSION}/${TARGET_TOOLCHAIN}"
     ]
  },
  "compilationDatabaseDirectory": "${PROJECT}"
}
EOS
}

# copy & rename firmware
# $1 board
# $2 firmware name
# return echo path or firmware
# -----------------------------------
dist_firmware() {
  board=$1
  firmware_name=$2

  cd $PROJECT
  cd zmk
  version=$(date +"%Y%m%d")_zmk_$(git rev-parse --short HEAD)
  cd ..
  mkdir -p dist
  src=build/$board/zephyr/zmk.uf2
  dst=dist/${firmware_name}_$version.uf2
  cp $src $dst
  echo $dst
}


macos_uf2_flash() {
  firmware=$1
  volume_name=$2

  dfu_volume=/Volumes/$volume_name
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
  firmware=$1
  volume_name=$2

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
  firmware=$1
  volume_name=$2

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

cd $PROJECT


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
elif (( $#pip_upgrade )); then
  pip_upgrade
  return
fi

# build option parameters
# -----------------------------------
(( $#without_update )) && UPDATE_BUILD=false
(( $#without_patch )) && APPLY_PATCHES=false
(( $#@ )) && TARGETS=("$@")

[[ -d modules ]] || UPDATE_BUILD=true
[[ -d zephyr ]] || UPDATE_BUILD=true
[[ -d zmk ]] || UPDATE_BUILD=true
[[ -d .west ]] || UPDATE_BUILD=true

#  clean build
# -----------------------------------
if $UPDATE_BUILD || (( $#with_clean )); then
  clean
fi

# pre build setup
# -----------------------------------
if (( $#with_setup )); then
  if (( $#with_docker )); then
    setup_docker
  else
    setup
  fi
fi


# build
# -----------------------------------
if (( $#with_docker )); then
  update_with_docker
else
  if [[ $(which python3) != $PROJECT/.venv/bin/python3 ]]; then
    source .venv/bin/activate
  fi
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
    if (( $#with_compile_db )); then
      cp $PROJECT/build/$board/compile_commands.json $PROJECT
      clangd_setting
    fi
  fi
  firmware_file=$(dist_firmware $board $firmware_name)
  if (( $#with_flash )); then
    flash_firmware $firmware_file $dfu_volume_name
  fi
done
