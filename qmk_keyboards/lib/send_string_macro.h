#pragma once

#include <quantum.h>

#define __DEBRACKET(...) __VA_ARGS__

// max
inline void send_dec(uint32_t v, uint8_t d) {
  char buf[d];
  if (v < 10) {
    send_char('0' + v);
    return;
  }
  for (; v && d; v /= 10, d--) {
    buf[d - 1] = '0' + (v % 10);
  }
  send_string(d ? &buf[d] : buf);
}

#define SEND_DEC1(value) send_dec(value, 1)
#define SEND_DEC2(value) send_dec(value, 2)
#define SEND_DEC3(value) send_dec(value, 3)
#define SEND_DEC4(value) send_dec(value, 4)
#define SEND_DEC5(value) send_dec(value, 5)
#define SEND_NIBBLE(value) send_nibble(value)
#define SEND_BOOL(value) send_string(value ? "true" : "false")
#define SEND_BYTE(value) \
  {                      \
    send_string("0x");   \
    send_byte(value);    \
  }
#define SEND_WORD(value) \
  {                      \
    send_string("0x");   \
    send_word(value);    \
  }

#define SEND_SEP(i, size) \
  if (i < (size - 1)) send_char(',');

// SEND_STRING is defined in send_string.h
// #define SEND_STRING(value) send_string(value)

// code F(i) need bracket (...)
#define SEND_ARRAY_CODE(size, term, l_bracket, r_bracket, code) \
  {                                                             \
    send_char(l_bracket);                                       \
    for (uint8_t i = 0; i < (size); i++) {                      \
      __DEBRACKET code SEND_SEP(i, size);                       \
    }                                                           \
    send_char(r_bracket);                                       \
    if ((term) != NULL) send_string(term);                      \
    wait_ms(3);                                                 \
  }

#define SEND_ARRAY_CODE_V(size, term, l_bracket, r_bracket, code) \
  {                                                               \
    send_char(l_bracket);                                         \
    send_string("\n");                                            \
    for (uint8_t i = 0; i < (size); i++) {                        \
      __DEBRACKET code SEND_SEP(i, size);                         \
      send_string("\n");                                          \
    }                                                             \
    send_char(r_bracket);                                         \
    if ((term) != NULL) send_string(term);                        \
    wait_ms(3);                                                   \
  }

// code F(i,j) need bracket (...)
#define SEND_2D_ARRAY_CODE(i_size, j_size, term, l_bracket, r_bracket, code)        \
  SEND_ARRAY_CODE_V(i_size, term, l_bracket, r_bracket,                             \
                    (send_char(l_bracket); for (uint8_t j = 0; j < (j_size); j++) { \
                      __DEBRACKET code SEND_SEP(j, j_size);                         \
                    } send_char(r_bracket);                                         \
                     wait_ms(3);))

#define SEND_C_PROP_NAME(name) \
  {                            \
    send_string(".");          \
    send_string(name);         \
    send_string(" = ");        \
  }

#define SEND_C_NAME_PROP_VALUE_CODE(name, term, code)       \
  {                                                         \
    SEND_C_PROP_NAME(name);                                 \
    __DEBRACKET code if ((term) != NULL) send_string(term); \
  }

#define SEND_C_NAME_PROP_VALUE(name, value, type, term) \
  SEND_C_NAME_PROP_VALUE_CODE(name, term, (SEND_##type(value);))

#define SEND_C_SYMBOL_PROP_VALUE(symbol, value, type, term) \
  SEND_C_NAME_PROP_VALUE(#symbol, value, type, term)

#define SEND_C_SYMBOL_PROP_SYMBOL_VALUE(symbol, value, term) \
  SEND_C_NAME_PROP_VALUE_CODE(#symbol, term, (send_string(#value);))

#define SEND_C_PROP_VALUE(value, type, term) SEND_C_SYMBOL_PROP_VALUE(value, value, type, term)

#define SEND_C_PROP_ARROW_VALUE(obj, prop, type, term) \
  SEND_C_NAME_PROP_VALUE_CODE(#prop, term, (SEND_##type(obj->prop);))

#define SEND_C_PROP_DOT_VALUE(obj, prop, type, term) \
  SEND_C_NAME_PROP_VALUE_CODE(#prop, term, (SEND_##type(obj.prop);))

// code FN(i) need bracket (...)
#define SEND_C_ARRAY_CODE(size, term, code) SEND_ARRAY_CODE(size, term, '{', '}', code)
#define SEND_C_ARRAY_CODE_V(size, term, code) SEND_ARRAY_CODE_V(size, term, '{', '}', code)

// code FN(i) need bracket (...)
#define SEND_C_INDEXED_ARRAY_CODE(size, term, code) \
  SEND_C_ARRAY_CODE_V(                              \
    size, term,                                     \
    (send_char('['); SEND_DEC2(i); send_string("] = {\n"); __DEBRACKET code send_char('}');))

// code FN(i, j) need bracket (...)
#define SEND_C_2D_ARRAY_CODE(i_size, j_size, term, code) \
  SEND_2D_ARRAY_CODE(i_size, j_size, term, '{', '}', code)

#define SEND_C_2D_ARRAY(array, i_size, j_size, type, term) \
  SEND_C_2D_ARRAY_CODE(i_size, j_size, term, (SEND_##type(array[i][j]);))

#define SEND_JS_PROP_NAME(name) \
  {                             \
    send_string(name);          \
    send_string(": ");          \
  }

// code FN(i, j) need bracket (...)
#define SEND_JS_NAME_PROP_VALUE_CODE(name, term, code)      \
  {                                                         \
    SEND_JS_PROP_NAME(name);                                \
    __DEBRACKET code if ((term) != NULL) send_string(term); \
  }

#define SEND_JS_NAME_PROP_VALUE(name, value, type, term) \
  SEND_JS_NAME_PROP_VALUE_CODE(name, term, (SEND_##type(value);))

#define SEND_JS_SYMBOL_PROP_VALUE(symbol, value, type, term) \
  SEND_JS_NAME_PROP_VALUE(#symbol, value, type, term)

#define SEND_JS_PROP_VALUE(value, type, term) SEND_JS_SYMBOL_PROP_VALUE(value, value, type, term)

#define SEND_JS_PROP_ARROW_VALUE(obj, prop, type, term) \
  SEND_JS_NAME_PROP_VALUE_CODE(#prop, term, (SEND_##type(obj->prop);))

#define SEND_JS_PROP_DOT_VALUE(obj, prop, type, term) \
  SEND_JS_NAME_PROP_VALUE_CODE(#prop, term, (SEND_##type(obj.prop);))

// code FN(i) need bracket (...)
#define SEND_JS_ARRAY_CODE(size, term, code) SEND_ARRAY_CODE(size, term, '[', ']', code)

// code FN(i) need bracket (...)
#define SEND_JS_ARRAY(array, size, type, term) \
  SEND_JS_ARRAY_CODE(size, term, (SEND_##type(array[i]);))

// code FN(i) need bracket (...)
#define SEND_JS_NAME_PROP_ARRAY_CODE(name, size, term, code) \
  {                                                          \
    SEND_JS_PROP_NAME(name);                                 \
    SEND_JS_ARRAY_CODE(size, term, code);                    \
  }

#define SEND_JS_NAME_PROP_ARRAY(name, array, size, type, term) \
  SEND_JS_NAME_PROP_ARRAY_CODE(name, size, term, (SEND_##type(array[i]);))

#define SEND_JS_PROP_ARRAY(array, size, type, term) \
  SEND_JS_NAME_PROP_ARRAYE(#array, array, size, type, term)

// code FN(i, j) need bracket (...)
#define SEND_JS_2D_ARRAY_CODE(i_size, j_size, term, code) \
  SEND_2D_ARRAY_CODE(i_size, j_size, term, '[', ']', code)

// code FN(i) need bracket (...)
#define SEND_JS_2D_ARRAY(array, i_size, j_size, type, term) \
  SEND_JS_2D_ARRAY_CODE(i_size, j_size, term, (SEND_##type(array[i][j]);))

// code FN(i) need bracket (...)
#define SEND_JS_NAME_PROP_2D_ARRAY_CODE(name, i_size, j_size, term, code) \
  {                                                                       \
    SEND_JS_PROP_NAME(name);                                              \
    SEND_JS_2D_ARRAY_CODE(i_size, j_size, term, code);                    \
  }

#define SEND_JS_NAME_PROP_2D_ARRAY(name, array, i_size, j_size, type, term) \
  SEND_JS_NAME_PROP_2D_ARRAY_CODE(name, i_size, j_size, term, (SEND_##type(array[i][j]);))

#define SEND_JS_PROP_2D_ARRAY(array, i_size, j_size, type, term) \
  SEND_JS_NAME_PROP_2D_ARRAY(#array, array, i_size, j_size, type, term)
