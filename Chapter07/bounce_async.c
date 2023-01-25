/* bounce_async.c
 * 	purpose animation with user control, using O_ASYNC on fd
 * 	note set_ticker() sends SIGALRM, handler does animation
 * 		keyboard send ISGIO, main only calls pause()
 * 	compile gcc bounce_async.c set_ticker.c -lcurses -o bounce_async
 */
#include<stdio.h>
#include<curses.h>
#include<signal.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

#define MESSAGE "hello"
#define BLANK   "     "

int row=10;
int col=0;
int dir=1;
int delay=200;
int flag=0;

int set_ticker(int);

void on_input(int signum)
{
	int c=getch();
	if(c=='Q' || c==EOF)
		flag=1;
	else if(c==' ')
		dir=-dir;
}

void on_alarm(int signum)
{
	signal(SIGALRM, on_alarm);
	mvaddstr(row, col, BLANK);
	col+=dir;
	mvaddstr(row, col, MESSAGE);
	refresh();

	if(dir==-1 && col <=0)
		dir=1;
	else if(dir==1 && col+strlen(MESSAGE)>=COLS)
		dir=-1;
}

void enable_kbd_signals()
{
	int fd_flags;

	fcntl(0, F_SETOWN, getpid());
	fd_flags=fcntl(0, F_GETFL);
	fcntl(0, F_SETFL, (fd_flags|O_ASYNC));
}

int main()
{	
	initscr();
	crmode();
	noecho();
	clear();

	signal(SIGIO, on_input);
	enable_kbd_signals();
	signal(SIGALRM, on_alarm);
	set_ticker(delay);

	move(row, col);
	addstr(MESSAGE);

	while(!flag)
		pause();
	endwin();
}
