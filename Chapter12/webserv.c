/* weberv.c - a minimal web server (version 0.2)
 * 	usage: ws portnumber
 *   features: support the GET command only
 *   	       runs in the current directory
 *   	       foks a new child to handle each request
 *   	       has MAJOR security holes, for demo purposes only
 *	       has many other weaknesses, but is a good start
 *	build: cc webserv.c socklib.c -o webserv
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <string.h>

void read_till_crnl( FILE *);
void process_rq( char *, int );
void header( FILE *, char * );
void cannot_do( int );
void do_404( char *, int );
int isadir( char * );
int not_exist( char * );
void do_ls( char *, int );
char *file_type( char * );
int ends_in_cgi( char * );
void do_exec( char *, int );
void do_cat( char *, int );
int make_server_socket( int );

int main( int argc, char *argv[] )
{
	int sockfd, sockfd_new;
	FILE *fpin;
	char request[BUFSIZ];

	if( argc == 1 ){
		fprintf( stderr, "usage: ws portnum\n" );
		exit(1);
	}
	sockfd = make_server_socket( atoi(argv[1]) );
	if( sockfd == -1 )
		exit(2);
	
	while(1){
		/* take a call, and buffer it */
		sockfd_new = accept( sockfd, NULL, NULL );
		fpin = fdopen( sockfd_new, "r" );
		
		/* read request */
		fgets( request, BUFSIZ, fpin );
		printf( "got a call: request = %s", request );
		read_till_crnl( fpin );

		process_rq( request, sockfd_new );

		fclose( fpin );
	}
}

/* ------------------------------------------------------------ *
 * read_till_crnl( FILE * )
 * skip over all request info until a CRNL is seen
 * ------------------------------------------------------------ */
void read_till_crnl( FILE *fp )
{
	char buf[BUFSIZ];
	while( fgets( buf, BUFSIZ, fp ) != NULL && strcmp( buf, "\r\n" ) != 0 );
}

/* ------------------------------------------------------------- *
 * process_rq( char *rq, int fd )
 * do what the request asks for and write reply to fd
 * handles request in a new process
 * rq is HTTP command: GET /foo/bar.html HTTP/1.0
 * -------------------------------------------------------------- */
void process_rq( char *rq, int fd )
{
	char cmd[BUFSIZ], arg[BUFSIZ];

	/* create a new process and return if not the child */
	if( fork() != 0 )
		return;

	strcpy( arg, "./" );		/* precede args with ./ */
	if( sscanf( rq, "%s%s", cmd, arg+2 ) != 2 )
		return;

	if( strcmp( cmd, "GET" ) != 0 )
		cannot_do( fd );
	else if( not_exist( arg ) )
		do_404( arg, fd );
	else if( isadir( arg ) )
		do_ls( arg, fd );
	else if( ends_in_cgi( arg ) )
		do_exec( arg, fd );
	else
		do_cat( arg, fd );
}

/* ---------------------------------------------------------------- *
 * the reply header thing: all functions need one 
 * if content_type is NULL then don't send content type
 * ---------------------------------------------------------------- */
void header( FILE *fp, char *content_type )
{
	fprintf( fp, "HTTP/1.0 200 OK\r\n" );
	if( content_type )
		fprintf( fp, "Content-type: %s\r\n", content_type );
}

/* ----------------------------------------------------------------- *
 * simple function first:
 * 	cannot_do(fd)	uniplemented HTTP command
 * and do_404(item,fd)	no such object
 * ------------------------------------------------------------------ */
void cannot_do( int fd )
{
	FILE *fp = fdopen( fd, "w" );

	fprintf( fp, "HTTP/1.0 501 Not Implemented\r\n" );
	fprintf( fp, "Content-type: text/plain\r\n" );
	fprintf( fp, "\r\n" );
	fprintf( fp, "That command is not yet implemented\r\n" );

	fclose( fp );
}

void do_404( char *item, int fd )
{
	FILE *fp = fdopen( fd, "w" );

	fprintf( fp, "HTTP/1.0 404 Not Found\r\n" );
	fprintf( fp, "Contetnt-type: text/plain\r\n" );
	fprintf( fp, "\r\n" );
	fprintf( fp, "The item you requested: %s\r\nis not found\r\n",
			item );

	fclose( fp );
}

/* -------------------------------------------------------------------- *
 * the directory listing section
 * isadir() uses stat, not_exist() uses stat
 * do_ls runs ls. It should not
 * -------------------------------------------------------------------- */
int isadir( char *f )
{
	struct stat info;
	return( stat( f, &info ) != -1 && S_ISDIR( info.st_mode ) );
}

int not_exist( char *f )
{
	struct stat info;
	return( stat( f, &info ) == -1 );
}

void do_ls( char *dir, int fd )
{
	FILE *fp;

	fp = fdopen( fd, "w" );
	header( fp, "text/plain" );
	fprintf( fp, "\r\n" );
	fflush( fp );

	dup2( fd, 1 );
	dup2( fd, 2 );
	close( fd );
	execlp( "ls", "ls", "-l", dir, NULL );
	perror( dir );
	exit(1);
}

/* ---------------------------------------------------------------------- *
 * the cgi stuff. function to check extension and
 * one to run the program.
 * ---------------------------------------------------------------------- */
char * file_type( char *f )
{
	char *cp;
	if( ( cp = strrchr( f, '.' ) ) != NULL )
		return cp + 1;
	return "";
}

int ends_in_cgi( char *f )
{
	return( strcmp( file_type(f), "cgi" )  == 0 );
}

void do_exec( char *prog, int fd )
{
	FILE *fp;

	fp = fdopen( fd, "w" );
	header( fp, NULL );
	fflush( fp );

	dup2( fd, 1 );
	dup2( fd, 2 );
	close( fd );
	execl( prog, prog, NULL );
	perror( prog );
	exit(1);
}

/* ----------------------------------------------------------------------- *
 * do_cat( filename, fd )
 * sends back contents after a header 
 * ----------------------------------------------------------------------- */
void do_cat( char *f, int fd )
{
	char *extension = file_type(f);
	char *content = "text/plain";
	FILE *fpsock, *fpfile;
	int c;

	if( strcmp( extension, "html" ) == 0 )
		content = "text/html";
	else if( strcmp( extension, "gif" ) == 0 )
		content = "image/gif";
	else if( strcmp( extension, "jpg" ) == 0 )
		content = "image/jpg";
	else if( strcmp( extension, "jpeg" ) == 0 )
		content = "image/jpeg";

	fpsock = fdopen( fd, "w" );
	fpfile = fopen( f, "r" );
	if( fpsock != NULL && fpfile != NULL )
	{
		header( fpsock, content );
		fprintf( fpsock, "\r\n" );
		while( ( c = getc( fpfile ) ) != EOF )
			putc( c, fpsock );
		fclose( fpfile );
		fclose( fpsock );
	}
	exit(0);
}
