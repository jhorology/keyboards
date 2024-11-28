if (CONFIG_CUSTOM_SHELL_CMD)
  zephyr_library_sources_ifdef(CONFIG_PICOSDK_USE_FLASH rp2flash.c)
  zephyr_library_sources_ifdef(CONFIG_INPUT_ANALOG_AXIS analog_axis.c)
endif()
