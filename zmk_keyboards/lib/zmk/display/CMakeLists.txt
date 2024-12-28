add_subdirectory(fonts)
add_subdirectory(img)
add_subdirectory(widgets)

target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM_80X128_MONO app PRIVATE status_screen_80x128_mono.c)

if (CONFIG_ZMK_CUSTOM_KEYBOARD_LOGO)
  if (CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM_80X128_MONO)
    # custom logo image should be named "<keymap name>_80x80.png" and
    # placed in same directory with .keymap file
    set(keyboard_logo_file "${KEYMAP_FILE}")
    cmake_path(REMOVE_EXTENSION keyboard_logo_file LAST_ONLY)
    set(keyboard_logo_file "${keyboard_logo_file}_80x80.png")
    if (EXISTS ${keyboard_logo_file})
      message(STATUS "Keyboard logo image: ${keyboard_logo_file}")
      include(${ZEPHYR_CURRENT_MODULE_DIR}/cmake/lv_img_conv.cmake NO_POLICY_SCOPE)
      generate_lv_image_for_target(app ${keyboard_logo_file} keyboard_logo_80x80 CF_INDEXED_1_BIT)
    endif()
  endif()
endif()
