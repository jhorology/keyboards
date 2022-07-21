#pragma once

#include "lib/apple_fn.h"
#ifdef VIAL_ENABLED
#include "lib/combo_actions.h"
#include "lib/key_override_actions.h"
#endif
#include "lib/tap_dance_actions.h"
#include "quantum.h"

#define XXX KC_NO

// clang-format off
#define LAYOUT_60_hhkb( \
  K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D, K2C, \
  K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1D, \
  K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B,      K2D, \
  K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B,      K3D, \
       K41, K42,                K45,                K4A, K4B \
) { \
  { K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D }, \
  { K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1D }, \
  { K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C, K2D }, \
  { K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B, XXX, K3D }, \
  { XXX, K41, K42, XXX, XXX, K45, XXX, XXX, XXX, XXX, K4A, K4B, XXX, XXX } \
}

#define LAYOUT_60_tsangan(                                              \
  K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D, K2C, \
  K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1D, \
  K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B,      K2D, \
  K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B,      K3D, \
  K40, K41, K42,                K45,                K4A, K4B,      K4D \
) { \
  { K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D }, \
  { K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1D }, \
  { K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C, K2D }, \
  { K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B, XXX, K3D }, \
  { K40, K41, K42, XXX, XXX, K45, XXX, XXX, XXX, XXX, K4A, K4B, XXX, K4D } \
}
// clang-format on
