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
uint8_t toggle_cell( const uint8_t x, const uint8_t y ); /* toggles the cell at the given coordinates. \
                                                            Returns the cursor X position */

void run( void );                           /* runs the simulation */
void update( void );                        /* updates the simulation */
uint8_t count_neighbours( const uint8_t x, const uint8_t y); /* counts nb neighbours of a cell */

void quit( void );

/******************* CUSTOM TYPES AND VALUES DEFINITIONS ****************/

#define NB_LINES    23
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
    
    /* go */
    draw_cells();
    run();
    
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



void draw_cells( void )
{
    #define KEY_LEFT    'j'
    #define KEY_DOWN    'k'
    #define KEY_UP      'i'
    #define KEY_RIGHT   'l'
    
    uint8_t quit = 0;
    uint8_t ch;
    
    uint8_t x = NB_COLUMNS >> 1u;
    uint8_t y = NB_LINES >> 1u;
    gotoxy(x,y);
    
    while ( quit == 0)
    {
        cursor(1);
		ch = cgetc();
		switch (ch) {
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
		case 'q':
			quit = 1;
			break;
		}
	}

    /* Cells was updated by the calls to toggle() */
    memcpy( Cells_Future, Cells, sizeof(Cells_Future) );

}


uint8_t toggle_cell( const uint8_t x, const uint8_t y )
{
    char* cell;   
    if( x == 0u || x >= NB_COLUMNS-1u || y == 0u || y >= NB_LINES-1u ) { return x; }    
    cell = &Cells[x][y];
    if( *cell == DEAD ) {
        *cell = ALIVE;
    } else {
        *cell = DEAD;
    }
    cputc( *cell ); /* shift the cursor */
    return wherex ();
}


void run( void  )
{
    char str_nb_iteration [5];
    uint16_t nb_iterations = 2u;
    char ch = '\0';    
    cursor(0);

    gotoxy( 0u, NB_LINES );
    printf("Iteration: 1");
    while( ch != 'q')
    {           
        update();
        gotoxy(11u, NB_LINES);
        printf( itoa(nb_iterations++, str_nb_iteration, 10) );
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
            nb_neighbours = count_neighbours(x,y);
            if( Cells[x][y] == ALIVE && \
                (nb_neighbours < 2u || nb_neighbours > 3u )
            ) {
                Cells_Future[x][y] = DEAD;
                cputcxy( x, y, DEAD );
            }
            else if( Cells[x][y] == DEAD && nb_neighbours == 3u ) {
                Cells_Future[x][y] = ALIVE;
                cputcxy( x, y, ALIVE );
            }
        }
    }
    memcpy( Cells, Cells_Future, sizeof(Cells) );
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

