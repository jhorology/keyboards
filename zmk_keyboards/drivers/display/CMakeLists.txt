# Copyright (c) 2021 The ZMK Contributors
# SPDX-License-Identifier: MIT

zephyr_library_amend()

zephyr_library_sources_ifdef(CONFIG_YA_UC8175 uc8175.c)
