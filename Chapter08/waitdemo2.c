/* waitdemo2.c - show how parent gets child status
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define DELAY 5

void child_code(), parent_code();

int main()
{
	int newpid;

	printf( "before:L mypid is %d\n", getpid() );
	if( ( newpid = fork() ) == -1 )
		perror("fork");
	else if( newpid == 0 )
		child_code( DELAY );
	else
		parent_code( newpid );
}

/* 
 * new process takes a nap and then exits
 */
void child_code( int delay )
{
	printf("child %d here. will sleep for %d seconds\n", getpid(), delay );
	sleep( delay );
	printf( "child done. about to exit\n" );
	exit(17);
}

/*
 * parent waits for child then prints a message
 */
void parent_code( int childpid )
{
	int wait_rv;		/* return value from wait() */
	int child_status;
	int high_8, low_7, bit_7;

	wait_rv = wait( &child_status );
	printf( "doen waiting for %d. Wait returned: %d\n", childpid, wait_rv );

	high_8 = child_status >> 8;
	low_7 = child_status & 0x7F;
	bit_7 = child_status & 0x80;
	printf( "status: exit=%d, sig=%d, core=%d\n", high_8, low_7, bit_7 );
}
