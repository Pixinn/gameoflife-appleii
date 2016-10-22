/* Conway's Game of Life for Apple II
* Copyright (C) 2016 Christophe Meneboeuf <christophe@xtof.info>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __GFX_H__
#define __GFX_H__

// Restores text mode
void __fastcall__ mode_text( void );
// Switches to the desired res and mode.
// Always displays page 1
void __fastcall__ gfx_init( uint8_t resolution, uint8_t mode );
// Fills the screen with the given color
void __fastcall__ gfx_fill( uint8_t color );
// Draws a pixel at the given coordinates
void __fastcall__ gfx_pixel( uint8_t color, uint8_t coord_x, uint8_t coord_y );
// Returns the pixel's color
uint8_t __fastcall__ gfx_get_pixel( uint8_t coord_x, uint8_t coord_y );
// Updates the screen by displaying the Future Page which becomes Current
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
