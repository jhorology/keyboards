// Copyright 2021 @ Keychron (https://www.keychron.com)
// SPDX-License-Identifier: GPL-2.0-or-later
// This file has been modified from the original keychron's QMK source code.

#ifndef DT_BINDINGS_LED_SNLED27351_H
#define DT_BINDINGS_LED_SNLED27351_H

/* PWM register addresses for devicetree byte-array properties (no 0x prefix). */
#define SNLED27351_RGB(r, g, b) r g b

#define CB1_CA1 00
#define CB1_CA2 01
#define CB1_CA3 02
#define CB1_CA4 03
#define CB1_CA5 04
#define CB1_CA6 05
#define CB1_CA7 06
#define CB1_CA8 07
#define CB1_CA9 08
#define CB1_CA10 09
#define CB1_CA11 0a
#define CB1_CA12 0b
#define CB1_CA13 0c
#define CB1_CA14 0d
#define CB1_CA15 0e
#define CB1_CA16 0f

#define CB2_CA1 10
#define CB2_CA2 11
#define CB2_CA3 12
#define CB2_CA4 13
#define CB2_CA5 14
#define CB2_CA6 15
#define CB2_CA7 16
#define CB2_CA8 17
#define CB2_CA9 18
#define CB2_CA10 19
#define CB2_CA11 1a
#define CB2_CA12 1b
#define CB2_CA13 1c
#define CB2_CA14 1d
#define CB2_CA15 1e
#define CB2_CA16 1f

#define CB3_CA1 20
#define CB3_CA2 21
#define CB3_CA3 22
#define CB3_CA4 23
#define CB3_CA5 24
#define CB3_CA6 25
#define CB3_CA7 26
#define CB3_CA8 27
#define CB3_CA9 28
#define CB3_CA10 29
#define CB3_CA11 2a
#define CB3_CA12 2b
#define CB3_CA13 2c
#define CB3_CA14 2d
#define CB3_CA15 2e
#define CB3_CA16 2f

#define CB4_CA1 30
#define CB4_CA2 31
#define CB4_CA3 32
#define CB4_CA4 33
#define CB4_CA5 34
#define CB4_CA6 35
#define CB4_CA7 36
#define CB4_CA8 37
#define CB4_CA9 38
#define CB4_CA10 39
#define CB4_CA11 3a
#define CB4_CA12 3b
#define CB4_CA13 3c
#define CB4_CA14 3d
#define CB4_CA15 3e
#define CB4_CA16 3f

#define CB5_CA1 40
#define CB5_CA2 41
#define CB5_CA3 42
#define CB5_CA4 43
#define CB5_CA5 44
#define CB5_CA6 45
#define CB5_CA7 46
#define CB5_CA8 47
#define CB5_CA9 48
#define CB5_CA10 49
#define CB5_CA11 4a
#define CB5_CA12 4b
#define CB5_CA13 4c
#define CB5_CA14 4d
#define CB5_CA15 4e
#define CB5_CA16 4f

#define CB6_CA1 50
#define CB6_CA2 51
#define CB6_CA3 52
#define CB6_CA4 53
#define CB6_CA5 54
#define CB6_CA6 55
#define CB6_CA7 56
#define CB6_CA8 57
#define CB6_CA9 58
#define CB6_CA10 59
#define CB6_CA11 5a
#define CB6_CA12 5b
#define CB6_CA13 5c
#define CB6_CA14 5d
#define CB6_CA15 5e
#define CB6_CA16 5f

#define CB7_CA1 60
#define CB7_CA2 61
#define CB7_CA3 62
#define CB7_CA4 63
#define CB7_CA5 64
#define CB7_CA6 65
#define CB7_CA7 66
#define CB7_CA8 67
#define CB7_CA9 68
#define CB7_CA10 69
#define CB7_CA11 6a
#define CB7_CA12 6b
#define CB7_CA13 6c
#define CB7_CA14 6d
#define CB7_CA15 6e
#define CB7_CA16 6f

#define CB8_CA1 70
#define CB8_CA2 71
#define CB8_CA3 72
#define CB8_CA4 73
#define CB8_CA5 74
#define CB8_CA6 75
#define CB8_CA7 76
#define CB8_CA8 77
#define CB8_CA9 78
#define CB8_CA10 79
#define CB8_CA11 7a
#define CB8_CA12 7b
#define CB8_CA13 7c
#define CB8_CA14 7d
#define CB8_CA15 7e
#define CB8_CA16 7f

#define CB9_CA1 80
#define CB9_CA2 81
#define CB9_CA3 82
#define CB9_CA4 83
#define CB9_CA5 84
#define CB9_CA6 85
#define CB9_CA7 86
#define CB9_CA8 87
#define CB9_CA9 88
#define CB9_CA10 89
#define CB9_CA11 8a
#define CB9_CA12 8b
#define CB9_CA13 8c
#define CB9_CA14 8d
#define CB9_CA15 8e
#define CB9_CA16 8f

#define CB10_CA1 90
#define CB10_CA2 91
#define CB10_CA3 92
#define CB10_CA4 93
#define CB10_CA5 94
#define CB10_CA6 95
#define CB10_CA7 96
#define CB10_CA8 97
#define CB10_CA9 98
#define CB10_CA10 99
#define CB10_CA11 9a
#define CB10_CA12 9b
#define CB10_CA13 9c
#define CB10_CA14 9d
#define CB10_CA15 9e
#define CB10_CA16 9f

#define CB11_CA1 a0
#define CB11_CA2 a1
#define CB11_CA3 a2
#define CB11_CA4 a3
#define CB11_CA5 a4
#define CB11_CA6 a5
#define CB11_CA7 a6
#define CB11_CA8 a7
#define CB11_CA9 a8
#define CB11_CA10 a9
#define CB11_CA11 aa
#define CB11_CA12 ab
#define CB11_CA13 ac
#define CB11_CA14 ad
#define CB11_CA15 ae
#define CB11_CA16 af

#define CB12_CA1 b0
#define CB12_CA2 b1
#define CB12_CA3 b2
#define CB12_CA4 b3
#define CB12_CA5 b4
#define CB12_CA6 b5
#define CB12_CA7 b6
#define CB12_CA8 b7
#define CB12_CA9 b8
#define CB12_CA10 b9
#define CB12_CA11 ba
#define CB12_CA12 bb
#define CB12_CA13 bc
#define CB12_CA14 bd
#define CB12_CA15 be
#define CB12_CA16 bf

#endif /* DT_BINDINGS_LED_SNLED27351_H */
