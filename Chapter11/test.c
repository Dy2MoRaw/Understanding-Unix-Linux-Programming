#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
	char abuf[INET_ADDRSTRLEN];
	const char *portnum = "12000";

	struct addrinfo *aip;
	struct addrinfo hints;
	
	struct sockaddr_storage *staddr;
	struct sockaddr_in *saddr;
	//struct hostent *hp = gethostent();

	memset( &hints, 0, sizeof(hints) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo( NULL, portnum, &hints, &aip );
	
	int sockfd = socket( aip->ai_family, aip->ai_socktype, 0 );
	staddr = ( struct sockaddr_storage * )aip->ai_addr;
	//inet_ntop( AF_INET, &(( struct sockaddr_in *) aip->ai_addr)->sin_addr, abuf, INET_ADDRSTRLEN );
	inet_ntop( aip->ai_family, &((struct sockaddr_in *)staddr)->sin_addr, abuf, INET_ADDRSTRLEN );
	printf( "by adddrinfo:%s\n", abuf );

	freeaddrinfo( aip );
}
