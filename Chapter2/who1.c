/* who1.c - a first version of the who program
 *          open, read UTMP file, and show results
 */
#include<stdio.h>
#include<stdlib.h>
#include<utmp.h>
#include<fcntl.h>
#include<unistd.h>

#define SHOWHOST	/* include remote machine on output */

void show_info(struct utmp*);

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
 */
void show_info( struct utmp *utbufp )
{
	printf( "%-8.8s", utbufp->ut_name );	/* logname, ut_user in new */
	printf( " " );
	printf( "%-8.8s", utbufp->ut_line );	/* tty */
	printf( " " );
	printf( "%10d", utbufp->ut_time );	/* login time, ut_tv.tv_sec in new */
	printf( " " );
#ifdef SHOWHOST
	printf( "(%s)", utbufp->ut_host );	/* host */
#endif
	printf( "\n" );
}
