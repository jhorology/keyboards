#include "apple_fn.h"

#ifdef APPLE_FN_ENABLE
__attribute__((weak)) bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case APPLE_FN:
            process_apple_fn(keycode, record);
            break;
    }
    return true;
}

void process_apple_fn(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        register_code(KC_APFN);
    } else {
        unregister_code(KC_APFN);
    }
}
#endif
