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
ZEPHYR_VERSION=3.5.0
ZEPHYR_SDK_VERSION=0.16.8

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
WIN_USBIPD="/mnt/c/Program Files/usbipd-win/usbipd.exe"
WIN_GSUDO="/mnt/c/Program Files/gsudo/Current/gsudo.exe"
# DOCSETS_DIR="$HOME/Library/Application Support/Dash/DockSets"

# key: target
#   [1]=board
#   [2]=firmwre_name
#   [3]=firmware file type
#   [4]=DFU volume name or device vid
#   [5]=DFU device pid
#   [6]=shields
local -A KEYBOARDS=(
  cz42l      nice_nano_v2:cz42_left:uf2:NICENANO:none:"cz42_left;gooddisplay_adapter;gooddisplay_gdew0102t4"
  cz42r      nice_nano_v2:cz42_right:uf2:NICENANO:none:"cz42_right;gooddisplay_adapter;gooddisplay_gdew0102t4"
  d60        bt60:d60_lite_hhkb_ec11:uf2:CKP:none:none
  fk68       fk680pro_v2:fk680pro_v2:uf2:"ZhaQian DFU":none:none
  q60        keychron_q60:keychron_q60:bin:0483:df11:none
  qk60       qk60_wired:qk60_wired_hhkb:bin:1688:2220:none
  tf60       kbdfans_tofu60_v2:tofu60_hhkb:uf2:RPI-RP2:none:none
  ju60       cyber60_rev_d:hibi_june60:uf2:CYBER60_D:none:none
)
TARGETS=(cz42l cz42r d60 fk68 q60 qk60 tf60 ju60)


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
           -docker-shell=docker_shell \
           {d,-with-docker}=with_docker \
           {c,-with-clean}=with_clean \
           {g,-with-compile-db}=with_compile_db \
           {w,-without-update}=without_update \
           {n,-without-patch}=without_patch \
           {f,-with-flash}=with_flash \
           {p,-with-pp}=with_pp \
           {l,-with-logging}=with_logging \
           {s,-with-shell}=with_shell \
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
        "    $THIS_SCRIPT:t --docker-shell                  enter docker container shell" \
        "    $THIS_SCRIPT:t [build options...] [TARGETS..]  build firmwares" \
        "" \
        "build options:" \
        "    -c,--with-clean       clean up generated files" \
        "    -d,--with-docker      build with docker" \
        "    -g,--with-compile-db  generate compile_command.json" \
        "    -w,--without-update   don't sync remote repository" \
        "    -n,--without-patch    don't apply patches" \
        "    -f,--with-flash       post build copy firmware to DFU drive" \
        "    -p,--with-pp          Save preprocessor output" \
        "    -l,--with-logging     Enable USB logging" \
        "    -s,--with-shell       Enable Shell" \
        "    --without-emacs       don't generate emacs settings when --with-comile-db" \
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
  local container_id_state=$(docker ps -a -f name=$CONTAINER_NAME --format "{{.ID}}:{{.State}}")
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
  # https://docs.zephyrproject.org/3.5.0/develop/getting_started/index.html#select-and-update-os
  sudo dnf update
  sudo dnf install wget git cmake gperf python3 dtc wget xz file \
       make gcc SDL2-devel file-libs \
       tio fd-find ripgrep fzf
  # gcc-multilib g++-multilib
  sudo dnf autoremove
  sudo dnf clean all

  winget=$(/mnt/c/Windows/System32/cmd.exe /C "where winget" 2> /dev/null || true)
  if [[ ! -z $winget ]]; then
    winget=${winget%$'\r'}
    winget=$(wslpath -u $winget)
    $winget install dorssel.usbipd-win || true
    $winget install gerardog.gsudo || true
  fi
}

macos_install_packages() {
  # https://docs.zephyrproject.org/3.5.0/develop/getting_started/index.html#select-and-update-os
  brew update
  brew install wget git cmake gperf python3 qemu dtc libmagic \
       doxygen graphviz librsvg \
       tio fd rg fzf

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
  pip3 install -r https://raw.githubusercontent.com/zephyrproject-rtos/zephyr/v$ZEPHYR_VERSION/scripts/requirements.txt
  pip3 install -r https://raw.githubusercontent.com/zephyrproject-rtos/zephyr/v$ZEPHYR_VERSION/doc/requirements.txt
  pip3 install ninja
  pip3 install pip-review
  pip3 install doc2dash
  pip3 cache purge
}

pip_upgrade() {
  pip_install
  pip-review -a
  pip3 cache purge
}

setup() {
  cd $PROJECT
  ${os}_install_packages

  if [[ ! -d "${ZEPHYR_SDK_INSTALL_DIR}/zephyr-sdk-${ZEPHYR_SDK_VERSION}" ]]; then
    mkdir -p "$ZEPHYR_SDK_INSTALL_DIR"
    cd "$ZEPHYR_SDK_INSTALL_DIR"
    sdk_minimal_file_name="zephyr-sdk-${ZEPHYR_SDK_VERSION}_${HOST_OS}-${HOST_ARCHITECTURE}_minimal.tar"
    if [[ ZEPHYR_SDK_VERSION > "0.15.2" ]] ;then
      sdk_minimal_file_name=${sdk_minimal_file_name}.xz
    else
      sdk_minimal_file_name=${sdk_minimal_file_name}.gz
    fi
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

  # remove old SDK
  cd "$ZEPHYR_SDK_INSTALL_DIR"
  for sdk in zephyr-sdk-*; do
    if [[ $sdk != zephyr-sdk-${ZEPHYR_SDK_VERSION} ]]; then
      rm -rf $sdk
    fi
  done

  # install python packages
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
    if [ -d zephyr ]; then
      # revert changes
      cd zephyr
      git reset --hard HEAD
      git clean -dfx
      cd ..
    fi
    if [ -d zmk ]; then
      cd zmk
      git reset --hard HEAD
      git clean -dfx
      cd ..
    fi
    west update -n
    if $WITH_PATCH; then
      cd zephyr
      git apply -3 --verbose ../patches/zephyr_*.patch
      cd ..
      cd zmk
      # temporary patch
      if [[ -f ../patches/_zmk_master.patch ]]; then
        git apply -3 --verbose ../patches/_zmk_master.patch
      fi
      git apply -3 --verbose ../patches/zmk_*.patch
      cd ..
    fi

    # temporary remove
    # build failed with warning: LV_Z_DPI defined without a type
    # rm -rf zmk/app/boards/shields/leeloo_micro
    # rm -rf zmk/app/boards/shields/nice_view

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
        if [ -d zephyr ]; then
            # revert changes
            cd zephyr
            git reset --hard HEAD
            git clean -dfx
            cd ..
        fi
        if [ -d zmk ]; then
            cd zmk
            git reset --hard HEAD
            git clean -dfx
            cd ..
        fi
        west update -n
        if $WITH_PATCH; then
            cd zephyr
            git apply -3 --verbose ../patches/zephyr_*.patch
            cd ..
            cd zmk
            if [[ -f ../patches/_zmk_master.patch ]]; then
               git apply -3 --verbose ../patches/_zmk_master.patch
            fi
            git apply -3 --verbose ../patches/zmk_*.patch
            cd ..
        fi
        west zephyr-export
    fi
EOF
}

# $1 board
build() {
  local target=$1
  local kbd=(${(@s/:/)KEYBOARDS[$target]})
  local board=$kbd[1]
  local shields=$kbd[6]
  local opts=()
  local pristine="auto"

  $WITH_UPDATE && pristine="always"

  (( $#with_logging )) && opts=($opts "-DCONFIG_ZMK_USB_LOGGING=y" "-DCONFIG_LOG_THREAD_ID_PREFIX=y")
  (( $#with_shell )) && opts=($opts "-DCONFIG_SHELL=y")
  #  temporarily fix dependencie issue for nrf boards
  (( $#with_shell )) && [[ $board = "bt60" || $board == "cyber60_rev_d" ]] && \
    opts=($opts -DCONFIG_CBPRINTF_COMPLETE=y -DCONFIG_SHELL_BACKEND_SERIAL=y)
  (( $#with_compile_db )) && opts=($opts "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON")
  (( $#with_pp )) && opts=($opts "-DEXTRA_CFLAGS=-save-temps=obj")
  [[ $shields != none ]] && opts+=("-DSHIELD=$shields")

  if (( $#with_compile_db )); then
    dot_clangd
    if $WITH_EMACS; then
      dot_dir_locals $target
      dot_projectile
    fi
  fi
  west build --pristine=$pristine --board $board --build-dir build/$target zmk/app -- \
       -DZMK_CONFIG=$PROJECT/zmk_keyboards $opts[*]

  if (( $#with_compile_db )); then
    mv $PROJECT/build/$target/compile_commands.json $PROJECT
  fi
}

# $1 board
build_with_docker() {
  local target=$1
  local kbd=(${(@s/:/)KEYBOARDS[$target]})
  local board=$kbd[1]
  local shields=$kbd[6]
  local opts=()
  local pristine="auto"
  $WITH_UPDATE && pristine="always"

  (( $#with_logging )) && opts=($opts "-DCONFIG_ZMK_USB_LOGGING=y")
  (( $#with_shell )) && opts=($opts "-DCONFIG_SHELL=y")
  (( $#with_pp )) && opts=($opts "-DEXTRA_CFLAGS=-save-temps=obj")
  [[ $shields != none ]] && opts+=("-DSHIELD=$shields")
  docker_exec -i <<-EOF
    west build --pristine=$pristine --board $board --build-dir build/$target zmk/app -- -DZMK_CONFIG="$CONTAINER_WORKSPACE_DIR/zmk_keyboards" $opts[*]
EOF
}

dot_clangd() {
  cat <<EOS > $PROJECT/.clangd
CompileFlags:
  Remove: [-mfp16-format*, -fno-reorder-functions, -fno-printf-return-value]
EOS
  rm -rf "${PROJECT}/.cache"
}

dot_dir_locals() {
  local target=$1
  cat <<EOS > $PROJECT/.dir-locals.el
((nil . ((projectile-git-use-fd . t)
         (projectile-git-fd-args . "--hidden --no-ignore -0 --exclude '\.*' --type f --strip-cwd-prefix")
         (counsel-rg-base-command . ("rg" "--no-ignore" "--max-columns" "240" "--with-filename" "--no-heading" "--line-number" "--color" "never" "%s"))
         (compile-command . "zmk_build.sh -wp $target")
)))
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
# $3 firmware typee
# return echo path or firmware
# -----------------------------------
dist_firmware() {
  local target=$1
  local kbd=(${(@s/:/)KEYBOARDS[$target]})
  local board=$kbd[1]
  local firmware_name=$kbd[2]
  local firmware_ext=$kbd[3]

  cd $PROJECT
  cd zmk
  local version=$(date +"%Y%m%d")_zmk_$(git rev-parse --short HEAD)
  cd ..
  mkdir -p dist
  local src=build/$target/zephyr/zmk.$firmware_ext
  local variant=""
  (( $#with_logging )) && variant="_logging"
  (( $#with_shell )) && variant="_shell"
  local dst=dist/${firmware_name}_${version}$variant.$firmware_ext
  cp $src $dst
  echo $dst
}



flash() {
  local target=$1
  local firmware=$2
  local kbd=(${(@s/:/)KEYBOARDS[$target]})
  local firmware_ext=$kbd[3]
  ${os}_flash_${firmware_ext} $target $firmware
}

macos_flash_uf2() {
  local target=$1
  local firmware=$2
  local kbd=(${(@s/:/)KEYBOARDS[$target]})
  local board=$kbd[1]
  local volume_name=$kbd[4]
  local dfu_volume=/Volumes/$volume_name

  echo -n "waiting for DFU volume [$dfu_volume] to be mounted..."
  while true; do
    if [[ -d $dfu_volume ]]; then
      echo ""
      echo "copying firmware [$firmware] to volume [$dfu_volume]..."
      sleep 1
      cp -X $firmware $dfu_volume || true
      # west flash --build-dir build/$target || true
      echo "flashing firmware finished successfully."
      return
    else
      echo -n "."
      sleep 1
    fi
  done
}

fedora_flash_uf2() {
  local target=$1
  local firmware=$2
  local kbd=(${(@s/:/)KEYBOARDS[$target]})
  local board=$kbd[1]
  local volume_name=$kbd[4]

  echo -n "waiting for DFU volume to be mounted..."
  while true; do
    dfu_drive=$(/mnt/c/Windows/System32/wbem/WMIC.exe logicaldisk get deviceid, volumename | grep $volume_name | awk '{print $1}')
    if [[ ! -z $dfu_drive ]]; then
      echo ""
      echo "copying firmware [$firmware] to drive [$dfu_drive]..."
      sleep 1
      $WIN_GSUDO c:\\Windows\\System32\\xcopy.exe "$(wslpath -w $firmware)" $dfu_drive\\
      echo "flashing firmware finished successfully."
      break
    else
      echo -n "."
      sleep 1
    fi
  done
}

macos_flash_bin() {
  local target=$1
  west flash --build-dir build/$target || true
}

fedora_flash_bin() {
  local target=$1
  local kbd=(${(@s/:/)KEYBOARDS[$target]})
  local hardware_id="$kbd[4]:$kbd[5]"

  sudo echo -n
  echo -n "waiting for target DFU device to be connected.."
  while true; do
    dfu_device=$($WIN_USBIPD list 2> /dev/null | grep "$hardware_id" || echo -n "")
    if [[ ! -z $dfu_device ]]; then
      if [[ $dfu_device =~ "Not shared" ]]; then
        $WIN_GSUDO "$(wslpath -w $WIN_USBIPD)" bind --hardware-id $hardware_id
      elif [[ $dfu_device =~ "Shared" ]]; then
        $WIN_GSUDO "$(wslpath -w $WIN_USBIPD)" attach --wsl --hardware-id $hardware_id
      elif [[ $dfu_device =~ "Attached" ]]; then
        sudo chmod -R 777 /dev/bus/usb
        west flash --build-dir build/$target || true
        return
      fi
    fi
    sleep 1
    echo -n "."
  done
}

macos_log_console() {
  local firmware=$1
  local log_file=logs/${firmware:t:r}.txt
  cd $PROJECT
  mkdir -p logs
  echo -n "waiting for debug output device to be connected.."
  while true; do
    echo -n "."
    sleep 1
    for tty_dev in /dev/tty.usbmodem*(N); do
      if [[ $tty_dev -nt $firmware ]]; then
        echo "found tty device [$tty_dev]"
        # to exit tio, [Ctrl + t][q]
        sudo chmod +urw $tty_dev
        tio --log --log-file=$log_file $tty_dev
        return
      fi
    done
  done
}

fedora_log_console() {
  local firmware=$1

  #TODO uidbipd-win
}

macos_shell_console() {
  local firmware=$1

  echo -n "waiting for shell device to be connected.."
  while true; do
    echo -n "."
    sleep 1
    for cu_dev in /dev/cu.usbmodem*(N); do
      if [[ $cu_dev -nt $firmware ]]; then
        echo "found cu device [$cu_dev]"
        sleep 3
        # to exit cu, [~][.][enter]
        sudo cu -l $cu_dev
        return
      fi
    done
  done
}

fedora_shell_console() {
  local firmware=$1

  #TODO uidbipd-win
}

zephyr_doc2dash() {
  cd $PROJECT
  if [ ! -d zephyr ]; then
    update
  fi

  # cleanup
  cd $PROJECT/zephyr
  git reset --hard
  git clean -dfx

  # see https://docs.zephyrproject.org/3.5.0/contribute/documentation/generation.html
  cd $PROJECT/zephyr/doc
  cmake -GNinja -B_build .
  cd _build
  # at first time failed on too many errors
  ninja -v html || ninja -v html

  mkdir -p $DOCSETS_DIR
  doc2dash --name Zephyr \
           --icon $PROJECT/zephyr/doc/_static/images/kite.png \
           --index-page index.html \
           --force \
           --destination $DOCSETS_DIR \
           --enable-js \
           --online-redirect-url https://docs.zephyrproject.org/$ZEPHYR_VERSION \
           html
  # hide left sidebar
  cat <<EOF >> $DOCSETS_DIR/Zephyr.docset/Contents/Resources/Documents/_static/css/theme.css
.wy-nav-side{display:none}
.wy-nav-content-wrap{margin-left:unset}
EOF

  # cleanup
  cd $PROJECT/zephyr
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

  #  activate virtual env
  # -----------------------------------
  if [[ $(which python3) != $PROJECT/.venv/bin/python3 ]]; then
    source .venv/bin/activate
  fi

  if [[ $(which west) != $PROJECT/.venv/bin/west ]]; then
    error_exit 1 "Not found west command, execute the --setup command"
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
  firmware_ext=$kbd[3]
  shields=$kbd[6]

  if (( $#with_docker )); then
    build_with_docker $target
  else
    build $target
  fi
  firmware_file=$(dist_firmware $target)
  if (( $#with_flash )); then
    (( $#with_logging )) && sudo echo -n
    (( $#with_shell )) && sudo echo -n
    flash $target $firmware_file
    if (( $#with_logging )); then
      ${os}_log_console $firmware_file
    elif (( $#with_shell )); then
      ${os}_shell_console $firmware_file
    fi
  fi
done
