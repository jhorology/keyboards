if ((NOT CONFIG_ZMK_SPLIT) OR CONFIG_ZMK_SPLIT_ROLE_CENTRAL)

  if (CONFIG_ZMK_USB_HOST_OS)
    target_sources(app PRIVATE usb_host_os.c)
    target_sources(app PRIVATE events/usb_host_os_changed.c)
    target_sources_ifdef(CONFIG_ZMK_USB_AUTO_SWITCH_LAYER app PRIVATE usb_auto_switch_layer.c)
  endif()

  target_sources_ifdef(CONFIG_ZMK_CUSTOM_SHELL_CMD app PRIVATE shell/zmk_cmd.c)

  if (CONFIG_ZMK_RADIAL_CONTROLLER)
    target_sources(app PRIVATE behaviors/behavior_rc_button.c)
    target_sources(app PRIVATE behaviors/behavior_rc_dial.c)
  endif()

  if (CONFIG_ZMK_INDICATOR_LED)
    target_sources(app PRIVATE indicator_led.c)
  endif()

  if (CONFIG_ZMK_RAW_HID_TEST)
    target_sources(app PRIVATE keymap_usb_hid.c)
    if (CONFIG_ZMK_BLE)
      target_sources(app PRIVATE keymap_hog.c)
    endif()
  endif()
endif()

# TODO generalize
if (CONFIG_BOARD_CYBER60_REV_D)
  target_sources(app PRIVATE cyber60_buzzer.c)
  target_sources(app PRIVATE cyber60_led.c)
endif()

add_subdirectory(display)
