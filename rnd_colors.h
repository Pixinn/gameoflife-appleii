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

#ifndef __RND_COLOTS__H_
#define  __RND_COLOTS__H_

extern uint8_t Colors[256];

void    __fastcall__  init_rnd_color( void );            /* Inits a buffer of 256 random colors */
uint8_t __fastcall__  get_color( void );                 /* Gets the next color from the buffer */

#endif
