/* rlsd.c - a remote ls server - with paranoia
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#define PORTNUM 15000
#define HOSTLEN 256
#define oops(msg) { perror(msg); exit(1); }

//void sanitize( char * );

int main( int argc, char *argv[] )
{
	struct sockaddr_in *servaddr;
	struct hostent *hp;
	struct addrinfo hints, *aip;

	int sockfd, sockfd_new;
	FILE *sock_fpi, *sock_fpo;
	FILE *pipe_fp;

	char dirname[100];
	char command[200];
	int dirlen, c;

	memset( &hints, 0, sizeof(hints) );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	hp = gethostent();
	getaddrinfo( hp->h_name, NULL, &hints, &aip );

	servaddr = ( struct sockaddr_in * )aip->ai_addr;
	servaddr->sin_port = htons( PORTNUM );
	servaddr->sin_family = AF_INET;

	sockfd = socket( aip->ai_family, aip->ai_socktype, aip->ai_protocol );
	if( sockfd == -1 )
		oops( "socket" );

	if( bind( sockfd, aip->ai_addr, aip->ai_addrlen ) != 0 )
		oops( "bind" );

	if( listen( sockfd, 1 ) != 0 )
		oops( "listen" );
	
	while(1){
		sockfd_new = accept( sockfd, NULL, NULL );
		if( sockfd_new == -1 )
			oops( "accept" );
		
		memset( dirname, 0, 100 );
		if( ( sock_fpi = fdopen( sockfd_new, "r" ) ) == NULL )
			oops( "fdopen reading" );
		if( fgets( dirname, 100, sock_fpi ) == NULL )
			oops( "reading dirname" );
	//	sanitize( dirname );

		memset( command, 0, 200 );
		sprintf( command, "ls %s", dirname );
		if( ( pipe_fp = popen( command, "r" ) ) == NULL )
			oops( "popen" );
		if( ( sock_fpo = fdopen( sockfd_new, "w" ) ) == NULL )
			oops( "fdopen writing" );
		while( ( c = getc( pipe_fp ) ) != EOF ) 
			putc( c, sock_fpo );

//		char buffer[BUFSIZ];
//		memset( buffer, 0, BUFSIZ );
//		while(  fgets( buffer, BUFSIZ, pipe_fp ) != NULL ){
//			buffer[strcspn(buffer, "\n")] = ' ';
//			fprintf( sock_fpo, "%s", buffer );
//		}

		pclose( pipe_fp );
		fclose( sock_fpo );
		fclose( sock_fpi );
	}

	close( sockfd );
	close( sockfd_new );
	freeaddrinfo( aip );
}

//void sanitize( char *str )
// /* it would be very bad if someone passed us an dirname like
// * "; rm *" and we naively created a command "ls ; rm *"
// *
// * so..we remove everything but slashes and alphanumerics
// * There are nicer solutions, see exercises
// */
//{
//	char *src, *dest;
//
//	for( src = dest = str; *src; src++ )
//		if( *src == '/' || isalnum( *src ) )
//			*dest++ = *src;
//	*dest = '\0';
//}
