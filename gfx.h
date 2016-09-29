#ifndef __GFX_H__
#define __GFX_H__


void __fastcall__ mode_text( void );
void __fastcall__ gfx_init( uint8_t resolution, uint8_t mode );
void __fastcall__ gfx_fill( uint8_t color );
void __fastcall__ gfx_pixel( uint8_t color, uint8_t coord_x, uint8_t coord_y );
void __fastcall__ gfx_refresh( void );

enum eMode {
  FULLSCREEN = 0,
  SPLIT = 1
};
enum eResolution {
  LOWRES = 0,
  HIRES = 1
};
enum eColor {
  BLACK = 0,
  MAGENTA,
  DARKBLUE,
  PURPLE,
  DARKGREEN,
  GREY,
  MEDIUMBLUE,
  LIGHTBLUE,
  BROWN,
  ORANGE,
  GREY2,
  PINK,
  GREEN,
  YELLOW,
  AQUA,
  WHITE
};

#endif
