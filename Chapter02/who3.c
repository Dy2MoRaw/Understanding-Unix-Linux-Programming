/* who3.c - who with buffered reads
 *          formats time nicely
 *          surpresses empty records
 *          buffers input (using utmplib functions)
 */
#include<stdio.h>
#include<stdlib.h>
#include<utmp.h>
#include<fcntl.h>
#include<unistd.h>
#include<time.h>
#include<sys/types.h>

#define SHOWHOST	/* include remote machine on output */
#define BufSize 16	/* NEW: the records number of buf array */
#define BUFFERSIZE (BufSize*(sizeof(struct utmp)))	/* NEW: creat an array that holds 16 utmp struct */
#define nullptr (( struct utmp * )NULL )

static char utmpbuf[BUFFERSIZE];	/* NEW: buffer array for storage */
static int num_recs;	/* number of records stored in buffer */
static int cur_rec;	/* current record position */
static int utmpfd = -1;

void show_info(struct utmp*);
void show_time(long);	
int utmp_reload();	/* NEW: reload records into buffer */
struct utmp *utmp_next();	/* NEW: get the next utmp from buffer */

int main()
{
	struct utmp *utmp_cur;	/* read info into here */
	
	if(( utmpfd = open( UTMP_FILE, O_RDONLY )) == -1 ){
		perror( UTMP_FILE );	/* UTMP_FILE is in utmp.h
					 * #define UTMP_FILE _PATH_UTMP
					 * _PATH_UTMP is in paths.h
					 * #define _PATH_UTMP "/var/run/utmp"
					 */
		exit(1);
	}
	
	while(( utmp_cur = utmp_next()) != nullptr )
		show_info( utmp_cur );
//	close( utmpfd );
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
	show_time( utbufp->ut_time );	/* NEW:display time */
	printf( " " );
#ifdef SHOWHOST
	printf( "(%s)", utbufp->ut_host );	/* host */
#endif
	printf( "\n" );
}

void show_time(long timeval)
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

int utmp_reload()
/*
 * read next bunch of records into buffer
 */
{
	num_recs = read( utmpfd, utmpbuf, BUFFERSIZE )/(sizeof(struct utmp));
	/* 
	 * On success, the number of bytes read is returnd.
	 * zero indicates end of file
	 */

	cur_rec = 0;
	return num_recs;
}

struct utmp *utmp_next()
/*
 * get one utmp at a time from the buffer array
 */
{
	struct utmp *ret;
	if( utmpfd == -1 )	/* open faild */
		return nullptr;
	if( cur_rec == num_recs && utmp_reload() == 0 )
	/*
	 * only when cur_rec == num_recs,
	 * or say the record goes into the end of buffer array.
	 * utmp_reload will run once again.
	 */
		return nullptr;

	ret = ( struct utmp* ) &utmpbuf[cur_rec * (sizeof(struct utmp))];
	cur_rec++;
	return ret;
}
