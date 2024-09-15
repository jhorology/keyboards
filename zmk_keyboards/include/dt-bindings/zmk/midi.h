#pragma once

/**
 * MIDI note number (0 - 127)
 * range C-1 NOTE_C(-1) - G9 NOTE_G(9)
 * In generarlly, A4 NOTE_A(4) = 440Hz
 */
#define NOTE_C(octave) (0 + (octave + 1) * 12)
#define NOTE_Cs(octave) (1 + (octave + 1) * 12)
#define NOTE_Db(octave) (1 + (octave + 1) * 12)
#define NOTE_D(octave) (2 + (octave + 1) * 12)
#define NOTE_Ds(octave) (3 + (octave + 1) * 12)
#define NOTE_Eb(octave) (3 + (octave + 1) * 12)
#define NOTE_E(octave) (4 + (octave + 1) * 12)
#define NOTE_F(octave) (5 + (octave + 1) * 12)
#define NOTE_Fs(octave) (6 + (octave + 1) * 12)
#define NOTE_Gb(octave) (6 + (octave + 1) * 12)
#define NOTE_G(octave) (7 + (octave + 1) * 12)
#define NOTE_Gs(octave) (8 + (octave + 1) * 12)
#define NOTE_Ab(octave) (8 + (octave + 1) * 12)
#define NOTE_A(octave) (9 + (octave + 1) * 12)
#define NOTE_As(octave) (10 + (octave + 1) * 12)
#define NOTE_Bb(octave) (10 + (octave + 1) * 12)
#define NOTE_B(octave) (11 + (octave + 1) * 12)
