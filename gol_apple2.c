/* Standard headers */
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
/* Specific headers */
#include <conio.h>

/******************* FUNCTION DEFINITIONS **************/

void init_display( void );                  /* Inits displayed playfield */
void draw_cells( void );                    /* lets the user draw some starting cells */
void toggle_cell( const uint8_t x, const uint8_t y );   /* toggles the cell at the given coordinates */

void run( void );                           /* runs the simulation */
void update( void );                        /* updates the simulation */
uint8_t count_neighbours( const uint8_t x, const uint8_t y); /* counts nb neighbours of a cell */

void quit( void );

/******************* CUSTOM TYPES AND VALUES DEFINITIONS ****************/

#define NB_LINES    20
#define NB_COLUMNS  40

#define ALIVE       '0'
#define DEAD        ' '



/******************* STATIC GLOBAL VARIABLES ******************/

char Cells[ NB_COLUMNS ][ NB_LINES ];
char Cells_Future[ NB_COLUMNS ][ NB_LINES ];


/******************** CODE ************************/


int main( int argc, char** argv )
{
    (void)argc;
    (void)argv;
    
    printf("HELLO");
    
    /* Initial state */
    memset( Cells, DEAD, sizeof(Cells) );
    
    
    init_display();
    
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
    /*for( i = 0u; i < NB_COLUMNS; ++i ) {
        cputcxy (i,0u,'+');
    }
    for( i = 0u; i < NB_COLUMNS; ++i ) {
        cputcxy (i,NB_LINES-1u,'+');
    }
    for( i = 0u; i < NB_LINES; ++i ) {
        cputcxy (0u,i,'+');
    }
    for( i = 0u; i < NB_LINES; ++i ) {
        cputcxy (NB_COLUMNS-1u,i,'+');
    }*/
}



void draw_cells( void )
{
    /* TBD */

    /* Cells was updated by the calls to toggle() */
    memcpy( Cells_Future, Cells, sizeof(Cells_Future) );

}


void toggle_cell( const uint8_t x, const uint8_t y )
{
    char* cell;
    if( x == 0u || x >= NB_COLUMNS-1 || y == 0u || y >= NB_LINES-1 ) { return; }
    cell = &Cells[x][y];
    if( *cell == DEAD ) {
        *cell = ALIVE;
    } else {
        *cell = DEAD;
    }
    /*mvaddch(y,x,*cell);
    refresh();
    */
}


void run( void  )
{
    #define PERIOD_MS 500000u
    /* TBD */
}


void update( void )
{
  /* TBD */
}


uint8_t count_neighbours( const uint8_t x, const uint8_t y)
{
    uint8_t count = 0;
    if( Cells[x-1][y-1] == ALIVE ) { ++count; }
    if( Cells[x-1][y] == ALIVE ) { ++count; }
    if( Cells[x-1][y+1] == ALIVE ) { ++count; }
    if( Cells[x][y-1] == ALIVE ) { ++count; }
    if( Cells[x][y+1] == ALIVE ) { ++count; }
    if( Cells[x+1][y-1] == ALIVE ) { ++count; }
    if( Cells[x+1][y] == ALIVE ) { ++count; }
    if( Cells[x+1][y+1] == ALIVE ) { ++count; }
    return count;
}

