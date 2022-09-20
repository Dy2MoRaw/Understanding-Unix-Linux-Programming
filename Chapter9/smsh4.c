/* smsh2.c - small-shell version 2
 * 		small shell that supports line parsing
 * 		and if..then..else.fi logic (by calling process())
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "smsh.h"
#include "varlib.h"

#define DFL_PROMPT "> "
//
int main()
{
	char *cmdline, *prompt, **arglist;
	int result;
	void setup();

	prompt = DFL_PROMPT;
	setup();

	while(( cmdline = next_cmd( prompt, stdin )) != NULL ){
		if(( arglist = splitline( cmdline )) != NULL ){
			result = process( arglist );
			freelist( arglist );
		}
		free( cmdline );
	}
	return 0;
}

void setup()
/*
 * purpose: initialize shell
 * returns: noting. calls fatal() if trouble
 */
{
	extern char **environ;
	VLenviron2table(environ);
	signal( SIGINT, SIG_IGN );
	signal( SIGQUIT, SIG_IGN );
}
