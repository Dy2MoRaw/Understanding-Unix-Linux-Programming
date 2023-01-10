/* execute.c - code use by small to execute commands */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "varlib.h"

int execute( char *argv[] )
/*
 * purpose : run a program passing it arguments
 * returns : status returned via wait, or -1 onerror
 * error   : -1 on fork() or wait() errors
 */
{
	int pid;
	int child_info = -1;
	extern char **environ;

	if( argv[0] == NULL )	
		return 0;
	if(( pid = fork() ) == -1 )
		perror("fork");
	else if( pid == 0 ){
		environ = VLtable2environ();	/* new line */
		signal( SIGINT, SIG_DFL );
		signal( SIGQUIT, SIG_DFL );
		execvp( argv[0], argv );
		perror( "cannot execute command" );
		exit(1);
	}
	else{
		if( wait(&child_info) == -1 )
			perror("wait");
	}
	return child_info;
}

