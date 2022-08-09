/* play_again4.c
 * 	purpose :  ask if user wants another transaction
 * 	method  :  set tty into char-by-char mode, no-echo mode
 * 		   set tty into no-delay mode
 * 		   read char, return result
 * 		   resets termial modes on SIGINT, ingores SIGQUIT
 * 	returns :  0 => yes, 1 => no, 2 => timeout
 * 	better  :  reset termial mode on interrupt
 */
#include<stdio.h>
#include<stdlib.h>
#include<termios.h>
#include<fcntl.h>
#include<ctype.h>		/* tolower */
#include<unistd.h>		/* sleep */
#include<string.h>		/* strchr */
#include<signal.h>		

#define QUESTION "Do you want another transaction"
#define TRIES 3	/* max tries */
#define SLEEPTIME 2	/* time per try */
#define BEEP	putchar('\a')	/* alert user */

int get_response( char *,int );
void set_cr_noecho_mode();
void tty_mode(int);
void set_nodelay_mode();
char get_ok_char();

int main()
{
	int response;
	void ctrl_c_handler(int);

	tty_mode(0);		/* save current mode */
	set_cr_noecho_mode();	/* set -icanon, -echo */
	set_nodelay_mode();	/* noiput => EOF */
	signal( SIGINT, ctrl_c_handler );	/* handle INT */
	signal( SIGQUIT, SIG_IGN );		/* ignore QUIT signals */
	response = get_response( QUESTION, TRIES );
	tty_mode(1);
	return response;
}

int get_response( char *question, int maxtries )
/*
 * purpose: ask a question and wait for a y/n answer
 * method : use getchar and ignore non y/n answers
 * returns: 0 => yes, 1 => no
 */
{
	int input;
	printf( "%s (y/n)? ", question );
	fflush(stdout);		/* force output:
				 * The driver buffer output until it 
				 * receives a newline or until the program
				 * tries to read from the terminal.
				 */
	while(1){
		sleep(SLEEPTIME);	/* wait a bit */
		input = tolower(get_ok_char());
		if( input == 'y' )
			return 0;
		if( input == 'n' )
			return 1;
		if( maxtries-- == 0 )		/* out a time ? */
			return 2;
		BEEP;
	}
}

/*
 * skip over non-legal chars and return y,Y,n,N or EOF
 */
char get_ok_char()
{
	int c;
	while(( c = getchar() ) != EOF && strchr( "yYnN", c ) == NULL )
		;
	return c;
}

void set_nodelay_mode()
/*
 * purpose: put file descriptor 0 into no-delay mode
 * method : use fcntl to set bits
 * notes  : tcsetattr() will do something similar, but it is complicated
 */
{
	int termflags;
	termflags = fcntl( 0, F_GETFL );	/* read curr. setting */
	termflags |= O_NONBLOCK;		/* flip on nodelay bit */
	fcntl( 0, F_SETFL, termflags );		/* and install */
}

void set_cr_noecho_mode()
/*
 * purpose: put file descriptor 0 (i.e. stdin) into chr-by-chr mode
 * method : use bits in termios
 */
{
	struct termios ttystate;

	tcgetattr( 0, &ttystate );	/* read curr setting */
	ttystate.c_lflag &= ~ICANON;	/* no buffering */
	ttystate.c_lflag &= ~ECHO;	/* no echo either */
	ttystate.c_cc[VMIN] = 1;	/* get 1 char at a time */
	tcsetattr( 0, TCSANOW, &ttystate );	/* install settings */
}

/* how == 0 => save current mode, how == 1 => restore mode */
/* this version handles termios and fcntl flags */
void tty_mode(int how)
{
	static struct termios original_mode;
	static int original_flags;
	static int stored = 0;

	if( how == 0 ){
		tcgetattr( 0, &original_mode );
		original_flags = fcntl( 0, F_GETFL );
		stored = 1;
	}
	else{
		tcsetattr( 0, TCSANOW, &original_mode );
		fcntl( 0, F_SETFL, original_flags );
	}
}

void ctrl_c_handler( int signum )
/*
 * purpose: called if SIGINT is detected
 * action : reset tty and scram
 */
{
	tty_mode(1);
	exit(1);
}
