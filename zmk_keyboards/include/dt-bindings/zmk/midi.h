#pragma once

/**
 * MIDI note number (0 - 127)
 * range C-1 NOTE_C(-1) - G9 NOTE_G(9)
 * In generarlly, A4 NOTE_A(4) = 440Hz
 */
#define NOTE(semi, oct) ((semi) + ((oct) + 1) * 12)

#define NOTE_C(oct) NOTE(0, oct)
#define NOTE_Cs(oct) NOTE(1, oct)
#define NOTE_Db(oct) NOTE(1, oct)
#define NOTE_D(oct) NOTE(2, oct)
#define NOTE_Ds(oct) NOTE(3, oct)
#define NOTE_Eb(oct) NOTE(3, oct)
#define NOTE_E(oct) NOTE(4, oct)
#define NOTE_F(oct) NOTE(5, oct)
#define NOTE_Fs(oct) NOTE(6, oct)
#define NOTE_Gb(oct) NOTE(6, oct)
#define NOTE_G(oct) NOTE(7, oct)
#define NOTE_Gs(oct) NOTE(8, oct)
#define NOTE_Ab(oct) NOTE(8, oct)
#define NOTE_A(oct) NOTE(9, oct)
#define NOTE_As(oct) NOTE(10, oct)
#define NOTE_Bb(oct) NOTE(10, oct)
#define NOTE_B(oct) NOTE(11, oct)
