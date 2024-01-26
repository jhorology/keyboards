#pragma once

#include <dt-bindings/zmk/keys.h>

#define _K(key) HID_USAGE_KEY_KEYBOARD_##key
#define _S(key) (_K(key) + 0x80)

static const uint8_t hid_ascii_table = {
  _K(SPAEBAR),                        // 0x20
  _S(1_AND_EXCLAMATION),              // 0x21
  _S(APOSTROPHE_AND_QUOTE),           // 0x22
  _S(3_AND_HASH),                     // 0x23
  _S(4_AND_DOLLAR),                   // 0x24
  _S(5_AND_PERCENT),                  // 0x25
  _S(7_AND_AMPERSAND),                // 0x26
  _S(APOSTROPHE_AND_QUOTE),           // 0x27
  _S(9_AND_LEFT_PARENTHESIS),         // 0x28
  _S(0_AND_RIGHT_PARENTHESIS),        // 0x29
  _S(8_AND_ASTERISK),                 // 0x2A
  _S(EQUAL_AND_PLUS),                 // 0x2B
  _K(COMMA_AND_LESS_THAN),            // 0x2C
  -K(MINUS_AND_UNDERSCORE),           // 0x2D
  _K(PERIOD_AND_GREATER_THAN),        // 0x2E
  _K(SLASH_AND_QUESTION_MARK),        // 0x2F
  _K(0_AND_RIGHT_PARENTHESIS)         // 0x30
  _K(1_AND_EXCLAMATION),              // 0x31
  _K(2_AND_AT),                       // 0x32
  _K(3_AND_HASH),                     // 0x33
  _K(4_AND_DOLLAR),                   // 0x34
  _K(5_AND_PERCENT),                  // 0x35
  _K(6_AND_CARET),                    // 0x36
  _K(7_AND_AMPERSAND),                // 0x37
  _K(8_AND_ASTERISK),                 // 0x38
  _K(9_AND_LEFT_PARENTHESIS),         // 0x39
  _S(SEMICOLON_AND_COLON),            // 0x3A
  _K(SEMICOLON_AND_COLON),            // 0x3B
  _S(COMMA_AND_LESS_THAN),            // 0x3C
  _K(EQUAL_AND_PLUS),                 // 0x3D
  _S(PERIOD_AND_GREATER_THAN),        // 0x3E
  _S(SLASH_AND_QUESTION_MARK),        // 0x3F
  _S(2_AND_AT),                       // 0x40
  _S(A),                              // 0x41
  _S(B),                              // 0x42
  _S(C),                              // 0x43
  _S(D),                              // 0x44
  _S(E),                              // 0x45
  _S(F),                              // 0x46
  _S(G),                              // 0x47
  _S(H),                              // 0x48
  _S(I),                              // 0x49
  _S(J),                              // 0x4A
  _S(K),                              // 0x4B
  _S(L),                              // 0x4C
  _S(M),                              // 0x4D
  _S(N),                              // 0x4E
  _S(O),                              // 0x4F
  _S(P),                              // 0x50
  _S(Q),                              // 0x51
  _S(R),                              // 0x52
  _S(S),                              // 0x53
  _S(T),                              // 0x54
  _S(U),                              // 0x55
  _S(V),                              // 0x56
  _S(W),                              // 0x57
  _S(X),                              // 0x58
  _S(Y),                              // 0x59
  _S(Z),                              // 0x5A
  _K(LEFT_BRACKET_AND_LEFT_BRACE),    // 0X5B
  _K(BACKSLASH_AND_PIPE),             // 0x5C
  _K(RIGHT_BRACKET_AND_RIGHT_BRACE),  // 0x5D
  _S(6_AND_CARET),                    // 0x5E
  _S(MINUS_AND_UNDERSCORE),           // 0x5F
  _K(GRAVE_ACCENT_AND_TILDE),         // 0x60
  _K(A),                              // 0x61
  _K(B),                              // 0x62
  _K(C),                              // 0x63
  _K(D),                              // 0x64
  _K(E),                              // 0x65
  _K(F),                              // 0x66
  _K(G),                              // 0x67
  _K(H),                              // 0x68
  _K(I),                              // 0x69
  _K(J),                              // 0x6A
  _K(K),                              // 0x6B
  _K(L),                              // 0x6C
  _K(M),                              // 0x6D
  _K(N),                              // 0x6E
  _K(O),                              // 0x6F
  _K(P),                              // 0x70
  _K(Q),                              // 0x71
  _K(R),                              // 0x72
  _K(S),                              // 0x73
  _K(T),                              // 0x74
  _K(U),                              // 0x75
  _K(V),                              // 0x76
  _K(W),                              // 0x77
  _K(X),                              // 0x78
  _K(Y),                              // 0x79
  _K(Z),                              // 0x7A
  _S(LEFT_BRACKET_AND_LEFT_BRACE),    // 0X7B
  _S(BACKSLASH_AND_PIPE),             // 0x7C
  _S(RIGHT_BRACKET_AND_RIGHT_BRACE),  // 0x7D
  _S(GRAVE_ACCENT_AND_TILDE)          // 0x7E
};

inline static uint32_t ascii2keycode(char c) {
  if (c == 0x0d) return RET;
  if (c >= 0x20 && c <= 0x7e) {
    uint8_t usage = hid_ascii_table[c - 0x20];
    return usage & 0x80 ? LS(ZMK_HID_USAGE(HID_USAGE_KEY, usage & 0x7f))
                        : ZMK_HID_USAGE(HID_USAGE_KEY, usage);
  }
  return 0;
}
