#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

/******************* FUNCTION DEFINITIONS **************/

void draw_cells( void );                    /* lets the user draw some starting cells */
void toggle_cell( const uint8_t x, const uint8_t y );   /* toggles the cell at the given coordinates */

void run( void );                           /* runs the simulation */
void update( void );                        /* updates the simulation */
uint8_t count_neighbours( const uint8_t x, const uint8_t y); /* counts nb neighbours of a cell */


/******************* CUSTOM TYPES AND VALUES DEFINITIONS ****************/

#define NB_LINES    24
#define NB_COLUMNS  40

#define ALIVE       '0'
#define DEAD        ' '



/******************* STATIC GLOBAL VARIABLES ******************/

char Cells[ NB_COLUMNS ][ NB_LINES ];
char Cells_Future[ NB_COLUMNS ][ NB_LINES ];


/******************** CODE ************************/


int main( void )
{
    /* TBD */
    
    return 0;
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

