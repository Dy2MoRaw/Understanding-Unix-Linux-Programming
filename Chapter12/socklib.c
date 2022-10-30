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

	struct hostent *hp = gethostent();
	memset( &hints, 0, sizeof(hints) );
	hints.ai_family = AF_INET;
	getaddrinfo( hp->h_name, NULL, &hints, &aip );
	
	saddr = ( struct sockaddr_in * )aip->ai_addr;
	saddr->sin_family = AF_INET;
	saddr->sin_port = htons( portnum );

	int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if( sockfd == -1 )
		return -1;

	if( bind( sockfd, ( struct sockaddr *)saddr, sizeof( *saddr ) ) )
		return -1;

	if( listen( sockfd, backlog ) != 0 )
		return -1;
	
	printf( "ip:%s\n", inet_ntoa( saddr->sin_addr ) );
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
