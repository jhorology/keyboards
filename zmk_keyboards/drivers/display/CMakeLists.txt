# Copyright (c) 2021 The ZMK Contributors
# SPDX-License-Identifier: MIT

zephyr_library_amend()

zephyr_library_sources_ifdef(CONFIG_UC8175_YA uc8175.c)
zephyr_library_sources_ifdef(CONFIG_LS0XX_YA ls0xx.c)
