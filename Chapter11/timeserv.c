/* timeserv.c - a socket-based time of day server
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>		/* struct sockaddr_in */
#include <netdb.h>		/* struct hostent */
#include <time.h>
#include <string.h>
#include <arpa/inet.h>

#define PORTNUM 13000		/* our time service phone number */

#define oops(msg)	{ perror(msg); exit(1); }

int main( int argc, char *argv[] )
{
	struct sockaddr_in *saddr;	/* build our address here */
	struct addrinfo hints, *aip;

	int sockfd, sockfd_new;		/* line id, file descriptor */
	FILE *sockfp;			/* use socket as stream */

	char *ctime();			/* convert secs to string */
	time_t thetime;			/* the time we report */

	struct hostent *hp = gethostent();

	memset( &hints, 0 ,sizeof(hints) );
	hints.ai_family = AF_INET;
	getaddrinfo( hp->h_name, NULL, &hints, &aip );

	saddr = ( struct sockaddr_in * )aip->ai_addr;
	saddr->sin_port = htons( PORTNUM );		/* fill in socket port */
	saddr->sin_family = AF_INET;			/* fill in addr family */

	sockfd = socket( AF_INET, SOCK_STREAM, 0 );	/* get a socket */
	if( sockfd == -1 )
		oops( "socket" );
	
	if( bind( sockfd, (struct sockaddr *)saddr, sizeof(*saddr) ) != 0 )
		oops( "bind" );

	if( listen( sockfd, 1 ) != 0 )
		oops( "listen" );

	while(1){
		sockfd_new = accept( sockfd, NULL, NULL );	/* wait for call */
		printf( "Wow! got a call\n" );
		if( sockfd_new == -1 )
			oops( "accept" );		/* error getting calls */

		sockfp = fdopen( sockfd_new, "w" );	/* we'll write to the */
		if( sockfp == NULL )			/* socket as a stream */
			oops( "fdopen" )		/* unless we can't */

		thetime = time( NULL );			/* get time */
							/* and covnert to string */
		fprintf( sockfp, "The time here is.." );
		fprintf( sockfp, "%s", ctime(&thetime) );
		fclose( sockfp );			/* release connection */
	}

	freeaddrinfo( aip );
}
