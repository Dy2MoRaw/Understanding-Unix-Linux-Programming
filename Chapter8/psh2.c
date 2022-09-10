/* prompting shell version 2
 * 	Sloves the 'one-shot' problem of version 1
 * 	Prompts for the command and its arguments.
 * 	Builds the argument vector for the call to execvp.
 * 	Uses execvp(), and never returns.
 */

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>

#define MAXARGS 20	/* cmdline args */
#define ARGLEN 100	/* token length */

int execute(char**);

int main()
{
	char *arglist[MAXARGS + 1];
	int numargs;	/* index into array */
	char argbuf[ARGLEN];	/* read stuff here */
	char *makestring();	/* malloc etc */

	numargs = 0;

	while( numargs < MAXARGS )
	{
		printf( "Arg[%d]? ", numargs );
		/* parsing stops if a newline character is found,
		 * and return nullptr on failure.
		 */
		if( fgets( argbuf, ARGLEN, stdin ) && *argbuf != '\n' )
			arglist[numargs++] = makestring(argbuf);
		else
		{
			if( numargs > 0 ){			/* any args? */
				arglist[numargs] = NULL;	/* close list */
				execute( arglist );		/* do it */
				numargs = 0;			/* and reset */
			}
		}
	}
	return 0;
}

int execute( char *arglist[] )
/*
 * use fork and execvp to do it
 */
{
	int pid, exitstatus;		/* of child */
	pid = fork();			/* make new process */
	switch( pid ){
		case -1:
			perror( "fork failed");
			exit(1);
		case 0:
			execvp( arglist[0], arglist );
			perror( "execvp failed" );
			exit(1);
		default:
			while( wait( &exitstatus ) != pid )
				;
			printf( "child exited with status %d, %d\n",
					exitstatus>>8, exitstatus&0377 );
	}
}

char *makestring( char *buf )
/*
 * trim off newline and create storage for the string
 */
{
	char *cp;

	buf[strlen(buf) - 1] = '\0';	/* trim newline */
	cp = malloc( strlen(buf) + 1 );	/* get memory */
	if( cp == NULL ){
		fprintf( stderr, "no memory\n" );
		exit(1);
	}
	strcpy( cp, buf );
	return cp;
}
