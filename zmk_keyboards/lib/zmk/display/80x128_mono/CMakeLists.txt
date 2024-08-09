if (CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM_80x128_MONO)
  target_sources(app PRIVATE custom_status_screen.c)
  target_sources(app PRIVATE zmklogo.c)
endif()
