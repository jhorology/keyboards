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
ZEPHYR_SDK_VERSION=0.16.9

# failed to build on error with 17.0
# ..../.local/zephyr-sdk-0.17.0/arm-zephyr-eabi/picolibc/include/sys/_stdint.h:14:13: error: expected ';' before 'typedef'
# 14 | _BEGIN_STD_C
#         |             ^
#         |             ;
#         ......
#         18 | typedef __int8_t int8_t ;
#         | ~~~~~~~
#         c
# ZEPHYR_SDK_VERSION=0.17.0

# it is recommended to extract the Zephyr SDK bundle at one of the following default locations:
#
# $HOME
# $HOME/.local
# $HOME/.local/opt
# $HOME/bin
# /opt
# /usr/local
ZEPHYR_SDK_INSTALL_DIR=$PROJECT/.local

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
WITH_UPDATE=true
WITH_PATCH=true
WITH_EMACS=true
DOCSETS_DIR=$HOME/.docsets
WIN_USBIPD="/mnt/c/Program Files/usbipd-win/usbipd.exe"
WIN_SUDO="/mnt/c/Windows/System32/sudo.exe"
WIN_WMIC="/mnt/c/Windows/System32/wbem/WMIC.exe"
WIN_TERMINAL="Microsoft/WindowsApps/wt.exe"
WIN_SIMPLE_COM="/mnt/c/Program Files/YaSuenag/SimpleCom/SimpleCom.exe"
WIN_HARDWARE_ID0="VID_05AC&PID_024F"
WIN_HARDWARE_ID1="VID_1D50&PID_615E"

# DOCSETS_DIR="$HOME/Library/Application Support/Dash/DockSets"
ZMK_STUDIO_BRANCH=main
PROTOC_INSTALL_DIR=$PROJECT/.local
PROTOC_VERSION=28.2

_error_exit() {
  print -Pr "%F{red}Error: $2" >&2
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
      _error_exit 1 'unsupported platform.'
    fi
    ;;
  * )
    _error_exit 1 'unsupported platform.'
    ;;
esac


# keyboard deinitions
#  properties:
#    name           required  firmware file name
#    ble            required  is ble wireless keyboard?
#    studio         required  can enable --with-studio opton?
#    board          required
#    shields        optional  if defined multiple shields, double quate semicolon separated list
#    firmware_type  required  uf2 or bin
#    dfu_volume     required  if firmware_type is uf2
#    dfu_vid        required  if firmware_type is bin
#    dfu_pid        required  if firmware_type is bin
#    log_opts       optional  config options when --with-logging, if defined multiple options, double quate comma separated list
declare -A cz42l=(
  [name]=cz42_left
  [ble]=true
  [studio]=true
  [board]=nice_nano_v2
  [shields]="cz42_left;gooddisplay_adapter;gooddisplay_gdew0102t4"
  [firmware_type]=uf2
  [dfu_volume]=CZ42_L
)

declare -A cz42r=(
  [name]=cz42_left
  [ble]=true
  [studio]=false
  [board]=nice_nano_v2
  [shields]="cz42_right;gooddisplay_adapter;gooddisplay_gdew0102t4"
  [firmware_type]=uf2
  [dfu_volume]=CZ42_R
)

declare -A d60=(
  [name]=d60_lite_hhkb_ec11
  [ble]=true
  [studio]=true
  [board]=bt60
  [firmware_type]=uf2
  [dfu_volume]=CKP
)

# *not enough RAM to enable studio
declare -A fk68=(
  [name]=fk680pro_v2
  [ble]=false
  [studio]=false
  [board]=fk680pro_v2
  [firmware_type]=uf2
  [dfu_volume]="ZhaQian DFU"
)

declare -A libra=(
  [name]=libra_mini
  [ble]=true
  [studio]=true
  [board]=nice_nano_v2
  [shields]=libra_mini
  [firmware_type]=uf2
  [dfu_volume]=LIBRA_MINI
  [log_opts]=CONFIG_INPUT_LOG_LEVEL_DBG
)

declare -A rz42l=(
  [name]=rz42_left
  [ble]=true
  [studio]=true
  [board]=rz42_left
  [firmware_type]=uf2
  [dfu_volume]=RZ42_L
  [log_opts]="CONFIG_INPUT_LOG_LEVEL_DBG,CONFIG_DISPLAY_LOG_LEVEL_DBG"
)

declare -A rz42r=(
  [name]=rz42_right
  [ble]=true
  [studio]=false
  [board]=rz42_right
  [firmware_type]=uf2
  [dfu_volume]=RZ42_R
  [log_opts]="CONFIG_INPUT_LOG_LEVEL_DBG,CONFIG_DISPLAY_LOG_LEVEL_DBG"
)

declare -A q60=(
  [name]=keychron_q60
  [ble]=false
  [studio]=true
  [board]=keychron_q60
  [firmware_type]=bin
  [dfu_vid]=0483
  [dfu_pid]=df11
)

# *Depricated
# Currently it does not work for unknown reasons
# it may work with ZMK before the Studio related code was merged. (around 2024/10 ?)
declare -A qk60=(
  [name]=qk60_wired_hhkb
  [ble]=false
  [studio]=false
  [board]=qk60_wired
  [firmware_type]=bin
  [dfu_vid]=1688
  [dfu_pid]=2220
)

declare -A tf60=(
  [name]=tofu60_v2_hhkb
  [ble]=false
  [studio]=true
  [board]=kbdfans_tofu60_v2
  [firmware_type]=uf2
  [dfu_volume]=RPI-RP2
)

declare -A ju60=(
  [name]=hibi_june60_tsangan
  [ble]=true
  [studio]=true
  [board]=cyber60_rev_d
  [firmware_type]=uf2
  [dfu_volume]=CYBER60_D
)

ALL_TARGETS=(cz42l cz42r d60 fk68 libra rz42l rz42r q60 qk60 tf60 ju60)
TARGETS=(cz42l cz42r d60 fk68 libra rz42l rz42r q60 tf60 ju60)


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
           -pip-upgrade=pip_upgrade \
           -zephyr-doc2dash=zephyr_doc2dash \
           -studio-app=studio_app \
           -studio-web=studio_web \
           {c,-with-clean}=with_clean \
           {g,-with-compile-db}=with_compile_db \
           {w,-without-update}=without_update \
           {n,-without-patch}=without_patch \
           {f,-with-flash}=with_flash \
           {p,-with-pp}=with_pp \
           {l,-with-logging}=with_logging \
           {s,-with-shell}=with_shell \
           {z,-with-studio}=with_studio \
           {x,-without-studio}=without_studio \
           {r,-with-ram-report}=with_ram_report \
           -without-emacs=without_emacs \
  || return

(( $#@ )) && TARGETS=("$@")

# subcommand --help
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
        "    $THIS_SCRIPT:t --pip-upgrade                   upgrade python packages" \
        "    $THIS_SCRIPT:t --zephyr-doc2dash               generate zephyr docsets" \
        "    $THIS_SCRIPT:t --studio-web                    launch ZMK Studio web" \
        "    $THIS_SCRIPT:t --studio-app                    launch ZMK Studio app" \
        "    $THIS_SCRIPT:t [build options...] [TARGETS..]  build firmwares" \
        "" \
        "build options:" \
        "    -c,--with-clean       clean up generated files" \
        "    -g,--with-compile-db  generate compile_command.json" \
        "    -w,--without-update   don't sync remote repository" \
        "    -n,--without-patch    don't apply patches" \
        "    -f,--with-flash       post build copy firmware to DFU drive" \
        "    -p,--with-pp          Save preprocessor output" \
        "    -l,--with-logging     Enable USB logging" \
        "    -s,--with-shell       Enable Shell" \
        "    -z,--with-studio      Enable ZMK Studio" \
        "    -x,--without-studio   Disable ZMK Studio" \
        "    -r,--with-ram-report  Enable ram_report" \
        "    --without-emacs       don't generate emacs settings when --with-comile-db" \
       "" \
        "available targets:"
  for target in $ALL_TARGETS; do
    local -A props=("${(Pkv@)target}")
    print -rC2 -- "   ${target}=("
    print -rC2 -- "     [name]=$props[name]"
    print -rC2 -- "     [ble]=$props[ble]"
    print -rC2 -- "     [studio]=$props[studio]"
    print -rC2 -- "     [board]=$props[board]"
    if (( ${+props[shields]} )); then
      print -rC2 -- "     [shields]=$props[shields]"
    fi
    print -rC2 -- "     [firmware_type=$props[firmware_type]"
    case $props[firmware_type] in
      uf2)
        print -rC2 -- "     [dfu_volume]=$props[dfu_volume]"
        ;;
      bin)
        print -rC2 -- "     [dfu_vid]=$props[dfu_vid]"
        print -rC2 -- "     [dfu_pid]=$props[dfu_pid]"
        ;;
    esac
    if (( ${+props[log_opts]} )); then
      print -rC2 -- "     [log_opts]=$props[log_opts]"
    fi
    print -rC2 -- "   )"
  done
}

main() {
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
  elif (( $#setup )); then
    setup
    return
  fi

  _activate_direnv

  #  sub commands
  # -----------------------------------
  if (( $#pip_upgrade )); then
    pip_upgrade
    return
  elif (( $#zephyr_doc2dash )); then
    zephyr_doc2dash
    return
  elif (( $#studio_app )); then
    studio_app
    return
  elif (( $#studio_web )); then
    studio_web
    return
  fi

  local unknown_targets=()
  for target in $TARGETS; do
    if ! (($ALL_TARGETS[(Ie)$target])); then
      unknown_targets+=($target)
    fi
  done
  if (( $#unknown_targets )); then
    _error_exit 1 "Unknown target(s): $unknown_targets"
  fi

  # build option parameters
  # -----------------------------------
  (( $#without_update )) && WITH_UPDATE=false
  (( $#without_patch )) && WITH_PATCH=false
  (( $#without_emacs )) && WITH_EMACS=false

  [[ -d modules ]] || WITH_UPDATE=true
  [[ -d zephyr ]] || WITH_UPDATE=true
  [[ -d zmk ]] || WITH_UPDATE=true
  [[ -d .west ]] || WITH_UPDATE=true

  #  clean build
  # -----------------------------------
  if $WITH_UPDATE; then
    clean
  fi

  cd $PROJECT

  # build
  # -----------------------------------
  _update_zmk

  for target in $TARGETS; do
    _build $target
    local firmware_file=$(_dist_firmware $target)
    if (( $#with_flash )); then
      (( $#with_logging )) && sudo echo -n
      (( $#with_shell )) && sudo echo -n
      _flash $target $firmware_file
      if (( $#with_logging )); then
        _${os}_log_console $target $firmware_file
      fi
      if (( $#with_shell )); then
        _${os}_shell_console $target $firmware_file
      fi
    fi
  done
}

# subcommand --clean
# clean generated files
#-------------------------------------------
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

# subcommand --clean-modules
#-------------------------------------------
clean_modules() {
  cd $PROJECT
  rm -rf modules
  rm -rf zmk
  rm -rf zephyr
  rm -rf .west
  rm -rf zmk-studio
  clean
}

# subcommand --clean-tools
#-------------------------------------------
clean_tools() {
  cd $PROJECT
  rm -rf $PROJECT/.venv
  rm -rf $PROJECT/.rustenv
  rm -rf ${ZEPHYR_SDK_INSTALL_DIR}/${ZEPHYR_SDK_VERSION}
}

# subcommand --clean-all
#-------------------------------------------
clean_all() {
  clean
  clean_modules
  clean_tools
}

# subcommand --setup
#-------------------------------------------
setup() {
  _${os}_install_packages
  _install_zephyr_sdk

  _install_protoc

  _activate_python_venv
  _install_python_packages

  _activate_rust_venv
  _install_rust_packages

  _install_node_modules

  _download_fonts

  _setup_zmk_studio
}

# subcommand --pip-upgrade
#-------------------------------------------
pip_upgrade() {
  cd $PROJECT
  _install_python_packages
  pip-review -a
  pip3 cache purge
}

rust_upgrade() {
  rustup update stable
}

# subcommand --zephyr-doc2dash
#-------------------------------------------
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

# subcommand --studio-app
#-------------------------------------------
studio_app() {
  _setup_zmk_studio

  cd $PROJECT/zmk-studio

  npx tauri dev
}

# subcommand --studio-web
#-------------------------------------------
studio_web() {
  _setup_zmk_studio

  cd $PROJECT/zmk-studio

  _open_browser 2 http://localhost:5173 &!

  npm run dev
}


# routines
#-------------------------------------------


_fedora_install_packages() {
  # --no-best some copr repos doesn't support fc41 yet
  sudo dnf update --no-best
  # https://docs.zephyrproject.org/3.5.0/develop/getting_started/index.html#select-and-update-os
  sudo dnf install -y wget git cmake gperf python3 dtc wget xz file \
       make gcc SDL2-devel file-libs \
       tio fd-find ripgrep fzf
  # gcc-multilib g++-multilib
  sudo dnf autoremove
  sudo dnf clean all

  winget=$(/mnt/c/Windows/System32/cmd.exe /C "where winget" 2> /dev/null || true)
  if [[ ! -z $winget ]]; then
    winget=${winget%$'\r'}
    winget=$(wslpath -u $winget)
    $winget install dorssel.usbipd-win YaSuenag.SimpleCom || true
  fi
}

_macos_install_packages() {
  # https://docs.zephyrproject.org/3.5.0/develop/getting_started/index.html#select-and-update-os
  brew update
  brew install wget git cmake gperf python3 qemu dtc libmagic \
       doxygen graphviz librsvg \
       tio fd
  brew cleanup
}

_activate_python_venv() {
  cd $PROJECT
  if [[ ! -d .venv ]]; then
    python3 -m venv .venv
    source .venv/bin/activate
    pip3 install rustenv
  else
    source .venv/bin/activate
  fi
}

_activate_rust_venv() {
  cd $PROJECT
  if [[ ! -d .rustenv ]]; then
    rustenv .rustenv
  fi
  source .rustenv/bin/activate
}

_activate_direnv() {
  cd $PROJECT
  direnv allow
  eval "$(direnv export zsh)"
}


_install_python_packages() {
  pip3 install west
  pip3 install -r https://raw.githubusercontent.com/zephyrproject-rtos/zephyr/v$ZEPHYR_VERSION/scripts/requirements.txt
  pip3 install -r https://raw.githubusercontent.com/zephyrproject-rtos/zephyr/v$ZEPHYR_VERSION/doc/requirements.txt
  pip3 install ninja
  pip3 install pip-review
  pip3 install doc2dash
  pip3 cache purge
}

_install_rust_packages() {
  # cargo install icu_tool
}

_install_zephyr_sdk() {
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

  # wget2 as wget in fedora40
  if [[ $os == "fedora" ]]; then
    sed -i -e "s/wget -q --show-progress/wget -q/g" setup.sh
  fi

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
}

_install_protoc() {
  local protoc_dir="${PROTOC_INSTALL_DIR}/protoc-${PROTOC_VERSION}"
  if [[ ! -d $protoc_dir ]]; then
    mkdir -p $protoc_dir
    cd $protoc_dir

    # https://github.com/protocolbuffers/protobuf/releases/download/v28.2/protoc-28.2-osx-x86_64.zip
    # https://github.com/protocolbuffers/protobuf/releases/download/v28.2/protoc-28.2-osx-aarch_64.zip
    # https://github.com/protocolbuffers/protobuf/releases/download/v28.2/protoc-28.2-osx-universal_binary.zip
    # https://github.com/protocolbuffers/protobuf/releases/download/v28.2/protoc-28.2-linux-x86_64.zip
    # https://github.com/protocolbuffers/protobuf/releases/download/v28.2/protoc-28.2-linux-aarch_64.zip

    local file=protoc-${PROTOC_VERSION}-
    if [[ $HOST_OS == macos ]]; then
      file=${file}osx-
    elif [[ $HOST_OS == linux ]]; then
      file=${file}linux-
    fi
    if [[ $HOST_ARCHITECTURE == aarch64 ]]; then
      file=${file}aarch_64.zip
    elif [[ $HOST_ARCHITECTURE == x86_64 ]]; then
      file=${file}x86_64.zip
    fi
    wget "https://github.com/protocolbuffers/protobuf/releases/download/v${PROTOC_VERSION}/$file"
    unzip $file
    rm -f $file
  fi

  # remove old protoc
  cd "$PROTOC_INSTALL_DIR"
  for protoc in protoc-*; do
    if [[ $protoc != protoc-${PROTOC_VERSION} ]]; then
      rm -rf $protoc
    fi
  done
}

_install_node_modules() {
  cd $PROJECT
  npm install
}

_download_fonts() {
  cd $PROJECT
  mkdir -p fonts
  cd fonts

  # MIT license
  if [[ ! -d CozetteFonts ]]; then
    wget https://github.com/slavfox/Cozette/releases/download/v.1.26.0/CozetteFonts-v-1-26-0.zip
    unzip CozetteFonts-v-1-26-0.zip
    rm -f CozetteFonts-v-1-26-0.zip
  fi

  # OFL-1.1 license
  if [[ ! -d fonts-DSEG_v046 ]]; then
    wget https://github.com/keshikan/DSEG/releases/download/v0.46/fonts-DSEG_v046.zip
    unzip fonts-DSEG_v046.zip
    rm -f fonts-DSEG_v046.zip
  fi

  # M+ FONT license
  if [[ ! -d PixelMplus-20130602 ]]; then
    wget https://github.com/itouhiro/PixelMplus/releases/download/v1.0.0/PixelMplus-20130602.zip
    unzip PixelMplus-20130602.zip
    rm -f PixelMplus-20130602.zip
  fi

  # OFL-1.1 license
  if [[ -d teko ]]; then
    cd teko
    git pull
    cd ..
  else
    git clone --depth 1 https://github.com/googlefonts/teko.git
  fi

  # unknown license
  # https://www.2112.net/rushfonts
  if [[ ! -d rushfonts ]]; then
    mkdir  rushfonts
    wget https://www.2112.net/rushfonts/rushfonts.zip
    unzip -o rushfonts.zip -d rushfonts
    rm -f rushfonts.zip
  fi

  # completly free for any prupose
  # http://fontlab.web.fc2.com/extra-small-font.html
  if [[ ! -d extra-small-font ]]; then
    wget https://ftp.vector.co.jp/66/05/114/extra-small-font.zip
    unzip extra-small-font.zip
    rm -f extra-small-font.zip
  fi

  # OFL-1.1 license
  if [[ -d font_Tiny5 ]]; then
    cd font_Tiny5
    git pull
    cd ..
  else
    git clone --depth 1 https://github.com/Gissio/font_Tiny5.git
  fi

  # OFL-1.1 license
  if [[ -d soft-type-micro ]]; then
    cd soft-type-micro
    git pull
    cd ..
  else
    git clone --depth 1 https://github.com/scfried/soft-type-micro.git
  fi
}

_setup_zmk_studio() {
  cd $PROJECT
  if [[ -d zmk-studio ]]; then
    cd zmk-studio
    local local_rev=$(git rev-parse $ZMK_STUDIO_BRANCH)
    local remote_rev=$(git ls-remote --heads origin $ZMK_STUDIO_BRANCH | awk '{print $1}')
    if [[ $local_rev != $remote_rev ]]; then
      git reset --hard HEAD
      cp package.json package.json.old
      git pull
      for patch in $PROJECT/patches/studio_app_${ZMK_STUDIO_BRANCH}_*.patch(N); do
        git apply -3 --verbose $patch
      done
      if [[ ! -z $(diff package.json package.json.old) ]]; then
        npm install
      fi
      rm package.json.old
    fi
  else
    cd $PROJECT
    git clone --depth 1 -b $ZMK_STUDIO_BRANCH https://github.com/zmkfirmware/zmk-studio.git
    cd zmk-studio
    for patch in $PROJECT/patches/studio_app_${ZMK_STUDIO_BRANCH}_*.patch(N); do
      git apply -3 --verbose $patch
    done
    npm install
  fi
}

_open_browser() {
  sleep $1
  if [[ $os == "macos" ]]; then
    open $2
  elif [[ $os = "fedora" ]]; then
    /mnt/c/Windows/System32/cmd.exe /c start $2 2> /dev/null
  fi
}

_update_zmk() {
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
      # zmk draft
      local draft_patches=(../patches/draft_zmk_*.patch(N))
     if [[ ! -z $draft_patches ]]; then
        git apply -3 --verbose $draft_patches
      fi
      git apply -3 --verbose ../patches/zmk_*.patch
      cd ..
    fi

    west zephyr-export
  fi
}

_build() {
  local target=$1
  local -A props=("${(Pkv@)target}")
  local opts=(--board $props[board] --build-dir build/$target)
  local defs=("-DZMK_CONFIG='$PROJECT/zmk_keyboards'")

  cd $PROJECT

  if (( $#with_clean )); then
    rm -rf build/$target
  fi

  if $WITH_UPDATE || ((  $#with_clean )); then
    opts+=(--pristine=always)
  else
    opts+=(--pristine=auto)
  fi

  if (( $#with_studio )) || $props[studio] && ! (( $#without_studio )); then
    if ! $props[ble]; then
      opts+=(--snippet studio-rpc-usb-uart)
    fi
    defs+=(-DCONFIG_ZMK_STUDIO=y)
    if (( $#with_logging )); then
      defs+=(-DCONFIG_ZMK_STUDIO_LOG_LEVEL_DBG=y)
    fi
  fi

  if (( $#with_logging )); then
    opts+=(--snippet usb-logging)
    if (( ${+props[log_opts]} )); then
      for log_opt in ${(@s/,/)props[log_opts]}; do
        defs+=("-D$log_opt=y")
      done
    fi
  fi

  if (( $#with_shell )); then
    opts+=(--snippet usb-shell)
  fi

  (( $#with_pp )) && defs+=(-DEXTRA_CFLAGS=-save-temps=obj)

  if (( ${+props[shields]} )); then
    defs+=("-DSHIELD='$props[shields]'")
  fi

  if (( $#with_compile_db )); then
    defs+=(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
    _dot_clangd
    if $WITH_EMACS; then
      _dot_dir_locals $target
      _dot_projectile
    fi
  fi

  if $props[ble]; then
    defs+=(-DEXTRA_CONF_FILE="$PROJECT/zmk_keyboards/ble_default.conf")
  fi

  print -Pr  "%F{green}---------------- start west build -- command-line----------------------"
  print -r "west build $opts[*] zmk/app -- $defs[*]"
  print -Pr  "%F{green}-----------------------------------------------------------------------"

  west build $opts[*] zmk/app -- $defs[*]

  if (( $#with_ram_report )); then
    west build --target ram_report --build-dir build/$target
  fi

  if (( $#with_compile_db )); then
    mv $PROJECT/build/$target/compile_commands.json $PROJECT
  fi
}

_dot_clangd() {
  cat <<EOS > $PROJECT/.clangd
CompileFlags:
  Remove: [-mfp16-format*, -fno-reorder-functions, -fno-printf-return-value]
EOS
  rm -rf "${PROJECT}/.cache"
}

_dot_dir_locals() {
  local target=$1
  cat <<EOS > $PROJECT/.dir-locals.el
((nil . ((projectile-git-use-fd . t)
         (projectile-git-fd-args . "--hidden --no-ignore -0 --exclude '\.*' --type f --strip-cwd-prefix")
         (counsel-rg-base-command . ("rg" "--no-ignore" "--max-columns" "240" "--with-filename" "--no-heading" "--line-number" "--color" "never" "%s"))
         (compile-command . "zmk_build.sh -wp $target")
)))
EOS
}

_dot_projectile() {
  cat <<EOS > $PROJECT/.projectile
-/dist
-/node_modules
-/xpacks
-/via_app
-/qmk_firmware
-/qmk_keyboards
-/zmk-studio
EOS
}

# copy & rename firmware
# $1 board
# $2 firmware name
# $3 firmware typee
# return echo path or firmware
# -----------------------------------
_dist_firmware() {
  local target=$1
  local -A props=("${(Pkv@)target}")

  cd $PROJECT
  cd zmk
  local version=$(date +"%Y%m%d")_zmk_$(git rev-parse --short HEAD)
  cd ..
  mkdir -p dist
  local src=build/$target/zephyr/zmk.$props[firmware_type]
  local variant=""
  (( $#with_logging )) && variant="${variant}_logging"
  (( $#with_shell )) && variant="${variant}_shell"
  (( $#with_studio )) || $props[studio] && ! (( $#without_studio )) && variant="${variant}_studio"
  local dst=dist/${props[name]}_${version}${variant}.$props[firmware_type]
  cp $src $dst
  echo $dst
}

_flash() {
  local target=$1
  local firmware=$2
  local -A props=("${(Pkv@)target}")
  _${os}_flash_${props[firmware_type]} $target $firmware
}
_macos_flash_uf2() {
  local target=$1
  local firmware=$2
  local -A props=("${(Pkv@)target}")
  local dfu_volume=/Volumes/$props[dfu_volume]

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

_fedora_flash_uf2() {
  local target=$1
  local firmware=$2
  local -A props=("${(Pkv@)target}")

  echo -n "waiting for DFU volume to be mounted..."
  while true; do
    dfu_drive=$($WIN_WMIC logicaldisk get deviceid, volumename | grep $props[dfu_volume] | awk '{print $1}')
    if [[ ! -z $dfu_drive ]]; then
      echo ""
      echo "copying firmware [$firmware] to drive [$dfu_drive]..."
      sleep 1
      $WIN_SUDO --inline c:\\Windows\\System32\\xcopy.exe "$(wslpath -w $firmware)" $dfu_drive\\
      echo "flashing firmware finished successfully."
      break
    else
      echo -n "."
      sleep 1
    fi
  done
}

_macos_flash_bin() {
  local target=$1
  west flash --build-dir build/$target || true
}

_fedora_flash_bin() {
  local target=$1
  local -A props=("${(Pkv@)target}")
  local hardware_id="$props[dfu_vid]:$props[dfu_pid]"

  sudo echo -n
  echo -n "waiting for target DFU device to be connected.."
  while true; do
    dfu_device=$($WIN_USBIPD list 2> /dev/null | grep "$hardware_id" || echo -n "")
    if [[ ! -z $dfu_device ]]; then
      if [[ $dfu_device =~ "Not shared" ]]; then
        $WIN_SUDO --inline "$(wslpath -w $WIN_USBIPD)" bind --hardware-id $hardware_id
      elif [[ $dfu_device =~ "Shared" ]]; then
        $WIN_SUDO --inline "$(wslpath -w $WIN_USBIPD)" attach --wsl --hardware-id $hardware_id
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

_macos_open_log_window() {
  local log_file=$1
  local tty_dev=$2

  osascript <<-EOF
tell application "iTerm2"
  tell current session of (create window with default profile)
    write text "cd $PROJECT"
    write text "tio --log --log-file=$log_file $tty_dev"
  end tell
end tell
EOF
}

_macos_log_console() {
  local target=$1
  local firmware=$2
  local -A props=("${(Pkv@)target}")

  local log_file=logs/${firmware:t:r}.txt
  local tty_devs=()
  local tty_dev=
  local not_found=true

  cd $PROJECT

  echo -n "waiting for debug output device to be connected.."
  while $not_found; do
    for tty_dev in /dev/tty.usbmodem*(N); do
      if [[ $tty_dev -nt $firmware ]]; then
        not_found=false
      fi
    done
    sleep 1
    echo -n "."
  done

  # there may be multiple tty devices studio/log/shell
  for tty_dev in /dev/tty.usbmodem*(N); do
    if [[ $tty_dev -nt $firmware ]]; then
      tty_devs+=($tty_dev)
    fi
  done

  if [[ $#tty_devs = 1 ]]; then
    tty_dev=$tty_devs[1]
  elif [[ $#tty_devs -gt 1 ]]; then
    # sort asc
    tty_devs=(${(o)tty_devs})
    if (( $#with_studio )) || $props[studio] && ! (( $#without_studio )) && ! $props[ble]; then
      # studio enabled
      # 1 - studio uart
      # 2 - loggging uart
      tty_dev=$tty_devs[2]
    else
      tty_dev=$tty_devs[1]
    fi
  else
    _error_exit 1 'tty device not found'
  fi

  echo "\nFound tty device [$tty_dev]. To exit logging, [Ctrl + t][q]"
  sudo chmod +ur $tty_dev
  mkdir -p logs
  rm -f $log_file

  if (( $#with_shell )); then
    # open with another terminal window
    _macos_open_log_window $log_file $tty_dev
  else
    tio --log --log-file=$log_file $tty_dev
  fi
}

# TODO
# - Doesn't work if multiple ZMK keyboards are connected
_fedora_log_console() {
  local target=$1
  local firmware=$2
  local -A props=("${(Pkv@)target}")
  local com_port

  cd $PROJECT

  echo -n "waiting for logging serial device to be connected.."
  _wait_com_port

  local com_ports=($(_list_com_ports))

  if [[ $#com_ports = 1 ]]; then
    com_port=$com_ports[1]
  elif [[ $#com_ports -gt 1 ]]; then
    if (( $#with_studio )) || $props[studio] && ! (( $#without_studio )) && ! $props[ble]; then
      # studio enabled
      # 1 - studio uart
      # 2 - loggging uart
      com_port=$com_ports[2]
    else
      com_port=$com_ports[1]
    fi
  else
    _error_exit 1 'logging COM port not found'
  fi

  echo "\nFound logging COM port [$com_port]. To exit logging, [F1]"

  # TODO log file
  _open_windows_terminal $com_port
}

_macos_shell_console() {
  local target=$1
  local firmware=$2
  local -A props=("${(Pkv@)target}")
  local cu_devs=()
  local cu_dev=

  cd $PROJECT

  if [[ $#with_logging = 0 ]]; then
    local not_found=true

    echo -n "waiting for debug shell uart device to be connected.."
    while $not_found; do
      for cu_dev in /dev/cu.usbmodem*(N); do
        if [[ $cu_dev -nt $firmware ]]; then
          not_found=false
        fi
      done
      sleep 1
      echo -n "."
    done
  fi

  # there may be multiple cu devices studio/log/shell
  for cu_dev in /dev/cu.usbmodem*(N); do
    if [[ $cu_dev -nt $firmware ]]; then
      cu_devs+=($cu_dev)
    fi
  done

  if (( $#cu_devs)); then
    # sort desc
    echo "cu_devs=(${cu_devs})"
    cu_devs=(${(O)cu_devs})
    echo "sorted cu_devs=(${cu_devs})"
    # last element
    cu_dev=${cu_devs[1]}
  else
     _error_exit 1 'cu device not found'
  fi

  echo "\nFound cu device [$cu_dev]. To exit shell, [~] <wait a second> [.][enter]"
  sudo cu -l $cu_dev
}

# TODO
# - Doesn't work if multiple ZMK keyboards are connected
_fedora_shell_console() {
  local target=$1
  local firmware=$2
  local -A props=("${(Pkv@)target}")
  local com_port

  cd $PROJECT

  if [[ $#with_logging = 0 ]]; then
    echo -n "waiting for shell serial device to be connected.."
    _wait_com_port
  fi

  local com_ports=($(_list_com_ports))

  if (( $#com_ports )); then
    # last element
    com_port=$com_ports[-1]
  else
    _error_exit 1 'shell COM port not found'
  fi

  echo "\nFound shell COM port [$com_port]. To exit shell, [F1]"

  _open_windows_terminal $com_port
}

_win_env() {
  /mnt/c/Windows/System32/cmd.exe /c echo "%${1}%" 2>/dev/null | tr -d '\r'
}

_list_com_ports() {
  # TODOk if multiple ZMK keyboards are connected
  $WIN_WMIC path Win32_SerialPort get deviceid, PNPDeviceID 2> /dev/null | grep -e $WIN_HARDWARE_ID0 -e $WIN_HARDWARE_ID1 | awk '{print $1}'
}

_wait_com_port() {
  local com_ports=()
  while [[ $#com_ports = 0 ]]; do
    com_ports=($(_list_com_ports))
    sleep 1
    echo -n "."
  done
}

_open_windows_terminal() {
  local com_port=$1
  $(wslpath $(_win_env LOCALAPPDATA))/$WIN_TERMINAL --window new $(wslpath -w $WIN_SIMPLE_COM) $com_port
}

main
