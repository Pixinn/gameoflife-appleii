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
int8_t editor_erase( void );            /* erases the content of the edit window */
int8_t editor_load_save( const uint8_t load_or_save );
int8_t file_load_save(char* filename, const uint8_t load_or_save, uint8_t* buffer, const uint16_t len);
void toggle_cell( const uint8_t x, const uint8_t y ); /* toggles the cell at the given coordinates. \
                                                            Returns the cursor X position */
uint16_t my_sleep(const uint8_t time);    /* "Sleeps" for an amount of time. 1 ~= 4.2ms  A dummy value is returned to trick the optimizer */
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

enum{
    ERR_FILE_NOERROR,
    ERR_FILE_OPEN,
    ERR_FILE_READ
};

enum eMyBoolean {
    FALSE = 0,
    TRUE = 1
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
                if( KeyPressed == 's' ) { /* Go back to editor */
                    State = STATE_EDITOR;
                } else if( KeyPressed == 'r' ) { /* reset and rerun */
                    memcpy( Cells, Cells_Initial, sizeof(Cells) );
                    memcpy( Cells_Future, Cells_Initial, sizeof(Cells_Future) );
                    draw_cells();
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
    printf("\n       *** READ WWW.XTOF.INFO ***\nn");
    exit(0);
}


void clear_coords()
{
    gotoxy(NB_COLUMNS-6, PRINTF_LINE+3);
    printf("     ");
}

void print_coords(const uint8_t x, const uint8_t y)
{
    uint8_t len_coord_cursor_x_len;
    char   buf_coord_cursor[10];
    char   buf_coord_cursor_y[5];

    /* Clears the coords */
    clear_coords();
    /* Coords to ascii */
    itoa(x, buf_coord_cursor, 10);
    len_coord_cursor_x_len = strlen(buf_coord_cursor);
    itoa(y, buf_coord_cursor_y, 10);
    strcat(buf_coord_cursor, ":");
    strcat(buf_coord_cursor, buf_coord_cursor_y);

    gotoxy(NB_COLUMNS-6+(2-len_coord_cursor_x_len), PRINTF_LINE+3);
    /* Prints the coords */
    printf(buf_coord_cursor);
}


void clear_text( void )
{
    register uint8_t i;
    clear_coords();
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

const char* const Text_Editor ="H K U J: Move the cursor\nSPACE  : Toggle a cell\n(L)oad - (S)ave - (E)rase - (R)un\n(A)bout";
void editor( void )
{
    #define KEY_LEFT    'h'
    #define KEY_DOWN    'j'
    #define KEY_UP      'u'
    #define KEY_RIGHT   'k'

    uint8_t quit, x_cursor, y_cursor;
    uint8_t color_pixel;
    uint8_t update_color = TRUE;
    uint8_t update_coords = TRUE;

    set_text( Text_Editor );

    //Place the cursor middle screen
    x_cursor = NB_COLUMNS >> 1u;
    y_cursor = NB_LINES >> 1u;

    quit = 0u;
    while ( quit == 0u)
    {
        if (update_coords != FALSE)
        {
            print_coords(x_cursor, y_cursor);
            update_coords = FALSE;
        }
        if (update_color != FALSE) {
            color_pixel = gfx_get_pixel( x_cursor, y_cursor );
        }
        gfx_pixel( CURSOR_COLOR, x_cursor, y_cursor ); //cursor
        KeyPressed = cgetc();
        switch (KeyPressed) {
        case KEY_LEFT:
                note(SIXTY_FOURTH, G3);
                update_color = TRUE;
                update_coords = TRUE;
                gfx_pixel( color_pixel, x_cursor, y_cursor );
                if( x_cursor > 1u ) { --x_cursor; }
          break;
        case KEY_DOWN:
                note(SIXTY_FOURTH, G3);
                update_color = TRUE;
                update_coords = TRUE;
                gfx_pixel( color_pixel, x_cursor, y_cursor );
                if( y_cursor < NB_LINES-2u ) { ++y_cursor; }
          break;
        case KEY_UP:
                note(SIXTY_FOURTH, G3);
                update_color = TRUE;
                update_coords = TRUE;
                gfx_pixel( color_pixel, x_cursor, y_cursor );
                if( y_cursor > 1u ) { --y_cursor; }
          break;
        case KEY_RIGHT:
                note(SIXTY_FOURTH, G3);
                update_color = TRUE;
                update_coords = TRUE;
                gfx_pixel( color_pixel, x_cursor, y_cursor );
                if( x_cursor < NB_COLUMNS-2u ) {	++x_cursor; }
          break;
        case ' ':
                if(    x_cursor > 0u && x_cursor < NB_COLUMNS-1u
                    && y_cursor > 0u &&  y_cursor < NB_LINES-1u )
                {
                  update_color = TRUE;
                  update_coords = TRUE;
                  toggle_cell( x_cursor++, y_cursor );
                  note(SIXTY_FOURTH, G5);
                  my_sleep(5u);  /* ~22ms */
                }
                break;
        case 'l':
          update_color = FALSE;
          update_coords = TRUE;
          if( editor_load_save( LOAD ) == 0 ) {
            draw_cells();
            update_color = TRUE;
          }
          set_text( Text_Editor );
          break;
        case 's':
          if( editor_load_save( SAVE ) == 0) {
            set_text( "Stage saved!\nPress a key to continue." );
            cgetc();
          }
          set_text( Text_Editor );
          gfx_pixel( color_pixel, x_cursor, y_cursor );
          update_color = FALSE;
          update_coords = TRUE;
          break;
        case 'e':
          update_coords = TRUE;
          update_color = FALSE;
          if( editor_erase() == 0 ) {
              draw_cells();
              update_color = TRUE;
          }
          set_text( Text_Editor );
          break;
        case 'r':
          quit = 1;
          gfx_pixel( color_pixel, x_cursor, y_cursor ); //clear cursor
          update_color = FALSE;
          update_coords = TRUE;
          break;
        case 'a':
          about();
          update_coords = TRUE;
        default:
          update_color = FALSE;
          update_coords = FALSE;
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
  char filename[ SAVENAME_LENGTH + 1 ];
  char custom_text[ 80 ];
  char str_custom[ 8 ];
  char num;
  int8_t err_file;

  load_or_save == LOAD ?  strcpy( str_custom, "loaded ") : \
                          strcpy( str_custom, "saved ");
  do {
    strcpy( custom_text, "Enter the slot number to be " );
    strcat( custom_text, str_custom );
    strcat( custom_text,  "[1-9]\nOr press escape to cancel.\n" );
    set_text( custom_text );
    num = cgetc();
  } while( num != ESC && (num < 0x31 || num > 0x39) );
  if( num == ESC ) { return -1; }

  strcpy( filename, SAVENAME );
  filename[ SAVENAME_LENGTH - 1 ] = num;
  filename[ SAVENAME_LENGTH ] = '\0';
  strcpy( custom_text, "Accessing: " );
  strcat( custom_text, filename );
  set_text( custom_text );
  err_file = file_load_save(filename, load_or_save, (uint8_t*)Cells, NB_COLUMNS*NB_LINES);
  if(err_file == ERR_FILE_OPEN) {
      strcpy( custom_text, "Cannot open file to be " );
      strcat( custom_text, str_custom );
      set_text( custom_text );
      cgetc();
      return -1;
  }
  else if( err_file == ERR_FILE_READ) {
      sprintf( custom_text, "An error occured: %x\nPress a key to continue.", file_error() );
      set_text( custom_text );
      cgetc();
      return -1;
  }
  return 0;
}

// A generic function to load / save the content of a file to / from a buffer
// filename: file to load / save
// load_or_save: LOAD or SAVE?
// buffer: buffer to write / read
// len: length to write / read
int8_t file_load_save(char* filename, const uint8_t load_or_save, uint8_t* buffer, const uint16_t len)
{
    uint16_t nb_bytes;
    uint8_t handle;
    file_open( filename, &handle );
    if( file_error() != NO_ERROR ) {
      file_close( handle );
      return ERR_FILE_OPEN;
    }
    load_or_save == LOAD ?  nb_bytes =  file_read( handle, buffer, len ) : \
                            nb_bytes =  file_write( handle, buffer, len );
    if( file_error() != NO_ERROR || nb_bytes != len ) {
      file_close( handle );
      return ERR_FILE_READ;
    }
    file_close( handle );
    return ERR_FILE_NOERROR;
}

// Resets loads the SAVE #0 that is inaccessible otherwise
int8_t editor_erase( void )
{
    char filename[ SAVENAME_LENGTH + 1 ];
    set_text( "Clearing the screen..." );
    strcpy( filename, SAVENAME );
    filename[ SAVENAME_LENGTH - 1 ] = 0x30;
    filename[ SAVENAME_LENGTH ] = '\0';
    return file_load_save(filename, LOAD, (uint8_t*)Cells, NB_COLUMNS*NB_LINES) == ERR_FILE_NOERROR ? 0 : -1;
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


uint16_t my_sleep(const uint8_t time)
{
    uint16_t i;
    volatile uint16_t dummy = 0u;
    uint16_t nb_iter = time << 7;
    for(i = 0u; i < nb_iter; ++i) {
        dummy += i;
    }
    return dummy;
}


void run( void  )
{
    char str_nb_iteration [5];
    uint16_t nb_iterations = 2u;
    KeyPressed = NO_KEY;
    gotoxy( 0u, PRINTF_LINE+1 );
    printf("Iteration:1\n(R)estart (S)top");
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
  clrscr();;
  mode_text();
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
     set_text( Text_Editor );
     draw_cells();
}
