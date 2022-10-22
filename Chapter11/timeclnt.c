/* timeclnt.c - a client for timeserv.c
 * 		usaget: timeclnt hostname portnumber
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define oops(msg) { perror(msg); exit(1); }

int main( int argc, char *argv[] )
{
	struct sockaddr_in *servaddr;
	struct hostent *hp;
	struct addrinfo hints, *aip;

	int sockfd, sockfd_new;

	char message[BUFSIZ];
	int messlen;

	memset( &hints, 0, sizeof(hints) );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;
	
	hp = gethostent();
	getaddrinfo( hp->h_name, NULL, &hints, &aip );
	
	sockfd = socket( aip->ai_family, aip->ai_socktype, aip->ai_protocol );
	if( sockfd == -1 )
		oops( "socket" );

	servaddr = ( struct sockaddr_in * )aip->ai_addr ;
	servaddr->sin_port = htons( atoi(argv[2]) );
	servaddr->sin_family = AF_INET;

	if( connect( sockfd, aip->ai_addr, aip->ai_addrlen ) != 0 )
		oops( "connet" );

	messlen = read( sockfd, message, BUFSIZ );
	if( messlen == -1 )
		oops( "read" );
	if( write( 1, message, messlen ) != messlen )
		oops( "write" );
	close( sockfd );

	freeaddrinfo( aip );
}
