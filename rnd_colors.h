#ifndef __RND_COLOTS__H_
#define  __RND_COLOTS__H_

extern uint8_t Colors[256];

void    __fastcall__  init_rnd_color( void );            /* Inits a buffer of 256 random colors */
uint8_t __fastcall__  get_color( void );                 /* Gets the next color from the buffer */

#endif
