#pragma once

#define __DEBRACKET(...) __VA_ARGS__

#define SEND_NIBBLE(value) send_nibble(value)
#define SEND_BOOL(value) send_string(value ? "true" : "false")
#define SEND_BYTE(value) \
  send_string("0x");     \
  send_byte(value)

#define SEND_WORD(value) \
  send_string("0x");     \
  send_word(value)

#define SEND_SEP(sep) \
  if (sep) send_string(",");

#define SEND_BREAK(sep)      \
  if (sep) send_string(","); \
  send_string("\n")

#define SEND_C_PROP_NAME(name) \
  send_string(".");            \
  send_string(name);           \
  send_string(" = ");

#define SEND_C_PROP_VALUE(value, type, sep) \
  SEND_C_PROP_NAME(#value);                 \
  SEND_##type(value);                       \
  SEND_BREAK(sep)

#define SEND_C_ARROW_PROP_VALUE(obj, prop, type, sep) \
  SEND_C_PROP_NAME(#prop);                            \
  SEND_##type(obj->prop);                             \
  SEND_BREAK(sep)

#define SEND_C_ENUM_PROP_VALUE(prop, value, sep) \
  SEND_C_PROP_NAME(#prop);                       \
  send_string(#value);                           \
  SEND_BREAK(sep)

#define SEND_C_DOT_PROP_VALUE(obj, prop, type, sep) \
  SEND_C_PROP_NAME(#prop);                          \
  SEND_##type(obj.prop);                            \
  SEND_BREAK(sep)

// code need bracket (...)
#define SEND_C_ARRAY_CODE(name, start, endExclusive, sep, code) \
  {                                                             \
    send_string(name);                                          \
    send_string(" = {");                                        \
    for (uint8_t i = start; i < (endExclusive); i++) {          \
      __DEBRACKET code SEND_SEP(row < (endExclusive - 1));      \
    }                                                           \
    send_string("};");                                          \
    SEND_BREAK(sep);                                            \
  }

#define SEND_C_OBJECT_ARRAY_CODE(name, start, endExclusive, code) \
  {                                                               \
    send_string(name);                                            \
    send_string(" = {\n");                                        \
    for (uint8_t i = start; i < (endExclusive); i++) {            \
      send_string("[");                                           \
      if (i >= 10) {                                              \
        send_char('0' + i / 10);                                  \
      }                                                           \
      send_char('0' + i % 10);                                    \
      send_string("] = {\n");                                     \
      __DEBRACKET code send_string("}");                          \
      SEND_BREAK(i < ((endExclusive)-1));                         \
    }                                                             \
    send_string("};\n");                                          \
  }

#define SEND_C_MATRIX_ARRAY_CODE(name, code)                \
  {                                                         \
    send_string(name);                                      \
    send_string(" = ");                                     \
    wait_ms(50);                                            \
    send_string("{\n");                                     \
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {       \
      send_string("{");                                     \
      for (uint8_t col = 0; col < MATRIX_COLS; col++) {     \
        __DEBRACKET code SEND_SEP(col < (MATRIX_COLS - 1)); \
      }                                                     \
      send_string("}");                                     \
      SEND_BREAK(row < (MATRIX_ROWS - 1));                  \
      wait_ms(50);                                          \
    }                                                       \
    send_string("};\n");                                    \
  }

#define SEND_JS_PROP_NAME(name) \
  send_string(name);            \
  send_string(": ");

#define SEND_JS_PROP_VALUE(value, type, sep) \
  SEND_JS_PROP_NAME(#value);                 \
  SEND_##type(value);                        \
  SEND_BREAK(sep)

#define SEND_JS_ARROW_PROP_VALUE(obj, prop, type, sep) \
  SEND_JS_PROP_NAME(#prop);                            \
  SEND_##type(obj->prop);                              \
  SEND_BREAK(sep)

#define SEND_JS_DOT_PROP_VALUE(obj, prop, type, sep) \
  SEND_JS_PROP_NAME(#prop);                          \
  SEND_##type(obj.prop);                             \
  SEND_BREAK(sep)

// code fn(i) need bracket (...)
#define SEND_JS_PROP_ARRAY_CODE(name, start, endExclusive, sep, code) \
  {                                                                   \
    SEND_JS_PROP_NAME(name);                                          \
    send_string("[");                                                 \
    for (uint8_t i = start; i < (endExclusive); i++) {                \
      __DEBRACKET code SEND_SEP(i < ((endExclusive)-1));              \
    }                                                                 \
    send_string("]");                                                 \
    SEND_BREAK(sep);                                                  \
  }

#define SEND_JS_PROP_ARRAY(array, type, start, endExclusive, sep) \
  SEND_JS_PROP_ARRAY_CODE(#array, start, endExclusive, sep, (SEND_##type(array[i]);))

#define SEND_JS_PROP_MATRIX_ARRAY_CODE(name, sep, code)     \
  {                                                         \
    SEND_JS_PROP_NAME(name);                                \
    wait_ms(50);                                            \
    send_string("[\n");                                     \
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {       \
      send_string("[");                                     \
      for (uint8_t col = 0; col < MATRIX_COLS; col++) {     \
        __DEBRACKET code SEND_SEP(col < (MATRIX_COLS - 1)); \
      }                                                     \
      send_string("]");                                     \
      SEND_BREAK(row < (MATRIX_ROWS - 1));                  \
      wait_ms(50);                                          \
    }                                                       \
    send_string("]");                                       \
    SEND_BREAK(sep);                                        \
  }
