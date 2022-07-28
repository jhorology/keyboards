#include QMK_KEYBOARD_H

#define MO2_TG3 TD(TD_MO2_TG3)        // MO(2), on double tap: TG(3)
#define CMD_EJ TD(TD_LCMD_EISU_KANA)  // for mac/HHKB, Left Cmd, on tap: 英数, on double tap かな
#define OPT_APFF TD(TD_LOPT_APFF)     // for mac/HHKB, Left Option, on tap hold: Apple fn/globe + FK overrde
#define WIN_LANG TD(TD_LWIN_LANG)     // for win/HHKB, Left Win, on double tap: Win + space
#define ALT_EJ TD(TD_LALT_EISU_KANA)  // for win/HHKB, Left Alt, on tap: 英数, on double tap かな

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // mac base layer
  [0] = LAYOUT_60_hhkb(
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSLS, KC_ESC,
    KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSPC,
    KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,
    KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,          MO2_TG3,
             OPT_APFF,CMD_EJ,                             KC_SPC,                             KC_RGUI, KC_RALT
  ),
  // standard base layer
  [1] = LAYOUT_60_hhkb(
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSLS, KC_ESC,
    KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSPC,
    KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,
    KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,          MO2_TG3,
             ALT_EJ,  WIN_LANG,                           KC_SPC,                             KC_RGUI, KC_RALT
  ),
  // HHKB-like fn layer
  [2] = LAYOUT_60_hhkb(
    TG(3),   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_INS,  KC_DEL,
    KC_CAPS, _______, _______, _______, _______, _______, _______, _______, KC_PSCR, KC_SLCK, KC_PAUS, KC_UP,   _______, _______,
    _______, RGB_TOG, RGB_VAI, RGB_SPI, RGB_HUI, RGB_SAI, KC_PAST, KC_PSLS, KC_HOME, KC_PGUP, KC_LEFT, KC_RGHT,          KC_PENT,
    _______, RGB_MOD, RGB_VAD, RGB_SPD, RGB_HUD, RGB_SAD, KC_PPLS, KC_PMNS, KC_END,  KC_PGDN, KC_DOWN, _______,          _______,
             _______, _______,                            _______,                            _______, _______
  ),
  // settings, application-specific keys
  [3] = LAYOUT_60_hhkb(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,  QK_BOOT,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    CL_TOGG, MAC_ON,  USJ_OFF, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
    _______, MAC_OFF, USJ_ON,  _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
             _______, AG_TOGG,                            _______,                                     _______, _______
  )
};
// clang-format on
