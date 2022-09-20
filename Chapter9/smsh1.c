/* smsh1.c - small-shell version 1
 * 		first really useful version after prompting shell
 * 		this one parses the command line tnto strings
 * 		uses fork, exec, wait, and ignores signals
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "smsh.h"

#define DFL_PROMPT "> "

int main()
{
	char *cmdline, *prompt, **arglist;
	int result;
	void setup();

	prompt = DFL_PROMPT;
	setup();

	while(( cmdline = next_cmd( prompt, stdin )) != NULL ){
		if(( arglist = splitline( cmdline )) != NULL ){
			result = execute( arglist );
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
	signal( SIGINT, SIG_IGN );
	signal( SIGQUIT, SIG_IGN );
}
