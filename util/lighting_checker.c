#include "config.h"
#include "rgb_matrix.h"
enum rgb_matrix_effects {
  RGB_MATRIX_NONE = 0,

// --------------------------------------
// -----Begin rgb effect enum macros-----
#define RGB_MATRIX_EFFECT(name, ...) RGB_MATRIX_##name,
#include "rgb_matrix_effects.inc"
#undef RGB_MATRIX_EFFECT

#if defined(RGB_MATRIX_CUSTOM_KB) || defined(RGB_MATRIX_CUSTOM_USER)
#  define RGB_MATRIX_EFFECT(name, ...) RGB_MATRIX_CUSTOM_##name,
#  ifdef RGB_MATRIX_CUSTOM_KB
#    include "rgb_matrix_kb.inc"
#  endif
#  ifdef RGB_MATRIX_CUSTOM_USER
#    include "rgb_matrix_user.inc"
#  endif
#  undef RGB_MATRIX_EFFECT
#endif
  // --------------------------------------
  // -----End rgb effect enum macros-------

  RGB_MATRIX_EFFECT_MAX
};
