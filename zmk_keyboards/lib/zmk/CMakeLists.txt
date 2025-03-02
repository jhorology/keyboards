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

  target_sources_ifdef(CONFIG_ZMK_INDICATOR_LED app PRIVATE indicator_led.c)

  target_sources_ifdef(CONFIG_ZMK_PWM_BUZZER app PRIVATE pwm_buzzer.c)
  if (CONFIG_ZMK_BEEP)
    target_sources(app PRIVATE beep.c)
    target_sources(app PRIVATE behaviors/behavior_beep.c)
  endif()

  target_sources(app PRIVATE behaviors/behavior_hold_tap_dance.c)

endif()



# TODO generalize
if (CONFIG_BOARD_CYBER60_REV_D)
  target_sources(app PRIVATE cyber60_led.c)
endif()

add_subdirectory(display)
