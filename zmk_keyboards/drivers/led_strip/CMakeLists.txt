# Copyright (c) 2026 The ZMK Contributors
# SPDX-License-Identifier: MIT

zephyr_library_amend()

zephyr_library_sources_ifdef(CONFIG_SNLED27351 snled27351.c)
