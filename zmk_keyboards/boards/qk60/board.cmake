# Copyright (c) 2023 The ZMK Contributors
# SPDX-License-Identifier: MIT

board_runner_args(dfu-util "--pid=1688:2220" "--alt=0" "--dfuse")
include(${ZEPHYR_BASE}/boards/common/dfu-util.board.cmake)
