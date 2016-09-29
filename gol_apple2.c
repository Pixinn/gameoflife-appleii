/* Standard headers */
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
/* Specific headers */
#include <conio.h>

#include "gfx.h"

/******************* FUNCTION DEFINITIONS **************/

void __fastcall__ init_asm( uint8_t* p_cell, uint8_t* p_cells_future );  /* Inits the variables used in the ASM scope */

void init_display( void );              /* Inits displayed playfield */
void draw_cells( void );                /* Draws the actual cells */
void editor( void );                    /* lets the user draw some starting cells */
uint8_t toggle_cell( const uint8_t x, const uint8_t y ); /* toggles the cell at the given coordinates. \
                                                            Returns the cursor X position */

void run( void );                           /* runs the simulation */
void __fastcall__ update( void );      /* updates the simulation */
void __fastcall__ update_asm( void );  /* updates the simulation */
uint8_t __fastcall__  count_neighbours( uint8_t* cell );                /* counts nb neighbours of the cell */


void quit( void );

/******************* CUSTOM TYPES AND VALUES DEFINITIONS ****************/

#define NB_LINES    23u
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


/******************* STATIC GLOBAL VARIABLES ******************/

uint8_t Cells[ NB_COLUMNS ][ NB_LINES ];
uint8_t Cells_Future[ NB_COLUMNS ][ NB_LINES ];
uint8_t Cells_Initial[ NB_COLUMNS ][ NB_LINES ];

/******************** CODE ************************/


int main( int argc, char** argv )
{
    (void)argc;
    (void)argv;

    //+ DEBUG
    //gotoxy(0,0);
    //printf("Cells:0x%x - Cells_future:0x%x",(uint16_t)(&Cells[0][0]),(uint16_t)(&Cells_Future[0][0]));
    //cgetc();
    //- DEBUG

    init_asm( (uint8_t*)Cells, (uint8_t*)Cells_Future );

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
                mode_text();
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
    clrscr();
    chlinexy (1u, 0u, NB_COLUMNS-2u );
    chlinexy (1u, NB_LINES-1u, NB_COLUMNS-2u );
    cvlinexy (0u, 1u, NB_LINES-1u );
    cvlinexy (NB_COLUMNS-1u, 1u, NB_LINES-1u );
    cputcxy ( 0u,  0u, '+' );
    cputcxy ( 0u , NB_LINES-1u,'+');
    cputcxy ( NB_COLUMNS-1u, 0u,'+');
    cputcxy ( NB_COLUMNS-1u, NB_LINES-1u,'+');
}


void draw_cells( void ) {
    uint8_t x, y;
    for( x = 1u; x < NB_COLUMNS - 1u; ++x )
    {
        for( y = 1u; y < NB_LINES - 1u; ++y )
        {
            if( Cells[x][y] == ALIVE ) {
                cputcxy ( x, y, SPRITE_ALIVE );
            } else {
                cputcxy ( x, y, SPRITE_DEAD );
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

    uint8_t quit, x, y;

    gotoxy( 0u, NB_LINES );
    printf("EDITOR   (D)one");

    x = NB_COLUMNS >> 1u;
    y = NB_LINES >> 1u;
    gotoxy(x,y);

    quit = 0;
    while ( quit == 0)
    {
        cursor(1);
        KeyPressed = cgetc();
        switch (KeyPressed) {
        case KEY_LEFT:
                if( x > 1u ) { gotoxy( --x, y ); }
          break;
        case KEY_DOWN:
                if( y < NB_LINES-2u ) { gotoxy( x, ++y ); }
          break;
        case KEY_UP:
                if( y > 1u ) { gotoxy( x, --y ); }
          break;
        case KEY_RIGHT:
                if( x < NB_COLUMNS-2u ) {	gotoxy( ++x, y ); }
          break;
        case ' ':
                x = toggle_cell( x, y );
                break;
        case 'd':
          quit = 1;
          break;
        }
	}

    /* Cells was updated by the calls to toggle() */
    memcpy( Cells_Future, Cells, sizeof(Cells_Future) );
    memcpy( Cells_Initial, Cells, sizeof(Cells_Initial) );
}


uint8_t toggle_cell( const uint8_t x, const uint8_t y )
{
    char* cell;
    if( x == 0u || x >= NB_COLUMNS-1u || y == 0u || y >= NB_LINES-1u ) { return x; }
    cell = &Cells[x][y];
    if( *cell == DEAD ) {
        *cell = ALIVE;
        cputc( SPRITE_ALIVE );
    } else {
        *cell = DEAD;
        cputc( SPRITE_DEAD );
    }
    return wherex ();
}


void run( void  )
{
    uint8_t i;
    char str_nb_iteration [5];
    uint16_t nb_iterations = 2u;
    KeyPressed = NO_KEY;

    gfx_init( LOWRES, SPLIT );
    gfx_fill( BLACK );
    cursor(0);
    gotoxy( 0u, NB_LINES );
    printf("Iteration:1     (R)eset (E)ditor (Q)uit");
    while( KeyPressed == NO_KEY)
    {
        /* Evolving the cells */
        update( );
        /* Printing iterations */
        gotoxy(10u, NB_LINES);
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
                //cputcxy( x, y, SPRITE_DEAD );
                gfx_pixel( BLACK, x, 2*y );
            }
            else if( *cell_curr == DEAD && nb_neighbours == 3u ) {
                *cell_future_line = ALIVE;
                gfx_pixel( WHITE, x, 2*y );
                //cputcxy( x, y, SPRITE_ALIVE );
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
