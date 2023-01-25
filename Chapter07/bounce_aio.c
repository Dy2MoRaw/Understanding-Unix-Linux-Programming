/* bounce_aio.c
 * 	purpose animation with user control, using aio_read
 * 	note set_ticker() sends SIGALRM, handler does animation
 * 		keyboard send ISGIO, main only calls pause()
 * 	compile gcc bounce_aio.c set_ticker.c -lcurses -o bounce_aio
 */
#include<stdio.h>
#include<curses.h>
#include<signal.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<aio.h>

#define MESSAGE "hello"
#define BLANK   "     "

int row=10;
int col=0;
int dir=1;
int delay=200;
int flag=0;

struct aiocb kbcbuf;

int set_ticker(int);

void on_input(int signum)
{
	int c;
	char *cp=(char *)kbcbuf.aio_buf;

	if(aio_error(&kbcbuf) != 0)
		perror("reading failed");
	else if(aio_return(&kbcbuf) == 1)
	{
		c=*cp;
		if(c=='Q' || c==EOF)
			flag=1;
		else if(c==' ')
			dir=-dir;
	}

	aio_read(&kbcbuf);
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

void setup_aio_buffer()
{
	static char input[1];
	
	/* describe what to read */
	kbcbuf.aio_fildes = 0;	/* standard input */
	kbcbuf.aio_buf = input;	/* buffer */
	kbcbuf.aio_nbytes = 1;	/* number to read */
	kbcbuf.aio_offset = 0;	/* offset in file */

	/* describe what to do when read is ready */
	kbcbuf.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
	kbcbuf.aio_sigevent.sigev_signo = SIGIO;
}

int main()
{	
	initscr();
	crmode();
	noecho();
	clear();

	signal(SIGIO, on_input);
	setup_aio_buffer();
	aio_read(&kbcbuf);

	signal(SIGALRM, on_alarm);
	set_ticker(delay);

	move(row, col);
	addstr(MESSAGE);

	while(!flag)
		pause();
	endwin();
}
