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
#include "file_io.h"
#include "music.h"

/******************* FUNCTION DEFINITIONS **************/

void __fastcall__ init_asm( uint8_t* p_cell, uint8_t* p_cells_future );  /* Inits the variables used in the ASM scope */

void init_display( void );              /* Inits displayed playfield */
void clear_text( void );                /* Clears the 4 line text field in splitted mode */
void set_text( const char* const text );/* Prints a custom text into the 4 line text field in splitted mode */
void draw_cells( void );                /* Draws the actual cells */
void editor( void );                    /* lets the user draw some starting cells */
int8_t editor_load_save( const uint8_t load_or_save );
void toggle_cell( const uint8_t x, const uint8_t y ); /* toggles the cell at the given coordinates. \
                                                            Returns the cursor X position */
void title_screen( void );             /* Loads and display the title screen */
void run( void );                      /* runs the simulation */
void about( void );                          /* displays the About screen */

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

#define SAVENAME "GOL.SAVE."
#define SAVENAME_LENGTH 10

enum {
  LOAD,
  SAVE
};

/******************* STATIC GLOBAL VARIABLES ******************/

static uint8_t Cells[ NB_COLUMNS ][ NB_LINES ];
static uint8_t Cells_Future[ NB_COLUMNS ][ NB_LINES ];
static uint8_t Cells_Initial[ NB_COLUMNS ][ NB_LINES ];

/******************** CODE ************************/


int main( int argc, char** argv )
{
    (void)argc;
    (void)argv;

    /* Displaying the Title Screen */
    title_screen();

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
    printf("\n *** THIS WAS LIFE BY WWW.XTOF.INFO ***\nn");
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
                gfx_pixel ( get_color(), x, y );
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
    uint8_t update_color = 1;

    const char* const text = "H K U J: Move the cursor\nSPACE  : Toggle a cell\nn(L)oad - (S)ave - (R)un - (A)bout";
    set_text( text );

    //Place the cursor middle screen
    x_cursor = NB_COLUMNS >> 1u;
    y_cursor = NB_LINES >> 1u;

    quit = 0;
    while ( quit == 0)
    {
        if (update_color) { color_pixel = gfx_get_pixel( x_cursor, y_cursor ); }
        gfx_pixel( CURSOR_COLOR, x_cursor, y_cursor ); //cursor
        KeyPressed = cgetc();
        switch (KeyPressed) {
        case KEY_LEFT:
                note(SIXTY_FOURTH, G3);
                update_color = 1;
                gfx_pixel( color_pixel, x_cursor, y_cursor );
                if( x_cursor > 1u ) { --x_cursor; }
          break;
        case KEY_DOWN:
                note(SIXTY_FOURTH, G3);
                update_color = 1;
                gfx_pixel( color_pixel, x_cursor, y_cursor );
                if( y_cursor < NB_LINES-2u ) { ++y_cursor; }
          break;
        case KEY_UP:
                note(SIXTY_FOURTH, G3);
                update_color = 1;
                gfx_pixel( color_pixel, x_cursor, y_cursor );
                if( y_cursor > 1u ) { --y_cursor; }
          break;
        case KEY_RIGHT:
                note(SIXTY_FOURTH, G3);
                update_color = 1;
                gfx_pixel( color_pixel, x_cursor, y_cursor );
                if( x_cursor < NB_COLUMNS-2u ) {	++x_cursor; }
          break;
        case ' ':
                if(    x_cursor > 0u && x_cursor < NB_COLUMNS-1u
                    && y_cursor > 0u &&  y_cursor < NB_LINES-1u )
                {
                  update_color = 1;
                  toggle_cell( x_cursor++, y_cursor );
                  note(SIXTY_FOURTH, G5);
                }
                break;
        case 'l':
          update_color = 0;
          if( editor_load_save( LOAD ) == 0 ) {
            draw_cells();
            update_color = 1;
          }
          set_text( text );
          break;
        case 's':
          if( editor_load_save( SAVE ) == 0) {
            set_text( "Stage saved!\nnPress a key to continue." );
            cgetc();
          }
          set_text( text );
          gfx_pixel( color_pixel, x_cursor, y_cursor );
          update_color = 0;
          break;
        case 'r':
          quit = 1;
          gfx_pixel( color_pixel, x_cursor, y_cursor ); //clear cursor
          update_color = 0;
          break;
        case 'a':
          about();
        default:
          update_color = 0;
          break;
        }
    }

    /* Cells was updated by the calls to toggle() */
    memcpy( Cells_Future, Cells, sizeof(Cells_Future) );
    memcpy( Cells_Initial, Cells, sizeof(Cells_Initial) );


}

// A common function to load or save the content of Cells.
// A custom string and function pointer is all that differentiate their behavior
int8_t editor_load_save( const uint8_t load_or_save )
{
  // NOTE sprintf with %s does not work...
  #define ESC 0x1B
  uint8_t handle;
  char filename[ SAVENAME_LENGTH + 1 ];
  char custom_text[ 80 ];
  char str_custom[ 8 ];
  char num;
  uint16_t nb_bytes;
  load_or_save == LOAD ?  strcpy( str_custom, "loaded ") : \
                          strcpy( str_custom, "saved ");
  do {
    strcpy( custom_text, "Enter the slot number to be " );
    strcat( custom_text, str_custom );
    strcat( custom_text,  "[0-9]\nOr press escape to cancel.\n" );
    set_text( custom_text );
    num = cgetc();
  } while( num != ESC && (num < 0x30 || num > 0x39) );
  if( num == ESC ) { return -1; }

  strcpy( filename, SAVENAME );
  filename[ SAVENAME_LENGTH - 1 ] = num;
  filename[ SAVENAME_LENGTH ] = '\0';
  file_open( filename, &handle );
  strcpy( custom_text, "Accessing: " );
  strcat( custom_text, filename );
  set_text( custom_text );
  if( file_error() != NO_ERROR ) {
    strcpy( custom_text, "Cannot open file to be " );
    strcat( custom_text, str_custom );
    set_text( custom_text );
    cgetc();
    file_close( handle );
    return -1;
  }
  load_or_save == LOAD ?  nb_bytes =  file_read( handle, (uint8_t*)Cells, NB_COLUMNS*NB_LINES ) : \
                          nb_bytes =  file_write( handle, (uint8_t*)Cells, NB_COLUMNS*NB_LINES );
  if( file_error() != NO_ERROR || nb_bytes != NB_COLUMNS*NB_LINES ) {
    sprintf( custom_text, "An error occured: %x\nnPress a key to continue.", file_error() );
    set_text( custom_text );
    cgetc();
    file_close( handle );
    return -1;
  }
  file_close( handle );
  return 0;
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
    printf("Iteration:1\nn(R)eset (E)ditor (Q)uit");
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

/****** HIRES SCREEN DEFINITIONS ***********/
#define HIRES_PAGE2     (char*)0x4000
#define HIRES_PAGE_SIZE 0x2000
#define SWITCH_TEXT *((uint8_t*)0xC050)=0
#define SWITCH_GRAPHICS *((uint8_t*)0xC050)=1
#define SWITCH_FULLSCREEN *((uint8_t*)0xC052)=1
#define SWITCH_PAGE2 *((uint8_t*)0xC055)=1
#define SWITCH_HIRES *((uint8_t*)0xC057)=1
/******* KEYBOARD ***********/
#define CLEAR_KEYBOARD_STROBE *((uint8_t*)0xC010)=1;
#define KEY_PRESSED (*((uint8_t*)0xC000)&0x7F) != 0

//The Title Screen asset is located in the "assets" folder
void title_screen( void )
{
  //Loading and displaying the picture
  uint8_t handle;
  file_open("GOL.SCREEN", &handle);
  SWITCH_GRAPHICS;
  SWITCH_FULLSCREEN;
  SWITCH_PAGE2;
  SWITCH_HIRES;
  if(file_read( handle, HIRES_PAGE2, HIRES_PAGE_SIZE ) != HIRES_PAGE_SIZE )  {
    printf("\nERROR, CANNOT READ GOL.SCREEN\nERRNO: %x\nn", file_error());
    file_close(handle);
    SWITCH_TEXT;
    exit(-1);
  }
  file_close(handle);

  //Playing the music
  pause(HALF);

  note(WHOLE, D4);
  note(QUARTER, F4);
  note(HALF, G4);
  note(QUARTER, As4);

  pause(QUARTER);

  note(QUARTER, F5);
  note(QUARTER, D5);
  note(WHOLE, C5);

  pause(QUARTER);

  note(EIGHTH, As4);
  note(EIGHTH, A4);
  note(EIGHTH, As4);
  note(EIGHTH, A4);
  note(EIGHTH, As4);
  note(EIGHTH, A4);
  note(WHOLE, G4);

  pause(EIGHTH);

  note(QUARTER, F4);
  note(QUARTER, G5);
}



void about( void )
{
  register uint8_t i;
  mode_text();
  gotoxy( 0u, 0u );
  for( i = 0; i < 24u; ++i ) {
      printf("                                       "); /* clears a line */
  }
  gotoxy( 0u, 0u );

  printf(  "         *** A GAME OF LIFE ***\n             --------------\n\n\
This port of Conway's Game of life was\nprogrammed by Christophe Meneboeuf.\n\
The code is available on my GitHub:\nhttps://www.github.com/pixinn\n\
In depth articles available on my blog:\nhttps://www.xtof.info/blog/\n\
Follow me on Twitter: @pixinn\n\n\
------\nMusic by Clint Slate\nhttps://www.facebook.com/clintslate\n------\n\n\
     FEEL FREE TO SHARE THIS DISK!" );
     cgetc();
     init_display();
     draw_cells();
}
