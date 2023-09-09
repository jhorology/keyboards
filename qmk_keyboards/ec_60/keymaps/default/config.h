#define NUM_TAP_DANCE_PREDEFINED_ENTRIES 2

#define VIA_EEPROM_LAYOUT_OPTIONS_DEFAULT                                                    \
  (LAYOUT_OPTION_SPLIT_BS + LAYOUT_OPTION_SPLIT_RSHIFT_1_75U_1U + LAYOUT_OPTION_ENTER_ANSI + \
   LAYOUT_OPTION_BOTTOM_ROW_7U)

/*  calibration data for each specfic keybopard */
// clang-format off
#define EC_BOTTOMING_READING_DEFAULT_USER { \
  {0x0205,0x023d,0x0242,0x02fa,0x027a,0x0277,0x022a,0x0206,0x024f,0x01d0,0x022d,0x0238,0x0267,0x018b,0x0216}, \
  {0x0249,0x02a0,0x02e4,0x0327,0x02a7,0x02d0,0x0275,0x024b,0x02eb,0x02a2,0x0259,0x0287,0x022c,0x01d4,0x03ff}, \
  {0x01a0,0x02d8,0x02df,0x0306,0x0294,0x02e2,0x029f,0x02ac,0x02e5,0x028b,0x02a0,0x02a8,0x03ff,0x0268,0x03ff}, \
  {0x019e,0x03ff,0x0250,0x02ea,0x0314,0x02a6,0x0260,0x02e0,0x0331,0x028c,0x02dc,0x028b,0x03ff,0x017a,0x01ff}, \
  {0x01c6,0x01f3,0x01b4,0x03ff,0x03ff,0x03ff,0x026a,0x03ff,0x03ff,0x03ff,0x017a,0x017a,0x01a8,0x03ff,0x03ff} \
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
    .actuation_mode = EC_ACTUATION_MODE_STATIC, \
    .actuation_threshold = EC_ACTUATION_THRESHOLD_DEFAULT, \
    .actuation_travel =  EC_ACTUATION_TRAVEL_DEFAULT, \
    .release_mode = EC_RELEASE_MODE_STATIC, \
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
