/* more01.c 
 * 	read and printf 24 lines then pause for
 * 	a few special commands
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
	int see_more(), reply;

	while(fgets(line, COUNT, fp)){
		if(num_of_lines == PAGELINE){
			reply = see_more();
			if(reply == 0)
				break;
			num_of_lines -= reply;
		}
		if(fputs(line, stdout) == EOF)
			exit(1);
		num_of_lines++;
	}
}

int see_more()
/*
 * pritn message, wait for response, return # of lines to
 * advance q means no, space means yes, CR means one line
 */
{
	int c;
	printf("\033[7m more? \033[m");
	while((c = getchar()) != EOF)
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
