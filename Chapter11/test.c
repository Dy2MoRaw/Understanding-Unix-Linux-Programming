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

	struct hostent *hp = gethostent();

	memset( &hints, 0, sizeof(hints) );
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_ADDRCONFIG;

	getaddrinfo( hp->h_name, portnum, &hints, &aip );
	
	//int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	//memset(abuf, 0, sizeof(abuf));

	//struct sockaddr_in *saddr = (struct sockaddr_in * )aip->ai_addr;
	inet_ntop( AF_INET, &(( struct sockaddr_in *) aip->ai_addr)->sin_addr 
				, abuf, INET_ADDRSTRLEN );
	printf( "by adddrinfo:%s\n", abuf );

	freeaddrinfo( aip );
}
