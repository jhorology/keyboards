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
# DOCSETS_DIR="$HOME/Library/Application Support/Dash/DockSets"
ZMK_STUDIO_BRANCH=main
PROTOC_INSTALL_DIR=$PROJECT/.local
PROTOC_VERSION=28.2

_error_exit() {
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
      _error_exit 1 'unsupported platform.'
    fi
    ;;
  * )
    _error_exit 1 'unsupported platform.'
    ;;
esac


# key: target
#   [1]=board
#   [2]=firmwre_name
#   [3]=firmware file type
#   [4]=DFU volume name or device vid
#   [5]=DFU device pid
#   [6]=shields
#   [7]=-additional options when --with-logging
local -A KEYBOARDS=(
  cz42l      nice_nano_v2:cz42_left:uf2:CZ42_L:none:"cz42_left;gooddisplay_adapter;gooddisplay_gdew0102t4":"CONFIG_LV_LOG_LEVEL_TRACE,CONFIG_DISPLAY_LOG_LEVEL_DBG"
  cz42r      nice_nano_v2:cz42_right:uf2:CZ42_R:none:"cz42_right;gooddisplay_adapter;gooddisplay_gdew0102t4":"CONFIG_LV_LOG_LEVEL_TRACE,CONFIG_DISPLAY_LOG_LEVEL_DBG"
  d60        bt60:d60_lite_hhkb_ec11:uf2:CKP:none:none:none
  fk68       fk680pro_v2:fk680pro_v2:uf2:"ZhaQian DFU":none:none:none
  libra      nice_nano_v2:libra_mini:uf2:LIBRA_MINI:none:libra_mini:"CONFIG_INPUT_LOG_LEVEL_DBG,CONFIG_ADC_LOG_LEVEL_DBG"
  rz42l      rz42_left:rz42_left:uf2:RZ42_L:none:nice_view:none
  rz42r      rz42_right:rz42_right:uf2:RZ42_R:none:nice_view:none
  q60        keychron_q60:keychron_q60:bin:0483:df11:none:none
  qk60       qk60_wired:qk60_wired_hhkb:bin:1688:2220:none:none
  tf60       kbdfans_tofu60_v2:tofu60_hhkb:uf2:RPI-RP2:none:none:none
  ju60       cyber60_rev_d:hibi_june60:uf2:CYBER60_D:none:none:none
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
        "    -r,--with-ram-report  Enable ram_report" \
        "    --without-emacs       don't generate emacs settings when --with-comile-db" \
       "" \
        "available targets:"
  for target in ${(k)KEYBOARDS}; do
    print -rC2 -- "   ${target}:"  "${KEYBOARDS[$target]}"
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
    if ! (( $+KEYBOARDS[$target] )); then
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
  if $WITH_UPDATE || (( $#with_clean )); then
    clean
  fi

  cd $PROJECT

  # build
  # -----------------------------------
  _update_zmk

  for target in $TARGETS; do
    kbd=(${(@s/:/)KEYBOARDS[$target]})
    board=$kbd[1]
    firmware_name=$kbd[2]
    firmware_ext=$kbd[3]
    shields=$kbd[6]

    _build $target

    firmware_file=$(_dist_firmware $target)
    if (( $#with_flash )); then
      (( $#with_logging )) && sudo echo -n
      (( $#with_shell )) && sudo echo -n
      _flash $target $firmware_file
      if (( $#with_logging )); then
        _${os}_log_console $firmware_file
      elif (( $#with_shell )); then
        _${os}_shell_console $firmware_file
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
    $winget install dorssel.usbipd-win || true
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
  # echo $path
  # env
  # exit 0
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
  if [[ ! -d CozetteFonts ]]; then
    wget https://github.com/slavfox/Cozette/releases/download/v.1.25.2/CozetteFonts-v-1-25-2.zip
    unzip CozetteFonts-v-1-25-2.zip
    rm -f CozetteFonts-v-1-25-2.zip
  fi

  if [[ ! -d fonts-DSEG_v046 ]]; then
    wget https://github.com/keshikan/DSEG/releases/download/v0.46/fonts-DSEG_v046.zip
    unzip fonts-DSEG_v046.zip
    rm -f fonts-DSEG_v046.zip
  fi

  if [[ ! -d PixelMplus-20130602 ]]; then
    wget https://github.com/itouhiro/PixelMplus/releases/download/v1.0.0/PixelMplus-20130602.zip
    unzip PixelMplus-20130602.zip
    rm -f PixelMplus-20130602.zip
  fi

  if [[ -d teko ]]; then
    cd teko
    git pull
  else
    git clone --depth 1 https://github.com/googlefonts/teko.git
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
  local kbd=(${(@s/:/)KEYBOARDS[$target]})
  local board=$kbd[1]
  local shields=$kbd[6]
  local log_options=$kbd[7]
  local opts=(--board $board --build-dir build/$target)
  local defs=("-DZMK_CONFIG='$PROJECT/zmk_keyboards'")

  if $WITH_UPDATE; then
    opts+=(--pristine=always)
  else
    opts+=(--pristine=auto)
  fi

  if (( $#with_studio )); then
    opts+=(--snippet studio-rpc-usb-uart)
    defs+=(-DCONFIG_ZMK_STUDIO=y)
    if (( $#with_logging )); then
      defs+=(-DCONFIG_ZMK_STUDIO_LOG_LEVEL_DBG=y)
    fi
  fi
  if (( $#with_logging )); then
    opts+=(--snippet zmk-usb-logging)
    defs+=(-DCONFIG_LOG_THREAD_ID_PREFIX=y)
    if [[ $log_options != none ]]; then
      echo $log_options
      for log_opt in ${(@s/,/)log_options}; do
        defs+=("-D$log_opt=y")
      done
    fi
  fi
  if (( $#with_shell )); then
    opts+=(--snippet usb-shell)
  fi
  (( $#with_pp )) && defs+=(-DEXTRA_CFLAGS=-save-temps=obj)
  [[ $shields != none ]] && defs+=("-DSHIELD='$shields'")
  if (( $#with_compile_db )); then
    defs+=(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
    _dot_clangd
    if $WITH_EMACS; then
      _dot_dir_locals $target
      _dot_projectile
    fi
  fi
  if (( $with_ram_report )); then
    opts+=(--target ram_report)
  fi

  # path=($PROTOC_INSTALL_DIR/protoc-$PROTOC_VERSION/bin $path) \
  #     west build $opts[*] zmk/app -- $defs[*]
  west build $opts[*] zmk/app -- $defs[*]

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
-/build
-/node_modules
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
  (( $#with_studio )) && variant="_studio"
  local dst=dist/${firmware_name}_${version}$variant.$firmware_ext
  cp $src $dst
  echo $dst
}

_flash() {
  local target=$1
  local firmware=$2
  local kbd=(${(@s/:/)KEYBOARDS[$target]})
  local firmware_ext=$kbd[3]
  _${os}_flash_${firmware_ext} $target $firmware
}

_macos_flash_uf2() {
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

_fedora_flash_uf2() {
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
      $WIN_SUDO c:\\Windows\\System32\\xcopy.exe "$(wslpath -w $firmware)" $dfu_drive\\
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
  local kbd=(${(@s/:/)KEYBOARDS[$target]})
  local hardware_id="$kbd[4]:$kbd[5]"

  sudo echo -n
  echo -n "waiting for target DFU device to be connected.."
  while true; do
    dfu_device=$($WIN_USBIPD list 2> /dev/null | grep "$hardware_id" || echo -n "")
    if [[ ! -z $dfu_device ]]; then
      if [[ $dfu_device =~ "Not shared" ]]; then
        $WIN_SUDO "$(wslpath -w $WIN_USBIPD)" bind --hardware-id $hardware_id
      elif [[ $dfu_device =~ "Shared" ]]; then
        $WIN_SUDO "$(wslpath -w $WIN_USBIPD)" attach --wsl --hardware-id $hardware_id
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

_macos_log_console() {
  local firmware=$1
  local log_file=logs/${firmware:t:r}.txt
  local tty_devs=()
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
  sleep 1

  # may exist 2 tty devices log and studio
  for tty_dev in /dev/tty.usbmodem*(N); do
    if [[ $tty_dev -nt $firmware ]]; then
      tty_devs+=($tty_dev)
    fi
  done

  if (( $#tty_devs )); then
    tty_devs=(${(O)tty_devs})
    echo "found tty device [${tty_devs[1]}]"
    # to exit tio, [Ctrl + t][q]
    sudo chmod +urw $tty_devs[1]
    mkdir -p logs
    rm -f $log_file
    tio --log --log-file=$log_file $tty_devs[1]
  else
    _error_exit 1 'unfound tty device'
  fi
}

_fedora_log_console() {
  local firmware=$1

  #TODO uidbipd-win
}

_macos_shell_console() {
  local firmware=$1

  echo -n "waiting for shell device to be connected.."
  while true; do
    echo -n "."
    sleep 1
    for cu_dev in /dev/cu.usbmodem*(N); do
      if [[ $cu_dev -nt $firmware ]]; then
        echo "Found cu device [$cu_dev], To exit shell, [~]<wait a second>[.][enter]"
        sleep 3
        sudo cu -l $cu_dev
        return
      fi
    done
  done
}

_fedora_shell_console() {
  local firmware=$1

  #TODO uidbipd-win
}

main
