#include QMK_KEYBOARD_H

// tap dance
// [single tap, single hold, multi tap, tap hold, tapping term]
const tap_dance_entry_t PROGMEM tap_dance_predefined_entries[NUM_TAP_DANCE_PREDEFINED_ENTRIES] = {
  // TD(0) - Left Alt, Alt + Apple fn
  [0] = {KC_LALT, KC_LALT, KC_LALT, APPLE_FF, TAPPING_TERM},
  // TD(1) - LCMD + Spacemacs leader key
  [1] = {A(KC_M), KC_LGUI, KC_LGUI, KC_LGUI, TAPPING_TERM},
  // TD(2) - RCMD + LANG2/LANG1
  [2] = {KC_LNG2, KC_RGUI, KC_LNG1, KC_RGUI, TAPPING_TERM},
  // TD(3) - Right Alt, Alt + layer switch
  [3] = {KC_RALT, KC_RALT, KC_RALT, MO(3), TAPPING_TERM}};

// since QMK 0.26, tap_dance_actions must be defined in keyma.c
tap_dance_action_t tap_dance_actions[TAP_DANCE_ENTRIES];

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // mac base layer
  [0] = LAYOUT_ansi_hhkb(
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSLS, KC_ESC,
    KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSPC,
    KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,
    KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,          MO(2),
             TD(0),   TD(1),   KC_SPC,                                                                 TD(2),   TD(3)
  ),
  // standard base layer
  [1] = LAYOUT_ansi_hhkb(
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSLS, KC_ESC,
    KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSPC,
    KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,
    KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,          MO(2),
             TD(0),   TD(1),   KC_SPC,                                                                 TD(2),   TD(3)
  ),
  // HHKB-like fn layer
  // + shift key for RGB decrement functions
  [2] = LAYOUT_ansi_hhkb(
    TG(3),   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_INS,  KC_DEL,
    KC_CAPS, KC_F16,  KC_F17,  KC_F18,  _______, _______, _______, _______, KC_PSCR, KC_SCRL, KC_PAUS, KC_UP,   _______, _______,
    _______, RGB_TOG, RGB_VAI, RGB_HUI, _______, _______, KC_PAST, KC_PSLS, KC_HOME, KC_PGUP, KC_LEFT, KC_RGHT,          KC_PENT,
    _______, RGB_MOD, RGB_SPI, RGB_SAI, _______, _______, KC_PPLS, KC_PMNS, KC_END,  KC_PGDN, KC_DOWN, _______,          _______,
             _______, _______,                            _______,                            _______, _______
  ),
  // settings, application-specific keys
  // row 3: normal settings, +shift key for unusual reversed setting
  [3] = LAYOUT_ansi_hhkb(
    QK_BOOT, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, TERM_LCK,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
    _______, RHID_OFF,AUT_ON,  _______, USJ_OFF, NK_ON,   CL_NORM, AG_NORM, BS_NORM, _______, _______, _______,          _______,
             _______, _______,                            _______,                                     _______, _______
  )
};
// clang-format on
