# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_property(ALLOW_EMPTY TRUE)

zephyr_library_sources_ifdef(CONFIG_INPUT_ANALOG_AXIS input_analog_axis.c)
zephyr_library_sources_ifdef(CONFIG_INPUT_ANALOG_AXIS_SETTINGS input_analog_axis_settings.c)
