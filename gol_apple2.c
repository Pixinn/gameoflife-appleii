/* Standard headers */
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
/* Specific headers */
#include <conio.h>

/******************* FUNCTION DEFINITIONS **************/

void init_display( void );              /* Inits displayed playfield */
void draw_cells( void );                /* Draws the actual cells */
void editor( void );                    /* lets the user draw some starting cells */
uint8_t toggle_cell( const uint8_t x, const uint8_t y ); /* toggles the cell at the given coordinates. \
                                                            Returns the cursor X position */

void run( void );                           /* runs the simulation */
void update( void );                        /* updates the simulation */
void count_neighbours( uint8_t* cell, uint8_t* count ); /* counts nb neighbours of the cell */

void quit( void );

/******************* CUSTOM TYPES AND VALUES DEFINITIONS ****************/

#define NB_LINES    23
#define NB_COLUMNS  40
const uint8_t JUMP_BEGINNING_NEXT_LINE = NB_LINES - 2u;

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
    char str_nb_iteration [5];
    uint16_t nb_iterations = 2u;
    KeyPressed = NO_KEY;
    cursor(0);

    gotoxy( 0u, NB_LINES );
    printf("Iteration:1     (R)eset (E)ditor (Q)uit");
    while( KeyPressed == NO_KEY)
    {
        /* Evolving the cells */
        update();
        /* Printing iterations */
        gotoxy(10u, NB_LINES);
        printf( itoa(nb_iterations++, str_nb_iteration, 10) );
        /* Testing key pressed */
        if( kbhit() ){
            KeyPressed = cgetc();
            break;
        }
    }
}


void update( void )
{
    uint8_t nb_neighbours;
    uint8_t x, y;
    for( y = 1u; y < NB_LINES-1; ++y )
    {
        for(  x = 1u; x < NB_COLUMNS-1; ++x)
        {
          register uint8_t nb_neighbours;
          uint8_t* cell = &Cells[x-1u][y-1u];
          count_neighbours( cell, &nb_neighbours );
          if( Cells[x][y] == ALIVE && \
              (nb_neighbours < 2u || nb_neighbours > 3u )
          ) {
                Cells_Future[x][y] = DEAD;
                cputcxy( x, y, SPRITE_DEAD );
            }
            else if( Cells[x][y] == DEAD && nb_neighbours == 3u ) {
                Cells_Future[x][y] = ALIVE;
                cputcxy( x, y, SPRITE_ALIVE );
            }
        }
    }
    memcpy( Cells, Cells_Future, sizeof(Cells) );
}


void count_neighbours( uint8_t* cell, uint8_t* count )
{
    *count = *cell++; *count += *cell++; *count += *cell;
    cell += JUMP_BEGINNING_NEXT_LINE;
    *count += *cell; cell+=2 ; *count += *cell;
    cell += JUMP_BEGINNING_NEXT_LINE;
    *count += *cell++; *count += *cell++; *count += *cell;
}
