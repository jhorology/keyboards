if (CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM_80X128_MONO)
  target_sources(app PRIVATE custom_status_screen.c)
  target_sources_ifdef(CONFIG_ZMK_WIDGET_BATTERY_STATUS_80X128_MONO app PRIVATE custom_battery_status.c)
  target_sources_ifdef(CONFIG_ZMK_WIDGET_OUTPUT_STATUS_80X128_MONO app PRIVATE custom_output_status.c)

  if ((NOT CONFIG_ZMK_SPLIT) OR CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
    target_sources(app PRIVATE zmk_logo_40x40.c)
  endif()

  if (CONFIG_ZMK_SPLIT AND (NOT CONFIG_ZMK_SPLIT_ROLE_CENTRAL))
    # custom logo image should be named "<keymap name>_80x80.png" and
    # placed in same directory with .keymap file
    set(custom_logo_file "${KEYMAP_FILE}")
    cmake_path(REMOVE_EXTENSION custom_logo_file LAST_ONLY)
    set(custom_logo_file "${custom_logo_file}_80x80.png")
    if (EXISTS ${custom_logo_file})
      message(STATUS "Custom logo image: ${custom_logo_file}")
      include(${ZEPHYR_CURRENT_MODULE_DIR}/cmake/lv_img_conv.cmake NO_POLICY_SCOPE)
      generate_lv_image_for_target(app ${custom_logo_file} custom_logo_80x80 CF_INDEXED_1_BIT)
    else()
      target_sources(app PRIVATE custom_logo_80x80.c)
     endif()
  endif()
endif()
