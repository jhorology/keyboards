#define NUM_TAP_DANCE_PREDEFINED_ENTRIES 2

#define VIA_EEPROM_LAYOUT_OPTIONS_DEFAULT                                                    \
  (LAYOUT_OPTION_SPLIT_BS + LAYOUT_OPTION_SPLIT_RSHIFT_1_75U_1U + LAYOUT_OPTION_ENTER_ANSI + \
   LAYOUT_OPTION_BOTTOM_ROW_7U)

/*  calibration data for each specfic keybopard */
// clang-format off
#define EC_BOTTOMING_READING_DEFAULT_USER { \
  {0x020e,0x02aa,0x0296,0x02f3,0x02aa,0x0281,0x028c,0x0271,0x0292,0x0208,0x0253,0x0257,0x02aa,0x01b0,0x025a}, \
  {0x02af,0x02bd,0x02f6,0x032b,0x02ad,0x0317,0x02ac,0x0282,0x032c,0x02f0,0x0298,0x02c4,0x021a,0x01e5,0x03ff}, \
  {0x01bb,0x0323,0x0320,0x0321,0x028d,0x02c3,0x02b9,0x02b3,0x0311,0x02d5,0x02b9,0x02d6,0x03ff,0x0294,0x03ff}, \
  {0x01ec,0x03ff,0x0266,0x0301,0x032d,0x02e0,0x02a2,0x0330,0x0374,0x02c0,0x0320,0x0295,0x03ff,0x018e,0x0204}, \
  {0x021e,0x0225,0x0214,0x03ff,0x03ff,0x03ff,0x0206,0x03ff,0x03ff,0x03ff,0x01a1,0x01c5,0x0209,0x03ff,0x03ff}  \
}
// clang-format on

// clang-format off
#define EC_PRESETS_DEFAULT_USER { \
  [0] = { \
    .actuation_mode = EC_ACTUATION_MODE_STATIC, \
    .actuation_threshold = EC_ACTUATION_THRESHOLD_DEFAULT, \
    .actuation_travel =  EC_ACTUATION_TRAVEL_DEFAULT, \
    .release_mode = EC_RELEASE_MODE_STATIC, \
    .release_threshold = EC_RELEASE_THRESHOLD_DEFAULT, \
    .release_travel = EC_RELEASE_TRAVEL_DEFAULT, \
    .deadzone = EC_DEADZONE_DEFAULT \
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
  } \
}
// clang-format on
