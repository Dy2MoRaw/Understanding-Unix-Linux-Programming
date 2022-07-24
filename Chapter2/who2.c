/* who2.c - suppresses empty records
 *          formats time nicely
 */
#include<stdio.h>
#include<stdlib.h>
#include<utmp.h>
#include<fcntl.h>
#include<unistd.h>
#include<time.h>

#define SHOWHOST	/* include remote machine on output */

void show_info(struct utmp*);
void showtime(long);	/* NEW */

int main()
{
	struct utmp current_record;	/* read info into here */
	int utmpfd;	/* read from this descriptor */
	int reclen = sizeof( current_record );

	if(( utmpfd = open( UTMP_FILE, O_RDONLY )) == -1 ){
		perror( UTMP_FILE );	/* UTMP_FILE is in utmp.h
					 * #define UTMP_FILE _PATH_UTMP
					 * _PATH_UTMP is in paths.h
					 * #define _PATH_UTMP "/var/run/utmp"
					 */
		exit(1);
	}
	
	while( read( utmpfd, &current_record, reclen ) == reclen )
		show_info( &current_record );
	close( utmpfd );
	return 0;
}

/* show info()
 * 	displays contents of the utmp struct in human readable form
 * 	*note* these sizes should not be hardwired
 * 	*NEW* displays nothing if record has no usr name
 */
void show_info( struct utmp *utbufp )
{
	if( utbufp->ut_type != USER_PROCESS )	/* NEW
						 *  in <utmp.h>, Values for ut_type field
						 * #define USER_PROCESS 7
						 * Normal process.
						 */
		return;
	printf( "%-8.8s", utbufp->ut_name );	/* logname, ut_user in new */
	printf( " " );
	printf( "%-8.8s", utbufp->ut_line );	/* tty */
	printf( " " );
	showtime( utbufp->ut_time );	/* NEW:display time */
	printf( " " );
#ifdef SHOWHOST
	printf( "(%s)", utbufp->ut_host );	/* host */
#endif
	printf( "\n" );
}

void showtime(long timeval)
/*
 * dsplays time in a format fit for human consuption
 * uses ctime to build a string then picks parts out of it
 * Note: %12.12s prints a string 12 chars wide and LIMITS
 * it to 12chars.
 */
{
	char *cp;	/* to hold address of time */
	cp = ctime( &timeval );	/* convert time to string
				 * string looks like
				 * Mon Feb 4 00:46:40 EST 1991
				 * 012345678912345.
				 */
	printf( "%12.12s", cp+4 );
}
