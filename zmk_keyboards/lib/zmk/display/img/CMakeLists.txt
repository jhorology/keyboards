if (CONFIG_ZMK_IMG_LOGO_40X40)
  generate_lv_image_for_target(app zmk_logo_40x40.png "zmk_logo_40x40" CF_ALPHA_1_BIT)
endif()

if (CONFIG_ZMK_IMG_STARMAN_68X80)
  generate_lv_image_for_target(app starman_68x80.png "starman_68x80" CF_ALPHA_1_BIT)
endif()
