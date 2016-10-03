/* Standard headers */
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
/* Specific headers */
#include <conio.h>

#include "gfx.h"
#include "rnd_colors.h"

/******************* FUNCTION DEFINITIONS **************/

void __fastcall__ init_asm( uint8_t* p_cell, uint8_t* p_cells_future );  /* Inits the variables used in the ASM scope */

void init_display( void );              /* Inits displayed playfield */
void draw_cells( void );                /* Draws the actual cells */
void editor( void );                    /* lets the user draw some starting cells */
void toggle_cell( const uint8_t x, const uint8_t y ); /* toggles the cell at the given coordinates. \
                                                            Returns the cursor X position */

void run( void );                           /* runs the simulation */
void __fastcall__ update( void );      /* updates the simulation */
void __fastcall__ update_asm( void );  /* updates the simulation */
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


/******************* STATIC GLOBAL VARIABLES ******************/

uint8_t Cells[ NB_COLUMNS ][ NB_LINES ];
uint8_t Cells_Future[ NB_COLUMNS ][ NB_LINES ];
uint8_t Cells_Initial[ NB_COLUMNS ][ NB_LINES ];

/******************** CODE ************************/


int main( int argc, char** argv )
{
    (void)argc;
    (void)argv;

    printf("PAUSE");
    cgetc();

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
    gotoxy( 1u, y-1u );
    printf("BYE BYE!");
    exit(0);
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
    cgetc();
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
    #define KEY_LEFT    'j'
    #define KEY_DOWN    'k'
    #define KEY_UP      'i'
    #define KEY_RIGHT   'l'

    uint8_t quit, x_cursor, y_cursor;
    uint8_t color_pixel;

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
        case 'd':
          quit = 1;
          gfx_pixel( color_pixel, x_cursor, y_cursor ); //clear cursor
          break;
        }
    }

    /* Cells was updated by the calls to toggle() */
    memcpy( Cells_Future, Cells, sizeof(Cells_Future) );
    memcpy( Cells_Initial, Cells, sizeof(Cells_Initial) );


}


void toggle_cell( const uint8_t x, const uint8_t y )
{
    char* cell;
    if( x == 0u || x >= NB_COLUMNS-1u || y == 0u || y >= NB_LINES-1u ) { return; }
    cell = &Cells[x][y];
    if( *cell == DEAD ) {
        *cell = ALIVE;
        gfx_pixel( CELL_COLOR, x, y );
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
    gotoxy( 0u, 23 );
    printf("Iteration:1     (R)eset (E)ditor (Q)uit");
    while( KeyPressed == NO_KEY)
    {
        /* Evolving the cells */
        update( );
        gotoxy(10u, 23);
        printf( itoa(nb_iterations++, str_nb_iteration, 10) );
        /* Testing key pressed */
        if( kbhit() ) {
            KeyPressed = cgetc();
            break;
        }
    }
}


void __fastcall__ update( void )
{
    uint8_t x, y;
    uint8_t* cell_neighbourhoud = (uint8_t*)Cells;         // cell_neighbourhoud = &Cells[0][0];
    uint8_t* cell_line = cell_neighbourhoud + NB_LINES + 1u; // cell_line = &Cells[1][1];
    uint8_t* cell_future = (uint8_t*)Cells_Future + NB_LINES + 1u; // cell_future = &Cells_Future[1][1];
    for( y = 1u; y < NB_LINES - 1u; ++y )
    {
        uint8_t* cell_curr = cell_line;
        uint8_t* cell_neighbourhoud_line = cell_neighbourhoud;
        uint8_t* cell_future_line = cell_future;
        for(  x = 1u; x < NB_COLUMNS - 1u; ++x)
        {
            uint8_t nb_neighbours = count_neighbours( cell_neighbourhoud_line );
            if( *cell_curr == ALIVE && \
                (nb_neighbours < 2u || nb_neighbours > 3u )
            ) {
                *cell_future_line = DEAD;
                gfx_pixel( BLACK, x, y );
            }
            else if( *cell_curr == DEAD && nb_neighbours == 3u ) {
                *cell_future_line = ALIVE;
                gfx_pixel( CELL_COLOR, x, y );
            }
            cell_curr               += NB_LINES;
            cell_neighbourhoud_line += NB_LINES;
            cell_future_line        += NB_LINES;
        }
        ++cell_line;
        ++cell_neighbourhoud;
        ++cell_future;
    }
    memcpy( Cells, Cells_Future, sizeof(Cells) );
}
