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

/* Standard headers */
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
/* CC65's Specific headers */
#include <conio.h>
/* My headers */
#include "gfx.h"
#include "rnd_colors.h"

/******************* FUNCTION DEFINITIONS **************/

void __fastcall__ init_asm( uint8_t* p_cell, uint8_t* p_cells_future );  /* Inits the variables used in the ASM scope */

void init_display( void );              /* Inits displayed playfield */
void clear_text( void );                /* Clears the 4 line text field in splitted mode */
void set_text( const char* const text );/* Prints a custom text into the 4 line text field in splitted mode */
void draw_cells( void );                /* Draws the actual cells */
void editor( void );                    /* lets the user draw some starting cells */
void editor_load( void );               /* Loads a previously saved state into the editor */
void editor_save( void );               /* Saves the current editor's state */
void toggle_cell( const uint8_t x, const uint8_t y ); /* toggles the cell at the given coordinates. \
                                                            Returns the cursor X position */

void run( void );                           /* runs the simulation */
void __fastcall__ update( void );      /* updates the simulation */
uint8_t __fastcall__  count_neighbours( uint8_t* cell );                /* counts nb neighbours of the cell */


void quit( void );

/******************* CUSTOM TYPES AND VALUES DEFINITIONS ****************/

#define NB_LINES    40u
#define NB_COLUMNS  40u

#define ALIVE       1u
#define DEAD        0u
#define SPRITE_ALIVE    '0'
#define SPRITE_DEAD     ' '

#define STATE_INIT      0u
#define STATE_EDITOR    1u
#define STATE_RUN       2u
#define STATE_QUIT      3u
uint8_t State = STATE_INIT;

#define NO_KEY     '\0'
char KeyPressed = NO_KEY;

#define CURSOR_COLOR    WHITE
#define BORDER_COLOR    WHITE
#define CELL_COLOR      ORANGE


#define PRINTF_LINE 20u


/******************* STATIC GLOBAL VARIABLES ******************/

static uint8_t Cells[ NB_COLUMNS ][ NB_LINES ];
static uint8_t Cells_Future[ NB_COLUMNS ][ NB_LINES ];
static uint8_t Cells_Initial[ NB_COLUMNS ][ NB_LINES ];

/******************** CODE ************************/


int main( int argc, char** argv )
{
    (void)argc;
    (void)argv;

    init_asm( (uint8_t*)Cells, (uint8_t*)Cells_Future );
    init_rnd_color();

    /* Running the state machine */
    while( State != STATE_QUIT )
    {
        switch (State) {
            case STATE_INIT:
                memset( Cells, DEAD, sizeof(Cells) );
                init_display();
                State = STATE_EDITOR;
                break;
            case STATE_EDITOR:
                editor();
                State = STATE_RUN;
                break;
            case STATE_RUN:
                clear_text();
                run();
                if( KeyPressed == 'e' ) { /* Go back to editor */
                    State = STATE_EDITOR;
                } else if( KeyPressed == 'r' ) { /* reset and rerun */
                    memcpy( Cells, Cells_Initial, sizeof(Cells) );
                    memcpy( Cells_Future, Cells_Initial, sizeof(Cells_Future) );
                    draw_cells();
                } else if( KeyPressed == 'q' ) { /* quit */
                    State = STATE_QUIT;
                }
                break;
            default:
                mode_text();
                printf("ERROR!");
                State = STATE_QUIT;
                break;
        }
    }

    quit();
    return 0;
}

void quit( void )
{
    uint8_t x, y;
    mode_text();
    screensize (&x, &y);
    clrscr();
    printf("\n*** THIS WAS LIFE BY WWW.XTOF.INFO ***\n\n");
    exit(0);
}


void clear_text( void )
{
    register uint8_t i;
    gotoxy( 0u, PRINTF_LINE );
    for( i = 0; i < 4u; ++i ) {
        printf("                                       "); /* clears a line */
    }
}

void set_text( const char* const text )
{
  clear_text();
  gotoxy( 0u, PRINTF_LINE );
  printf( text );
}


void init_display( void )
{
    register uint8_t i;
    clrscr();
    gfx_init( LOWRES, SPLIT );
    gfx_fill( BLACK );
    for( i = 0u; i < NB_COLUMNS; ++i ) {
      gfx_pixel( BORDER_COLOR, i, 0u );
    }
    for( i = 0u; i < NB_COLUMNS; ++i ) {
      gfx_pixel( BORDER_COLOR, i, NB_LINES-1u );
    }
    for( i = 0u; i < NB_LINES; ++i ) {
      gfx_pixel( BORDER_COLOR, 0u, i );
    }
    for( i = 0u; i < NB_LINES; ++i ) {
      gfx_pixel( BORDER_COLOR, NB_LINES-1u, i );
    }
}


void draw_cells( void ) {
    uint8_t x, y;
    for( x = 1u; x < NB_COLUMNS - 1u; ++x )
    {
        for( y = 1u; y < NB_LINES - 1u; ++y )
        {
            if( Cells[x][y] == ALIVE ) {
                gfx_pixel ( CELL_COLOR, x, y );
            } else {
                gfx_pixel ( BLACK, x, y );
            }
        }
    }
}


void editor( void )
{
    #define KEY_LEFT    'h'
    #define KEY_DOWN    'j'
    #define KEY_UP      'u'
    #define KEY_RIGHT   'k'

    uint8_t quit, x_cursor, y_cursor;
    uint8_t color_pixel;

    const char* const text = "J L I K: Move the cursor\nSPACE  : Toggle a cell\n\n(L)oad - (S)ave - (D)one";
    set_text( text );

    //Place the cursor middle screen
    x_cursor = NB_COLUMNS >> 1u;
    y_cursor = NB_LINES >> 1u;

    quit = 0;
    while ( quit == 0)
    {
        color_pixel = gfx_get_pixel( x_cursor, y_cursor );
        gfx_pixel( CURSOR_COLOR, x_cursor, y_cursor ); //cursor
        KeyPressed = cgetc();
        switch (KeyPressed) {
        case KEY_LEFT:
                gfx_pixel( color_pixel, x_cursor, y_cursor );
                if( x_cursor > 1u ) { --x_cursor; }
          break;
        case KEY_DOWN:
                gfx_pixel( color_pixel, x_cursor, y_cursor );
                if( y_cursor < NB_LINES-2u ) { ++y_cursor; }
          break;
        case KEY_UP:
                gfx_pixel( color_pixel, x_cursor, y_cursor );
                if( y_cursor > 1u ) { --y_cursor; }
          break;
        case KEY_RIGHT:
                gfx_pixel( color_pixel, x_cursor, y_cursor );
                if( x_cursor < NB_COLUMNS-2u ) {	++x_cursor; }
          break;
        case ' ':
                if(    x_cursor > 0u && x_cursor < NB_COLUMNS-1u
                    && y_cursor > 0u &&  y_cursor < NB_LINES-1u )
                {
                  toggle_cell( x_cursor++, y_cursor );
                }
                break;
        case 'l':
          editor_save();
          set_text( text );
          gfx_pixel( color_pixel, x_cursor, y_cursor );
          break;
        case 's':
          editor_save();
          set_text( text );
          gfx_pixel( color_pixel, x_cursor, y_cursor );
          break;
        case 'd':
          quit = 1;
          gfx_pixel( color_pixel, x_cursor, y_cursor ); //clear cursor
          break;
        default: break;
        }
    }

    /* Cells was updated by the calls to toggle() */
    memcpy( Cells_Future, Cells, sizeof(Cells_Future) );
    memcpy( Cells_Initial, Cells, sizeof(Cells_Initial) );


}


void editor_load( void )
{
  #define ESC 0x1B
  char num;
  do {
    set_text( "Enter the number to be loaded [0-9]\nOr press escape to cancel.\n" );
    num = cgetc();
  } while( num != ESC && (num < 0x30 || num > 0x39) );
}



void editor_save( void )
{
  #define ESC 0x1B
  char num;
  do {
    set_text( "Enter the number to be saved [0-9]\nOr press escape to cancel.\n" );
    num = cgetc();
  } while( num != ESC && (num < 0x30 || num > 0x39) );
}



void toggle_cell( const uint8_t x, const uint8_t y )
{
    char* cell;
    if( x == 0u || x >= NB_COLUMNS-1u || y == 0u || y >= NB_LINES-1u ) { return; }
    cell = &Cells[x][y];
    if( *cell == DEAD ) {
        *cell = ALIVE;
        gfx_pixel( get_color(), x, y );
    } else {
        *cell = DEAD;
        gfx_pixel( BLACK, x, y );
    }
}


void run( void  )
{
    char str_nb_iteration [5];
    uint16_t nb_iterations = 2u;
    KeyPressed = NO_KEY;
    gotoxy( 0u, PRINTF_LINE+1 );
    printf("Iteration:1\n\n(R)eset (E)ditor (Q)uit");
    while( KeyPressed == NO_KEY)
    {
        /* Evolving the cells */
        update( );
        gotoxy(10u, PRINTF_LINE+1);
        printf( itoa(nb_iterations++, str_nb_iteration, 10) );
        /* Testing key pressed */
        if( kbhit() ) {
            KeyPressed = cgetc();
            break;
        }
    }
}
