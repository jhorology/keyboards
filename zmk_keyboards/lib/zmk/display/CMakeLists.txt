include(${ZEPHYR_CURRENT_MODULE_DIR}/cmake/lv_img_conv.cmake NO_POLICY_SCOPE)

add_subdirectory(fonts)
add_subdirectory(img)
add_subdirectory(custom_widgets)

target_sources_ifdef(CONFIG_ZMK_DISPLAY_CUSTOM_PORTRAIT_MONO_MAIN app PRIVATE portrait_mono_main.c)
target_sources_ifdef(CONFIG_ZMK_DISPLAY_CUSTOM_PORTRAIT_MONO_PERIPHERAL app PRIVATE portrait_mono_peripheral.c)
target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_PRESENTER app PRIVATE status_presenter.c)

if (CONFIG_ZMK_CUSTOM_KEYBOARD_LOGO)
  # custom logo image should be named "<keymap name>.png" and
  # placed in same directory with .keymap file
  set(keyboard_logo_file "${KEYMAP_FILE}")
  cmake_path(REMOVE_EXTENSION keyboard_logo_file LAST_ONLY)
  set(keyboard_logo_file "${keyboard_logo_file}.png")
  if (EXISTS ${keyboard_logo_file})
    message(STATUS "Keyboard logo image: ${keyboard_logo_file}")
    generate_lv_image_for_target(app ${keyboard_logo_file} "keyboard_logo" CF_ALPHA_1_BIT)
  endif()
endif()
