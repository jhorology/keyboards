target_include_directories(app PRIVATE include)

if (CONFIG_ZMK_USB_HOST_OS)
  target_sources(app PRIVATE src/usb_host_os.c)
  target_sources(app PRIVATE src/events/usb_host_os_changed.c)
endif()
target_sources_ifdef(CONFIG_ZMK_USB_AUTO_SWITCH_LAYER app PRIVATE src/usb_auto_switch_layer.c)

if (CONFIG_ZMK_RADIAL_CONTROLLER)
  target_sources(app PRIVATE src/behaviors/behavior_rc_button.c)
  target_sources(app PRIVATE src/behaviors/behavior_rc_dial.c)
endif()
