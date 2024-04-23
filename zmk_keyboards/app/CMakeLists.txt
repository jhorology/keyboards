target_include_directories(app PRIVATE include)

if (CONFIG_ZMK_USB_HOST_OS)
  target_sources(app PRIVATE src/usb_host_os.c)
  target_sources(app PRIVATE src/events/usb_host_os_changed.c)
  target_sources_ifdef(CONFIG_ZMK_USB_AUTO_SWITCH_LAYER app PRIVATE src/usb_auto_switch_layer.c)
endif()

target_sources_ifdef(CONFIG_ZMK_CUSTOM_SHELL_CMD app PRIVATE src/shell/zmk_cmd.c)

if (CONFIG_ZMK_RADIAL_CONTROLLER)
  target_sources(app PRIVATE src/behaviors/behavior_rc_button.c)
  target_sources(app PRIVATE src/behaviors/behavior_rc_dial.c)
endif()

if (CONFIG_ZMK_INDICATOR_LED)
  target_sources(app PRIVATE src/indicator_led.c)
endif()

if (CONFIG_ZMK_RAW_HID_TEST)
  target_sources(app PRIVATE src/keymap_usb_hid.c)
  if (CONFIG_ZMK_BLE)
    target_sources(app PRIVATE src/keymap_hog.c)
  endif()
endif()
