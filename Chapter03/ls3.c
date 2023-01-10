/* ls3.c -
 * 	 - list contents of directory or directories
 * 	 - if no args, use . else list files in args
 * 	 - uses stat and pwd.h and grp.h
 * 	 - Verion New Added:
 * 	 - 1. DEBUG: try ls2 /tmp.
 * 	 - 2. sort filenames.
 * 	 - 3. handle the -a option.
 * 	 - 4. handle total blocks.
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<dirent.h>
#include<sys/stat.h>
#include<pwd.h>
#include<grp.h>

#define Maxline 4096

static char *dname;
static char *fname;
static char pathname[Maxline];

void do_ls(char []);
void dostat(char *);
void show_file_info(char *, struct stat *);
void ftop(char *filename, char *pathname);
void mode_to_letters(int, char []);
char *uid_to_name(uid_t);
char *gid_to_name(gid_t);
int fname_cmp( const void*, const void* );

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
	struct dirent *sortf[Maxline];
	int i = 0;
	int total = 0;
	struct stat info;

	dname = dirname;

	if(( dir_ptr = opendir( dirname )) == NULL )
		fprintf( stderr, "ls2: cannot open %s\n", dirname );
	
	while((direntp = readdir(dir_ptr)) != NULL ){
		fname = direntp->d_name;
		ftop(fname, pathname);
		stat(pathname, &info);
		total += info.st_blocks;
	}
	printf("total %d\n", total/2);

	if(( dir_ptr = opendir( dirname )) == NULL )
		fprintf( stderr, "ls2: cannot open %s\n", dirname );

	while((direntp = readdir(dir_ptr)) != NULL ){
		if(i == Maxline){
			perror("Out of range");
		}
		sortf[i++] = direntp;
	}

	qsort(sortf, i, sizeof(sortf[0]), fname_cmp);

	for(int j = 0; j < i; j++){
		fname = sortf[j]->d_name;
		ftop(fname, pathname);
		dostat(pathname);
	}
}

void dostat( char *pathname )
{
	/* NOTE:
	 * the struct stat shoud creat an instance first,
	 * then can be pointed to by a pointer.
	 */
	struct stat info;

	if( stat( pathname, &info ) == -1 ){
		perror( pathname );
		exit(1);
	}else{
		show_file_info(fname, &info);
	}
}

void ftop(char *filename, char *pathname)
{
	int len = sizeof(dname);
	
	memset(pathname, 0, sizeof(pathname[Maxline]));

	if(dname[len-1] != '/'){
		sprintf(pathname, "%s/%s", dname, filename);
	}else{
		sprintf(pathname, "%s%s", dname, filename);
	}
}

void show_file_info( char *filename, struct stat *info_p )
/*
 * display the info about 'pathname'. The info is stored in struct at info_p
 */
{
	char *uid_to_name(), *ctime(), *gid_to_name();
	void mode_to_letters();
	char modestr[11];

	mode_to_letters( info_p -> st_mode, modestr );

	printf( "%s", modestr );
	printf( "%3d ", (int) info_p -> st_nlink );
	printf( "%-6s ", uid_to_name( info_p -> st_uid ));
	printf( "%-6s ", gid_to_name( info_p -> st_gid ));
	printf( "%8ld ", (long) info_p -> st_size );
	printf( "%.12s ", ctime( &info_p -> st_mtime ) + 4 );

	printf( "%s\n", filename );
}

/* Utility functions
 * I think utitily here means not logical functions,
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

int fname_cmp( const void *p, const void *q )
{
	char *l = (*( struct dirent **)p )->d_name;
	char *r = (*( struct dirent **)q )->d_name;

	return strcmp( l , r );
}
