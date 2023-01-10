/* spwd.c
 * 	 - strats in current directory and recursively
 * 	 - climbs up to root of filesystem, prints top part
 * 	 - then prints current part.
 *
 * 	 - uses readdir() to get info about each thing.
 *
 * 	 - Debug: prints an empty string if run from "/"
 */
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<string.h>
#include<unistd.h>

ino_t get_inode(char *);
void printpathto(ino_t);
void inode_to_name(ino_t, char *, int);

int main()
{
	char root_name[8092];
	if( get_inode( "." ) == get_inode( ".." ) ){
		inode_to_name( get_inode("."), root_name, 8092);
		printf( "/%s\n", root_name );
	}else{
		printpathto( get_inode( "." ));
		putchar( '\n' );
	}
	return 0;
}
void printpathto( ino_t curdir_inode )
/*
 *	prints path leading down to an object
 *	with this inode kindof recursive.
 */
{
	/* 
	 * in< stdio.h >
	 * #define BUFSIZ 8192
	 */
	char its_name[BUFSIZ];

	if( get_inode("..") != curdir_inode ){
		chdir( ".." );	/* means .(current directory)
				 * change to her parent dir .. 
				 */

		inode_to_name( curdir_inode, its_name, BUFSIZ );
		printpathto( get_inode( ".") );
		printf( "/%s", its_name );
	}
}

void inode_to_name( ino_t inode_to_find, char *namebuf, int buflen )
/*
 * 	looks through current directory for a file with this inode
 * 	number and copies its name into namebuf
 */
{
	DIR *dir_ptr;
	struct dirent *direntp;

	if(( dir_ptr = opendir( "." )) == NULL ){
		perror( "." );
		exit(1);
	}
	/*
	 * search directory for a file with specified inode
	 */
	while(( direntp = readdir( dir_ptr )) != NULL )
		if( direntp->d_ino == inode_to_find )
		{
			strncpy( namebuf, direntp->d_name, buflen );
		//	namebuf[buflen-1] = '\0';
			closedir( dir_ptr );
			return;
		}	
	fprintf( stderr, "error looking for inode %ld\n", inode_to_find );
	exit(1);
}

ino_t get_inode( char *fname )
/*
 *	a directory is just a file containing names of other files.
 *	There make no difference between a file and a directory.
 */
{
	struct stat info;
	if( stat( fname, &info ) == -1 ){
		fprintf( stderr, "Cannot stat " );
		perror( fname );
		exit(1);
	}
	return info.st_ino;
}
