/* socklib.c
 * 	This file contains functions used lots when writing internet
 * 	client/server programs. The two main functions here are:
 *
 * 	int make_server_socket( portnum ) returns a server socket
 * 						or -1 if error
 * 	int make_server_socket_q( portnum, bakclog )
 *
 * 	int connet_to_server( char *hostname, int portnum )
 * 					returns a conneted socket
 * 					or -1 if error
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
#include <arpa/inet.h>

#define BACKLOG 1

int make_server_socket_q( int portnum, int backlog )
{
	struct sockaddr_in *saddr;
	struct addrinfo hints, *aip;

	char port[BUFSIZ];

	memset( &hints, 0, sizeof(hints) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	sprintf( port, "%d", portnum );
	getaddrinfo( NULL, port, &hints, &aip );

	int sockfd = socket( aip->ai_family, aip->ai_socktype, 0 );
	if( sockfd == -1 )
		return -1;

	if( bind( sockfd, aip->ai_addr, aip->ai_addrlen ) )
		return -1;

	if( listen( sockfd, backlog ) != 0 )
		return -1;
	
	char ip[BUFSIZ];
	inet_ntop( aip->ai_family, &((struct sockaddr_in *)aip->ai_addr)->sin_addr
							, ip, BUFSIZ );
	printf( "ip:%s\n", ip );
	return sockfd;
}

int make_server_socket( int portnum )
{
	return make_server_socket_q( portnum, BACKLOG );
}

int connect_to_server( char *host, int portnum )
{
	struct sockaddr_in servaddr;
	struct hostent *hp;
	int sockfd;

	sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if( sockfd == -1 )
		return -1;

	if( ( hp = gethostbyname( host ) ) == NULL )
		return -1;
	
	memset( &servaddr, 0, sizeof(servaddr) );
	bcopy( hp->h_addr, ( struct sockaddr * )&servaddr.sin_addr,
			hp->h_length );
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons( portnum );

	if( connect( sockfd, ( struct sockaddr * )&servaddr,
			       	sizeof( servaddr ) ) != 0 )
		return -1;

	return sockfd;
}
