#pragma once

#include <dt-bindings/zmk/hid_usage.h>
#include <dt-bindings/zmk/hid_usage_pages.h>
#include <dt-bindings/zmk/modifiers.h>
#include <dt-bindings/zmk/keys.h>

/*
 * JIS keys
 */

/* 全角/半角 */
#define JIS_ZENKAKU_HANKAKU GRAVE

/* 2" number 2 and double quate */
#define JIS_N2_AND_DQT N2
#define JIS_DQT LS(JIS_N2_AND_DQT)

/* 6& number 6 and ampasand */
#define JIS_N6_AND_AMPS N6
#define JIS_AMPS LS(JIS_N6_AND_AMPS)

/* 7& number 7 and single quate */
#define JIS_N7_AND_SQT N7
#define JIS_SQT LS(JIS_N7_AND_SQT)

/* 8( number 8 and left parenthsis */
#define JIS_N8_AND_LPAR N8
#define JIS_LPAR LS(JIS_N8_AND_LPAR)

/* 9) number 9 and right parenthsis */
#define JIS_N9_AND_RPAR N9
#define JIS_RPAR LS(JIS_N9_AND_RPAR)

/* -= minus and equal */
#define JIS_MINUS_AND_EQUAL MINUS
#define JIS_EQUAL LS(JIS_MINUS_AND_EQUAL)

/* ^~ caret and tilde */
#define JIS_CARET_AND_TILDE EQUAL
#define JIS_CARET JIS_CARET_AND_TILDE
#define JIS_TILDE LS(JIS_CARET_AND_TILDE)

/* ¥| YEN and pipe */
#define JIS_YEN_AND_PIPE INTERNATIONAL_3
#define JIS_YEN JIS_YEN_AND_PIPE
#define JIS_PIPE LS(JIS_YEN)

/* @` at mark and Grave */
#define JIS_AT_AND_GRAVE LEFT_BRACKET
#define JIS_AT JIS_AT_AND_GRAVE
#define JIS_GRAVE LS(JIS_AT)

/* [{ left bracket and lfet brace */
#define JIS_LBKT_AND_LBRC RIGHT_BRACKET
#define JIS_LBKT JIS_LBKT_AND_LBRC
#define JIST_LBRC LS(JIS_LBKT)

/* ;+ semicolon and plus */
#define JIS_SEMI_AMD_PLUS SEMICOLON
#define JIS_SEMI JIS_SEMI_AMD_PLUS
#define JIS_PLUS LS(JIS_SEMI)

/* :* colon and astarisks */
#define JIS_COLON_AND_STAR SINGLE_QUATE
#define JIS_COLON JIS_SEMI_AMD_PLUS
#define JIS_STAR LS(JIS_COLON)

/* [{ Right bracket and Right brace */
#define JIS_RBKT_AND_RBRC NON_US_HASH
#define JIS_RBKT JIS_RBKT_AND_RBRC
#define JIS_RBRC LS(JIS_RBKT)

/* \_ Backslash and Underscore */
#define JIS_BSLH_AND_UNDS INTERNATIONAL_1
#define JIS_BSLH JIS_BSLH_AND_UNDS
#define JIS_UNDS LS(JIS_BSLH)

/* 英数 and Capslock  */
#define JIS_EISU_AND_CAPS CAPSLOCK
#define JIS_EISU JIS_EISU_AND_CAPS
#define JIS_CAPS LS(JIS_EISU)
