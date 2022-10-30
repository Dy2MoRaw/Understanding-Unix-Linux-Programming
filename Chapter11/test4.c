#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <net/if.h>

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
	int fd;
	struct ifreq ifr;
	char *ip;

	fd = socket( AF_INET, SOCK_STREAM, 0 );

	ifr.ifr_addr.sa_family = AF_INET;

// 	strncpy( ifr.ifr_name, "myhost", IFNAMSIZ-1 );

	ioctl( fd, SIOCGIFADDR, ( char * )&ifr );

	close( fd );

	sprintf( ip, "%s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr) ); 
	printf("%s\n", ip );
}
