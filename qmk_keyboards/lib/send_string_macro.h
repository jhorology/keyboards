#pragma once

#define SEND_NIBBLE(value) send_nibble(value)
#define SEND_BOOL(value) send_string(value ? "true" : "false")
#define SEND_BYTE(value) \
  send_string("0x");     \
  send_byte(value)

#define SEND_WORD(value) \
  send_string("0x");     \
  send_word(value)

#define SEND_SEP(sep)        \
  if (sep) send_string(","); \
  send_string("\n")

#define SEND_C_PROP_NAME(name) \
  send_string(".");            \
  send_string(name);           \
  send_string(" = ");

#define SEND_C_PROP_VALUE(value, type, sep) \
  SEND_C_PROP_NAME(#value);                 \
  SEND_##type(value);                       \
  SEND_SEP(sep)

#define SEND_C_ARROW_PROP_VALUE(obj, prop, type, sep) \
  SEND_C_PROP_NAME(#prop);                            \
  SEND_##type(obj->prop);                             \
  SEND_SEP(sep)

#define SEND_C_ENUM_PROP_VALUE(prop, value, sep) \
  SEND_C_PROP_NAME(#prop);                       \
  send_string(#value);                           \
  SEND_SEP(sep)

#define SEND_C_DOT_PROP_VALUE(obj, prop, type, sep) \
  SEND_C_PROP_NAME(#prop);                          \
  SEND_##type(obj.prop);                            \
  SEND_SEP(sep)

#define SEND_C_MATRIX_LOOP(...)                       \
  wait_ms(50);                                        \
  send_string("{\n");                                 \
  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {   \
    send_string("{");                                 \
    for (uint8_t col = 0; col < MATRIX_COLS; col++) { \
      __VA_ARGS__                                     \
      if (col < (MATRIX_COLS - 1)) {                  \
        send_string(",");                             \
      }                                               \
    }                                                 \
    send_string("}");                                 \
    if (row < (MATRIX_ROWS - 1)) {                    \
      send_string(",");                               \
    }                                                 \
    send_string(" \n");                               \
    wait_ms(50);                                      \
  }                                                   \
  send_string("}");

#define SEND_JS_PROP_NAME(name) \
  send_string(name);            \
  send_string(": ");

#define SEND_JS_PROP_VALUE(value, type, sep) \
  SEND_JS_PROP_NAME(#value);                 \
  SEND_##type(value);                        \
  SEND_SEP(sep)

#define SEND_JS_ARROW_PROP_VALUE(obj, prop, type, sep) \
  SEND_JS_PROP_NAME(#prop);                            \
  SEND_##type(obj->prop);                              \
  SEND_SEP(sep)

#define SEND_JS_DOT_PROP_VALUE(obj, prop, type, sep) \
  SEND_JS_PROP_NAME(#prop);                          \
  SEND_##type(obj.prop);                             \
  SEND_SEP(sep)

#define SEND_JS_MATRIX_LOOP(...)                      \
  wait_ms(50);                                        \
  send_string("[\n");                                 \
  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {   \
    send_string("[");                                 \
    for (uint8_t col = 0; col < MATRIX_COLS; col++) { \
      __VA_ARGS__                                     \
      if (col < (MATRIX_COLS - 1)) {                  \
        send_string(",");                             \
      }                                               \
    }                                                 \
    send_string("]");                                 \
    if (row < (MATRIX_ROWS - 1)) {                    \
      send_string(",");                               \
    }                                                 \
    send_string(" \n");                               \
    wait_ms(50);                                      \
  }                                                   \
  send_string("]");
