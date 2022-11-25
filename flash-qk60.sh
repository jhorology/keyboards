#!/bin/zsh

PROJECT=$(realpath $0:h)
cd "$PROJECT"

dfu-util -a 0 -d 1688:2220 -s 0x08006000:leave -D "$1"
