/* bounce3d.c
 * 	bounce a character (default is 'o') around the screen
 * 	defined by some parameters
 * 	NEW: set border, set paddle
 * 	     can use paddle to relect the ball, else end the game
 */

#include <curses.h>
#include <signal.h>
#include <unistd.h>
#include "bounce.h"

struct ppball the_ball;
int dir;
int paddle_y;

void set_up();
void wrap_up();
void set_ticker(int);
int bounce_or_lose(struct ppball*);

int main()
{
	int c;
	set_up();
	while(( c = getchar()) != 'Q' ){
		if( c == 's' ) dir = 1;
		else if( c == 'w' ) dir = -1;
	}
	wrap_up();
}

void set_up()
/* 
 * init structure and other stuff
 */
{
	void ball_move(int);
	void set_border();
	void set_paddle( int, int);
	void paddle_move();

	the_ball.y_pos = Y_INIT;
	the_ball.x_pos = X_INIT;
	the_ball.y_ttg = the_ball.y_ttm = Y_TTM;
	the_ball.x_ttg = the_ball.x_ttm = X_TTM;
	the_ball.y_dir = 1;
	the_ball.x_dir = 1;
	the_ball.symbol = DFL_SYMBOL;

	paddle_y = Y_INIT;

	initscr();
	noecho();
	crmode();

	signal( SIGINT, SIG_IGN );
	/* int mvaddch( int y, int x, const chtype ch ) */
	mvaddch( the_ball.y_pos, the_ball.x_pos, the_ball.symbol );
	refresh();

	signal( SIGALRM, ball_move );
	set_ticker( 1000 / TICKS_PER_SEC );	/* send millsecs pertick */

	set_border();
	set_paddle( paddle_y, paddle_y );
}

void wrap_up()
{
	set_ticker( 0 );
	endwin();
}

void set_border()
{
	move( TOP_ROW-1, LEFT_EDGE );
	hline( ACS_HLINE, RIGHT_EDGE - LEFT_EDGE + 1 );
	move( BOT_ROW+1, LEFT_EDGE );
	hline( ACS_HLINE, RIGHT_EDGE -LEFT_EDGE + 1 );
	move( TOP_ROW, LEFT_EDGE-1 );
	vline( ACS_VLINE, BOT_ROW - TOP_ROW + 1 );
	refresh();
}

void set_paddle( int pre_y, int cur_y )
{
	move( pre_y, RIGHT_EDGE+1 );
	vline( ' ', PADDLE_LEN );
	move( cur_y, RIGHT_EDGE+1 );
	vline( ACS_CKBOARD, PADDLE_LEN );	
	dir = 0;	/* reset */
	refresh();
}

void paddle_move()
{
	int old_y = paddle_y;
	paddle_y += dir;

	if( paddle_y < TOP_ROW )
		paddle_y = TOP_ROW;
	if( paddle_y + PADDLE_LEN > BOT_ROW )
		paddle_y = BOT_ROW - PADDLE_LEN + 1;

	set_paddle( old_y, paddle_y ); 
}

void ball_move( int signo )
{
	int y_cur, x_cur, moved;

	signal( SIGALRM, SIG_IGN );	/* dont get caught now */	
	y_cur = the_ball.y_pos;		/* old spot */
	x_cur = the_ball.x_pos;
	moved = 0;
	
	/* for every tick, ttg decrement 1, until equal to 0,
	 * then do the position movment. In another word, 
	 * every y_ttg number of ticks to move 1 y axis movement
	 */
	if( the_ball.y_ttm > 0 && (--the_ball.y_ttg == 0) ){
		the_ball.y_pos += the_ball.y_dir;	/* move */
		the_ball.y_ttg = the_ball.y_ttm;	
		moved = 1;
	}

	if( the_ball.x_ttm > 0 && (--the_ball.x_ttg == 0) ){
		the_ball.x_pos += the_ball.x_dir;
		the_ball.x_ttg = the_ball.x_ttm;
		moved = 1;
	}

	if( moved ){
		mvaddch( y_cur, x_cur, BLANK );
//		mvaddch( y_cur, x_cur, BLANK );
		mvaddch( the_ball.y_pos, the_ball.x_pos, the_ball.symbol );
		bounce_or_lose( &the_ball );
		move( LINES-1, COLS-1 );
		refresh();
	}

	paddle_move();
	signal( SIGALRM, ball_move );		/* for unreliable system */
}

int bounce_or_lose( struct ppball *bp )
{
	int return_val = 0;

	if( bp->y_pos == TOP_ROW ){
		bp->y_dir = 1;
		return_val = 1;
	}else if( bp->y_pos == BOT_ROW ){
		bp->y_dir = -1;
		return_val = 1;
	}	

	if( bp->x_pos == LEFT_EDGE ){
		bp->x_dir = 1;
		return_val = 1;
	}else if(( bp->x_pos == RIGHT_EDGE ) && 
			( bp->y_pos >= paddle_y ) &&
			( bp->y_pos <= paddle_y + PADDLE_LEN )){
		bp->x_dir = -1;
		return_val = 1;
	}else if( bp->x_pos > RIGHT_EDGE ){
		move((TOP_ROW+BOT_ROW)/2, (LEFT_EDGE+RIGHT_EDGE)/2);
		addstr( "GAME OVER!" );
		refresh();
		wrap_up();
	}

	return return_val;
}

