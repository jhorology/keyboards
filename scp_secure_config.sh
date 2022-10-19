#!/bin/zsh -eu

cd "$(dirname $0)"

PROJECT=$(realpath --relative-to="$HOME" .)

if [ ! -s .config ]; then
  print -r "Error: Missing .config file." >&2
  exit 1
fi

source .config

if [[ ! ${MAIN_DEV_HOST-} ]]; then
  print -r "Error: MAIN_DEV_HOST is not defined." >&2
  exit 1
fi

if [ $(uname -n) = $MAIN_DEV_HOST ]; then
  print -r "Error: this is MAIN_DEV_HOST." >&2
  exit 1
fi


for target in qmk_keyboards/*/
do
  if [ $target != "qmk_keyboards/lib/" ]; then
     scp $MAIN_DEV_HOST:"~/${PROJECT}/${target}secure_config.h" "${target}"
  fi
done
