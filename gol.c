#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <locale.h>
#include <curses.h>
#include <signal.h>
#include <string.h>
static void finish(int sig);

void draw_cells( void );                    /* lets the user draw some starting cells */
void toggle_cell( uint8_t x, uint8_t y );   /* toggles the cell at the given coordinates */

void run( void );                           /* runs the simulation */
void update( void );                        /* updates the simulation */
uint8_t count_neighbours( const uint8_t x, const uint8_t y); /* counts nb neighbours of a cell */

#define NB_LINES    24
#define NB_COLUMNS  40

#define ALIVE       '0'
#define DEAD        ' '

/* state of the cells */
char Cells[ NB_COLUMNS ][ NB_LINES ];
char Cells_Future[ NB_COLUMNS ][ NB_LINES ];

int main( void )
{
    uint8_t i;

    signal(SIGINT, finish); /* arrange interrupts to terminate */

    /* Initial state */
    memset( Cells, DEAD, sizeof(Cells) );

    /* Init ncurses */
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    nonl();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    /* init playfield */
    for( i = 0u; i < NB_COLUMNS; ++i ) {
        mvaddch(0u,i,'+');
    }
    for( i = 0u; i < NB_COLUMNS; ++i ) {
    mvaddch(NB_LINES-1u,i,'+');
    }
    for( i = 0u; i < NB_LINES; ++i ) {
        mvaddch(i,0u,'+');
    }
    for( i = 0u; i < NB_LINES; ++i ) {
        mvaddch(i,NB_COLUMNS-1u,'+');
    }
    refresh();

    /* go */
    draw_cells();
    run();

    endwin();
    return 0;
}



void draw_cells( void )
{
    int quit = 0;

    int ch;
    int y = NB_COLUMNS / 2;
    int x = NB_LINES / 2;

    move(x,y);
    refresh();

    while ( quit == 0)
    {
		getyx(stdscr, y, x);
		ch = getch();
		switch (ch) {
		case KEY_LEFT:
            if( x > 0 ) { move(y, --x); }
			break;
		case KEY_DOWN:
            if( y < NB_LINES-1 ) { move(++y, x); }
			break;
		case KEY_UP:
            if( y > 0 ) { move(--y, x); }
			break;
		case KEY_RIGHT:
            if( x < NB_COLUMNS-1 ) {	move(y, ++x); }
			break;
		case ' ':
            toggle_cell(x,y);
            break;
		case 'q':
			quit = 1;
			break;
		}

		refresh();
	}

    /* Cells was updated by the calls to toggle() */
    memcpy( Cells_Future, Cells, sizeof(Cells_Future) );

}


void toggle_cell( const uint8_t x, const uint8_t y )
{
    if( x == 0u || x >= NB_COLUMNS-1 || y == 0u || y >= NB_LINES-1 ) { return; }
    char* cell = &Cells[x][y];
    if( *cell == DEAD ) {
        *cell = ALIVE;
    } else {
        *cell = DEAD;
    }
    mvaddch(y,x,*cell);
    refresh();
}


void run( void  )
{
    #define PERIOD_MS 500000u
    curs_set(0);
    char ch = '\0';
    while( ch != 'q')
    {
        ch = getch();
        update();
        usleep( PERIOD_MS );
    }
}


void update( void )
{
    uint8_t x, y;
    for( y = 1u; y < NB_LINES-1; ++y )
    {
        for(  x = 1u; x < NB_COLUMNS-1; ++x)
        {
            uint8_t nb_neighbours = count_neighbours(x,y);
            if( Cells[x][y] == ALIVE && \
                (nb_neighbours < 2u || nb_neighbours > 3u )
            ) {
                Cells_Future[x][y] = DEAD;
                mvaddch(y,x,DEAD);
            }
            else if( Cells[x][y] == DEAD && nb_neighbours == 3u ) {
                Cells_Future[x][y] = ALIVE;
                mvaddch(y,x,ALIVE);
            }
        }
    }
    memcpy( Cells, Cells_Future, sizeof(Cells) );
    refresh();
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

static void finish(int sig)
{
    (void) sig;
    endwin();

    /* do your non-curses wrapup here */

    exit(0);
}
