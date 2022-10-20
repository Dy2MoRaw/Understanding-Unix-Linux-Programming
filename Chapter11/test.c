#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void printf_family(struct addrinfo *aip)
{
	printf("family:" );
	switch( aip->ai_family ){
	case AF_INET:
		printf("inet\n");
		break;
	case AF_INET6:
		printf("inet6\n");
		break;
	case AF_UNIX:
		printf( "unix\n" );
		break;
	case AF_UNSPEC:
		printf( "unspecified\n");
		break;
	default:
		printf( "unknow\n" );
	}
}

int main()
{
	char hostname[100];
	char abuf[INET_ADDRSTRLEN];
	const char *addr;

	struct addrinfo *aip;
	struct addrinfo hints;

	struct hostent *hp = gethostent();
	//struct hostent *hp = gethostbyname(hostname);

	hints.ai_family = AF_INET;

	addr = inet_ntop( AF_INET, &hp->h_addr_list, abuf, INET_ADDRSTRLEN );
	printf( "by host:%s\n", addr );
	printf( "by host:%s\n", hp->h_name );

	getaddrinfo( hp->h_name, NULL, &hints, &aip );

	printf_family( aip );
	memset(abuf, 0, sizeof(abuf));
	//addr = inet_ntop( AF_INET, &(aip->ai_addr)->sa_data, abuf, INET_ADDRSTRLEN );
	struct sockaddr_in *saddr = (struct sockaddr_in * )aip->ai_addr;
	printf( "by adddrinfo:%s\n", inet_ntoa( saddr->sin_addr ));

	freeaddrinfo( aip );
}
