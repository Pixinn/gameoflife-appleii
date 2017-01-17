#ifndef _MUSIC_H
#define _MUSIC_H

#include <stdint.h>

void __fastcall__ note(const uint8_t nb_shift_duration, const uint8_t idx_not);
void __fastcall__ pause(const uint8_t nb_shift_duration);

enum eNotes {
           Ds3 = 0, E3, F3, Fs3, G3, Gs3, A3, As3, B3,
  C4, Cs4, D4, Ds4, E4, F4, Fs4, G4, Gs4, A4, As4, B4,
  C5, Cs5, D5, Ds5, E5, F5, Fs5, G5, Gs5, A5, As5, B5
};

enum eDuration {
  WHOLE = 0, HALF, QUARTER, EIGHTH, SIXTEENTH, THIRTY_SECOND, SIXTY_FOURTH, HUNDRED_TWENTY_EIGHTH
};

#endif
