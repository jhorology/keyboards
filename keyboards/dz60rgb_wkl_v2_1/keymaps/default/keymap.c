#include QMK_KEYBOARD_H

#define LALT_IME TD(TD_LALT_IME)   // LALT + ` on double tap, KC_LALT in other case.
#define CMD_EISU TD(TD_LGUI_EISU)  // KC_LNG2(英数) on double tap, KC_LGUI in other case.
#define CMD_KANA TD(TD_RGUI_KANA)  // KC_LNG1(かな) on double tap, KC_LGUI in other case.
#define CMD_TGEJ \
  TD(TD_LGUI_EISU_KANA)  // Toogle send KC_LNG1(かな) and KC_LNG2(英数)
                         // on double tap, KC_LGUI in other case.

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // mac base layer
  [0] = LAYOUT_60_hhkb(
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSLS, KC_ESC,
    KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSPC,
    KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,
    KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,          MO(2),
             KC_LALT, CMD_TGEJ,                           KC_SPC,                             KC_RGUI, APPLE_FF
  ),
  // standard base layer
  [1] = LAYOUT_60_hhkb(
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSLS, KC_ESC,
    KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSPC,
    KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,
    KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,          MO(2),
             KC_LALT, KC_LGUI,                            KC_SPC,                             KC_RGUI, APPLE_FF
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
