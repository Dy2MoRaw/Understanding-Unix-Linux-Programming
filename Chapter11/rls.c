/* rls.c - a client for a remote directory listing service
 * 	   usage: rls hostname directory
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
#define PORTNUM 15000

int main( int argc, char *argv[] )
{
	struct sockaddr_in *servaddr;
	struct addrinfo hints, *aip;
	struct hostent *hp;

	int sockfd;
	char buffer[BUFSIZ];
	int n_read;
	
	if( argc != 3 ){
		perror("Usage: ./rls hostname directory");
	       	exit(1);
	}
	memset( &hints, 0, sizeof(hints) );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	hints.ai_flags = 0;

	if( ( hp = gethostbyname( argv[1] ) ) == NULL )
		oops( argv[1] );

	getaddrinfo( hp->h_name, NULL, &hints, &aip );

	servaddr = ( struct sockaddr_in * )aip->ai_addr;
	servaddr->sin_family = AF_INET;
	servaddr->sin_port = htons( PORTNUM );

	sockfd = socket( aip->ai_family, aip->ai_socktype, aip->ai_protocol );
	if( sockfd == -1 )
		oops( "socket" );

	if( connect( sockfd, aip->ai_addr, aip->ai_addrlen ) != 0 )
		oops( "connect" );
	
	// send massage and then get results
	if( write( sockfd, argv[2], strlen(argv[2]) ) == -1 )
		oops( "write" );

	if( write( sockfd, "\n", 1 ) == -1 )
		oops( "write" );
	
	memset( buffer, 0, sizeof(char) * BUFSIZ );
	while( ( n_read = read( sockfd, buffer, BUFSIZ ) ) > 0 )
		if( write( 1, buffer, n_read ) == -1 )
			oops( "write" );

	close( sockfd );
	freeaddrinfo(aip);
}
