#define NUM_TAP_DANCE_PREDEFINED_ENTRIES 2

#define VIA_EEPROM_LAYOUT_OPTIONS_DEFAULT                                                    \
  (LAYOUT_OPTION_SPLIT_BS + LAYOUT_OPTION_SPLIT_RSHIFT_1_75U_1U + LAYOUT_OPTION_ENTER_ANSI + \
   LAYOUT_OPTION_BOTTOM_ROW_7U)

/*  calibration data for each specfic keybopard */
// clang-format off
#define EC_BOTTOMING_READING_DEFAULT_USER { \
  {0x0225,0x027c,0x027a,0x030a,0x0295,0x0270,0x0282,0x0247,0x0248,0x0245,0x0273,0x0269,0x02a9,0x019d,0x022c}, \
  {0x02a0,0x02c2,0x02f0,0x0345,0x02ce,0x02f9,0x0295,0x0275,0x032b,0x02e5,0x029e,0x02c0,0x0257,0x01e1,0x03ff}, \
  {0x01ca,0x02fb,0x0307,0x0321,0x02c9,0x030a,0x02df,0x0300,0x031b,0x02b8,0x02dc,0x02fa,0x03ff,0x02a0,0x03ff}, \
  {0x01c2,0x03ff,0x027f,0x0308,0x033b,0x02be,0x02b4,0x031a,0x0361,0x02c4,0x031c,0x02a2,0x03ff,0x01ad,0x0220}, \
  {0x01f9,0x01ed,0x01e7,0x03ff,0x03ff,0x03ff,0x020a,0x03ff,0x03ff,0x03ff,0x01af,0x01bc,0x01f2,0x03ff,0x03ff} \
}
// clang-format on

//
// TODO optimize for purpose
// EC(0) alphas
// EC(1) none-alphas
// EC(2) mods bottom row
// EC(3) little finger home Ctrl/hhkb fn
// EC(4) enter
// EC(5) spacebar
// EC(6) prevent misstouch
// clang-format off
#define EC_PRESETS_DEFAULT_USER {               \
  [0] = { \
    .actuation_mode = EC_ACTUATION_MODE_DYNAMIC, \
    .actuation_threshold = EC_ACTUATION_THRESHOLD_DEFAULT, \
    .actuation_travel =  128, \
    .release_mode = EC_RELEASE_MODE_DYNAMIC, \
    .release_threshold = EC_RELEASE_THRESHOLD_DEFAULT, \
    .release_travel = 128, \
    .deadzone = 112 \
  }, \
  [1] = { \
    .actuation_mode = EC_ACTUATION_MODE_STATIC, \
    .actuation_threshold = EC_ACTUATION_THRESHOLD_DEFAULT, \
    .actuation_travel =  EC_ACTUATION_TRAVEL_DEFAULT, \
    .release_mode = EC_RELEASE_MODE_STATIC, \
    .release_threshold = EC_RELEASE_THRESHOLD_DEFAULT, \
    .release_travel = EC_RELEASE_TRAVEL_DEFAULT, \
    .deadzone = EC_DEADZONE_DEFAULT \
  }, \
  [2] = { \
    .actuation_mode = EC_ACTUATION_MODE_STATIC, \
    .actuation_threshold = EC_ACTUATION_THRESHOLD_DEFAULT, \
    .actuation_travel =  EC_ACTUATION_TRAVEL_DEFAULT, \
    .release_mode = EC_RELEASE_MODE_STATIC, \
    .release_threshold = EC_RELEASE_THRESHOLD_DEFAULT, \
    .release_travel = EC_RELEASE_TRAVEL_DEFAULT, \
    .deadzone = EC_DEADZONE_DEFAULT \
  }, \
  [3] = { \
    .actuation_mode = EC_ACTUATION_MODE_DYNAMIC, \
    .actuation_threshold = EC_ACTUATION_THRESHOLD_DEFAULT, \
    .actuation_travel =  EC_ACTUATION_TRAVEL_DEFAULT, \
    .release_mode = EC_RELEASE_MODE_DYNAMIC, \
    .release_threshold = EC_RELEASE_THRESHOLD_DEFAULT, \
    .release_travel = EC_RELEASE_TRAVEL_DEFAULT, \
    .deadzone = EC_DEADZONE_DEFAULT \
  }, \
  [4] = { \
    .actuation_mode = EC_ACTUATION_MODE_STATIC, \
    .actuation_threshold = EC_ACTUATION_THRESHOLD_DEFAULT, \
    .actuation_travel =  EC_ACTUATION_TRAVEL_DEFAULT, \
    .release_mode = EC_RELEASE_MODE_STATIC, \
    .release_threshold = EC_RELEASE_THRESHOLD_DEFAULT, \
    .release_travel = EC_RELEASE_TRAVEL_DEFAULT, \
    .deadzone = EC_DEADZONE_DEFAULT \
  }, \
  [5] = { \
    .actuation_mode = EC_ACTUATION_MODE_STATIC, \
    .actuation_threshold = EC_ACTUATION_THRESHOLD_DEFAULT, \
    .actuation_travel =  EC_ACTUATION_TRAVEL_DEFAULT, \
    .release_mode = EC_RELEASE_MODE_STATIC, \
    .release_threshold = EC_RELEASE_THRESHOLD_DEFAULT, \
    .release_travel = EC_RELEASE_TRAVEL_DEFAULT, \
    .deadzone = EC_DEADZONE_DEFAULT \
  }, \
  [6] = { \
    .actuation_mode = EC_ACTUATION_MODE_STATIC, \
    .actuation_threshold = EC_ACTUATION_THRESHOLD_DEFAULT + 160, \
    .actuation_travel =  EC_ACTUATION_TRAVEL_DEFAULT, \
    .release_mode = EC_RELEASE_MODE_STATIC, \
    .release_threshold = EC_RELEASE_THRESHOLD_DEFAULT, \
    .release_travel = EC_RELEASE_TRAVEL_DEFAULT, \
    .deadzone = EC_DEADZONE_DEFAULT \
  }, \
  [7] = { \
    .actuation_mode = EC_ACTUATION_MODE_STATIC, \
    .actuation_threshold = EC_ACTUATION_THRESHOLD_DEFAULT, \
    .actuation_travel =  EC_ACTUATION_TRAVEL_DEFAULT, \
    .release_mode = EC_RELEASE_MODE_STATIC, \
    .release_threshold = EC_RELEASE_THRESHOLD_DEFAULT, \
    .release_travel = EC_RELEASE_TRAVEL_DEFAULT, \
    .deadzone = EC_DEADZONE_DEFAULT \
  } \
}
// clang-format on
