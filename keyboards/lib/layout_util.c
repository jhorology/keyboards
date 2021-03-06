/* Copyright 2022 Msafumi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

#include "layout_util.h"

#include "keymap_extras/keymap_japanese.h"

//   local prototypes
//------------------------------------------

bool process_layout_conversion(layout_conversion_item_t *table, uint16_t table_length, uint16_t keycode,
                               keyrecord_t *record);

//   local variables
//------------------------------------------

static layout_conversion_item_t ansi_on_apple_jis_table[] = {
    // src, dest, dest on shift
    {KC_GRV, JP_GRV, JP_TILD},    // "`", "~"
    {KC_2, 0, JP_AT},             // "@"
    {KC_6, 0, JP_CIRC},           // "^"
    {KC_7, 0, JP_AMPR},           // "&"
    {KC_8, 0, JP_ASTR},           // "*"
    {KC_9, 0, JP_LPRN},           // "("
    {KC_0, 0, JP_RPRN},           // ")"
    {KC_MINS, 0, JP_UNDS},        // "_"
    {KC_EQL, JP_EQL, JP_PLUS},    // "=", "+"
    {KC_LBRC, JP_LBRC, JP_LCBR},  // "[", "{"
    {KC_RBRC, JP_RBRC, JP_RCBR},  // "]", "}"

    {KC_BSLS, LALT(JP_YEN), JP_PIPE},  // "\", "|"
                                       // TODO
                                       // some applications (eg.emacs) interprets backslash as Alt + JP_YEN
                                       // see https://qiita.com/hirokisince1998/items/029741559d7ba7078523
                                       //
                                       // for emacs init.el:
                                       //
                                       // (define-key global-map [?\M-??] [?\\])
                                       // (define-key global-map [?\C-\M-??] [?\C-\\])
                                       // (defun isearch-add-backslash()
                                       // (interactive)
                                       // (isearch-printing-char ?\\ 1))
                                       // (define-key isearch-mode-map [?\M-??] 'isearch-add-backslash)
                                       //
    // {KC_CAPS, JP_CAPS, JP_EISU},  // CAPSLOCK
    {KC_SCLN, 0, JP_COLN},       // :
    {KC_QUOT, JP_QUOT, JP_DQUO}  // '
};

static uint16_t ansi_jis_override_key_flags;
static uint16_t ansi_jis_override_shift_flags;

// globl functions
//------------------------------------------

bool process_ansi_layout_on_apple_jis(uint16_t keycode, keyrecord_t *record) {
  return process_layout_conversion(&ansi_on_apple_jis_table[0],
                                   sizeof(ansi_on_apple_jis_table) / sizeof(layout_conversion_item_t), keycode, record);
}

// local functions
//------------------------------------------

bool process_layout_conversion(layout_conversion_item_t *table, uint16_t table_length, uint16_t keycode,
                               keyrecord_t *record) {
  uint16_t flag;
  layout_conversion_item_t *item = 0;
  for (uint16_t i; i < table_length; i++) {
    if (keycode == table[i].src) {
      flag = 1 << i;
      item = &table[i];
      break;
    }
  }
  if (!item) return true;

  if (record->event.pressed) {
    bool l_shift = keyboard_report->mods & MOD_BIT(KC_LSFT);
    bool r_shift = keyboard_report->mods & MOD_BIT(KC_RSFT);
    bool shift = l_shift || r_shift;
    bool alt = keyboard_report->mods & (MOD_BIT(KC_LALT) | MOD_BIT(KC_RALT));
    uint16_t dest_kc = shift ? item->dest_on_shift : item->dest;
    if (dest_kc != 0) {
      if (dest_kc & QK_LSFT) {
        if (!shift) register_code(KC_LSFT);
        register_code(dest_kc & 0xff);
        if (!shift) unregister_code(KC_LSFT);
      } else if (dest_kc & QK_LALT) {
        if (!alt) register_code(KC_LALT);
        register_code(dest_kc & 0xff);
        if (!alt) unregister_code(KC_LALT);
      } else {
        if (l_shift) unregister_code(KC_LSFT);
        if (r_shift) unregister_code(KC_RSFT);
        register_code(dest_kc & 0xff);
        if (l_shift) register_code(KC_LSFT);
        if (r_shift) register_code(KC_RSFT);
      }
      ansi_jis_override_key_flags |= flag;
      if (shift) {
        ansi_jis_override_shift_flags |= flag;
      } else {
        ansi_jis_override_shift_flags &= ~flag;
      }
      return false;
    }
  } else {
    if (ansi_jis_override_key_flags & flag) {
      unregister_code(((ansi_jis_override_shift_flags & flag) ? item->dest_on_shift : item->dest) & 0xff);
      ansi_jis_override_key_flags &= ~flag;
      return false;
    }
  }
  return true;
}
