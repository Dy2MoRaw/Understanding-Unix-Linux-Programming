/* test with gethostent() function:
 */
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 2048

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

	int sockfd;
	FILE *sockfp;

	char *ctime();
	time_t thetime;
	
	/* hostent */
	struct hostent *hp = gethostent();
	memcpy( hostname, hp->h_name, 100 );
	printf( "by hostname:%s\n", hostname );
	hints.ai_family = AF_INET;

	int i;
	struct in_addr **addr_list = (struct in_addr **)hp->h_addr_list;
	for( i = 0; addr_list[i] != NULL; i++ )
		printf( "by host:%s\n", inet_ntoa(*addr_list[i]) );
	printf( "by host:%s\n", hp->h_name );

	/* getaddrinfo */
	getaddrinfo( hp->h_name, NULL, &hints, &aip );

	printf_family( aip );
	memset(abuf, 0, sizeof(abuf));


	/* socket */
	sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	
	//saddr->sin_addr = **addr_list;
	struct sockaddr_in *saddr = (struct sockaddr_in * )aip->ai_addr;
	printf( "by adddrinfo:%s\n", inet_ntoa( saddr->sin_addr ));
	
	saddr->sin_port = htons( PORT );
	saddr->sin_family = AF_INET;

	if( bind( sockfd, (struct sockaddr *)saddr, sizeof(*saddr) ) != 0 )
		perror( "bind" );

	if( listen( sockfd, 1 ) != 0 )
		perror( "listen" );

	while(1){
		int sockfd_new = accept( sockfd, NULL, NULL );
		printf( "Got a call!\n" );
		if( sockfd_new == -1 )
			perror( "accept" );

		sockfp = fdopen( sockfd_new, "w" );
		if( sockfp == NULL )
			perror( "fdopen" );

		thetime = time(NULL);
		fprintf( sockfp, "The time here is.." );
		fprintf( sockfp, "%s", ctime(&thetime) );
		fclose( sockfp );
	}

	freeaddrinfo( aip );
}
