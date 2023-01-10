/* more02.c 
 * 	read and printf 24 lines then pause for
 * 	a few special commands
 * 	feature of version 0.2: reads from /dev/tty for commands
 */

#include <stdio.h>
#include <stdlib.h>

#define PAGELINE 24
#define COUNT 1024

void do_more(FILE *fp)
/*
 * read PAGELINE lines, then call see_more() for further instructions
 */
{
	char line[COUNT];
	int num_of_lines = 0;
	int see_more(FILE *), reply;
	FILE *fp_tty;

	fp_tty = fopen("/dev/tty", "r");	/* version 0.2 */
	if(fp_tty == NULL)
		exit(1);

	while(fgets(line, COUNT, fp)){
		if(num_of_lines == PAGELINE){
			reply = see_more(fp_tty);	/* version 0.2 */
			if(reply == 0)
				break;
			num_of_lines -= reply;
		}
		if(fputs(line, stdout) == EOF)
			exit(1);
		num_of_lines++;
	}
}

int see_more(FILE *fp)
/*
 * pritn message, wait for response, return # of lines to
 * advance q means no, space means yes, CR means one line
 */
{
	int c;
	printf("\033[7m more? \033[m");
	while((c = getc(fp)) != EOF)	/* version 0.2 */
	{
		if(c == 'q' )
			return 0;
		if(c == ' ' )
			return PAGELINE;
		if(c == '\n')
			return 1;
	}
	return 0;
}

int main( int argc, char *argv[] )
{
	FILE *fp;
	if(argc == 1)
		do_more(stdin);
	else
		while(--argc)
			if((fp = fopen(*++argv, "r")) != NULL )
			{
				do_more(fp);
				fclose(fp);
			}
			else
				exit(1);
	return 0;
}
