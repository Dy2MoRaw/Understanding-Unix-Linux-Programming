/* hello5.c
 * 	purpose: bounce a message back and forth across the screen
 * 	compile: cc hello5.c -lcurses -o hello5
 */
#include <curses.h>
#include <string.h>
#include <unistd.h>

#define LEFTEDGE 10
#define RIGHTEDGE 30
#define ROW 10

int main()
{
	char message[] = "Hello";
	char blank[] = "     ";
	int dir = +1;
	int pos = LEFTEDGE;

	initscr();
	clear();
	while(1){
		move(ROW, pos);
		addstr( message );
        	move( LINES-1, COLS-1 );
		refresh();
		sleep(1);
		move( ROW, pos );
		addstr( blank );
		pos += dir;
		if( pos >= RIGHTEDGE )
			dir = -1;
		if( pos <= LEFTEDGE )
			dir = +1;
	}
}
