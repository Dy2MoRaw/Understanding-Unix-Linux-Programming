/* ls2.c -
 * 	 - list contents of directory or directories
 * 	 - if no args, use . else list files in args
 * 	 - uses stat and pwd.h and grp.h
 * 	 - BUG: try ls2 /tmp
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<dirent.h>
#include<sys/stat.h>
#include<pwd.h>
#include<grp.h>

void do_ls(char []);
void dostat(char *);
void show_file_info(char *, struct stat *);
void mode_to_letters(int, char []);
char *uid_to_name(uid_t);
char *gid_to_name(gid_t);

int main(int ac, char* av[])
{
	if( ac == 1 )
		do_ls( "." );
	else
		while( --ac ){
			printf( "%s:\n", *++av );
			do_ls( *av );
		}
}

void do_ls( char dirname[] )
/*
 * list files in directory called dirname
 */
{
	DIR *dir_ptr;	/* a pointer to the directory stream */
	struct dirent *direntp;	/* each entry */

	if(( dir_ptr = opendir( dirname )) == NULL )
		fprintf( stderr, "ls2: cannot open %s\n", dirname );
	else
	{
		/* readdir() returns a ptr to a dirent structure
		 * representing the next entry in the directory stream.
		 */
		while(( direntp = readdir( dir_ptr )) != NULL )
			dostat( direntp -> d_name );
	//	closedir( dir_ptr );
	}
}

void dostat( char *filename )
{
	/* NOTE:
	 * the struct stat shoud creat an instance first,
	 * then can be pointed to by a pointer.
	 */
	struct stat info;
	if( stat( filename, &info ) == -1 )
		perror( filename );
	else
		show_file_info( filename, &info );
}

void show_file_info( char *filename, struct stat *info_p )
/*
 * display the info about 'filename'. The info is stored in struct at info_p
 */
{
	char *uid_to_name(), *ctime(), *gid_to_name();
	void mode_to_letters();
	char modestr[11];

	mode_to_letters( info_p -> st_mode, modestr );

	printf( "%s", modestr );
	printf( "%4d ", (int) info_p -> st_nlink );
	printf( "%-8s ", uid_to_name( info_p -> st_uid ));
	printf( "%-8s ", gid_to_name( info_p -> st_gid ));
	printf( "%8ld ", (long) info_p -> st_size );
	printf( "%.12s ", ctime( &info_p -> st_mtime ) + 4 );
	printf( "%s\n", filename );
}

/* Utility functions
 * I think utitily here means not logic functions,
 * but some format transformation.
 */

/*
 * This function takes a mode value and a char array
 * and puts into the char array the file type and the 
 * nine letters that correspond to the bits in mode.
 * NOTE: It does not code setuid, setgid, and sticky codes.
 */
void mode_to_letters( int mode, char str[] )
{
	strcpy( str, "----------" );	/* default = no perms */
	
	/* 
	 * in <bits/stat.h> macros:
	 * #define S_ISDIR(m) (((mode) & (S_IFMT)) == (0040000))
	 * which S_IFMT is a mask that selects the first four bits.
	 * #define S_IFMT 0170000
	 */
	if( S_ISDIR(mode) ) str[0] = 'd';
	if( S_ISCHR(mode) ) str[0] = 'c';
	if( S_ISBLK(mode) ) str[0] = 'b';
	
	/* can find the permission bits definition
	 * in open(2) O_CREAT.
	 */ 
	if( mode & S_IRUSR ) str[1] = 'r';
	if( mode & S_IWUSR ) str[2] = 'w';
	if( mode & S_IXUSR ) str[3] = 'x';

	if( mode & S_IRGRP ) str[4] = 'r';
	if( mode & S_IWGRP ) str[5] = 'w';
	if( mode & S_IXGRP ) str[6] = 'x';

	if( mode & S_IROTH ) str[7] = 'r';
	if( mode & S_IWOTH ) str[8] = 'w';
	if( mode & S_IXOTH ) str[9] = 'x';
}

char *uid_to_name(uid_t uid)
/*
 *  return pointer to username associated with uid, uses getpw()
 */
{
	struct passwd *getpwuid(), *pw_ptr;
	static char numstr[10];

	if(( pw_ptr = getpwuid( uid )) == NULL ){
		sprintf( numstr, "%d", uid );
		return numstr;
	}
	else
		return pw_ptr -> pw_name;
}

char *gid_to_name( gid_t gid )
/*
 * return pointer to group number gid. used getgrgid(3)
 */
{
	struct group *getgrgid(), *grp_ptr;
	static char numstr[10];

	if(( grp_ptr = getgrgid( gid )) == NULL ){
		sprintf( numstr, "%d", gid );
		return numstr;
	}
	else
		return grp_ptr -> gr_name;
}
