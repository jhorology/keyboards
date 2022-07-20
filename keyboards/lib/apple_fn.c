#include "apple_fn.h"

#ifdef APPLE_FN_ENABLE
/*
  bit0: apple_fn pressed state
  bi-12: 1-0, minus, equla keyes remapped state
 */
static uint16_t force_fkeys;

__attribute__((weak)) bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case APPLE_FN:
      process_apple_fn(keycode, record);
      return false;
    case APPLE_FF:
      process_apple_ff(keycode, record);
      return false;
    case KC_1 ... KC_0:
      return process_apple_ff_fkey(keycode - KC_1, record);
    case KC_MINS:
      return process_apple_ff_fkey(10, record);
    case KC_EQL:
      return process_apple_ff_fkey(11, record);
  }
#ifdef APPLE_FN_FORCE_FKEY

#endif
  return true;
}

void process_apple_fn(uint16_t keycode, keyrecord_t *record) {
  if (record->event.pressed) {
    register_code(KC_APFN);
  } else {
    unregister_code(KC_APFN);
  }
}
void process_apple_ff(uint16_t keycode, keyrecord_t *record) {
  if (record->event.pressed) {
    force_fkeys |= 1;
    register_code(KC_APFN);
  } else {
    force_fkeys &= 0xfffe;
    unregister_code(KC_APFN);
  }
}
bool process_apple_ff_fkey(uint16_t fkey_index, keyrecord_t *record) {
  uint16_t mask = 1 << (fkey_index + 1);
  if (record->event.pressed) {
    if (force_fkeys & 1) {
      force_fkeys |= mask;
      register_code(KC_F1 + fkey_index);
      return false;
    }
  } else {
    if (force_fkeys & mask) {
      force_fkeys &= ~mask;
      unregister_code(KC_F1 + fkey_index);
      return false;
    }
  }
  return true;
}
#endif
