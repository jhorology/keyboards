add_subdirectory(fonts)
add_subdirectory(img)
add_subdirectory(widgets)

target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM_80X128_MONO_CENTRAL app PRIVATE status_screen_80x128_mono_central.c)
target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM_80X128_MONO_PERIPHERAL app PRIVATE status_screen_80x128_mono_peripheral.c)
target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM_68X160_MONO_CENTRAL app PRIVATE status_screen_68x160_mono_central.c)
target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM_68X160_MONO_PERIPHERAL app PRIVATE status_screen_68x160_mono_peripheral.c)
target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_PRESENTER app PRIVATE status_presenter.c)

if (CONFIG_ZMK_CUSTOM_KEYBOARD_LOGO)
  if (CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM_80X128_MONO_PERIPHERAL)
    set(keyboard_logo_size "80x80")
  endif()
  if (CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM_68X160_MONO_PERIPHERAL)
    set(keyboard_logo_size "68x80")
  endif()

  # custom logo image should be named "<keymap name>_<width>x<height>.png" and
  # placed in same directory with .keymap file
  set(keyboard_logo_file "${KEYMAP_FILE}")
  cmake_path(REMOVE_EXTENSION keyboard_logo_file LAST_ONLY)
  set(keyboard_logo_file "${keyboard_logo_file}_${keyboard_logo_size}.png")
  if (EXISTS ${keyboard_logo_file})
    message(STATUS "Keyboard logo image: ${keyboard_logo_file}")
    include(${ZEPHYR_CURRENT_MODULE_DIR}/cmake/lv_img_conv.cmake NO_POLICY_SCOPE)
    generate_lv_image_for_target(app ${keyboard_logo_file} "keyboard_logo_${keyboard_logo_size}" CF_INDEXED_1_BIT)
  endif()
endif()
