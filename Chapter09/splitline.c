/* splitline.c - command reading and parsing functions for smsh
 *
 * char *next_cmd( char *prompt, FILE *fp ) - get next command
 * char **splitline( char *str );	    - parse a string
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "smsh.h"

char *next_cmd( char *prompt, FILE *fp ) 
/*
 * purpose:                    read next command line from fp
 * returns: dynamically allocated string holding command line
 *  errors:                 NULL at EOF (not really an error)
 *          calls fatal from emalloc()
 *   notes: allocates space in BUFSIZE chunks
 */
{
	char *buf;
	int bufspace = BUFSIZ;
	int curindex = 0;
	int arrindex = 0;
	int c;

	buf = emalloc( BUFSIZ );
	arrindex = BUFSIZ/sizeof(char);
	printf("%s", prompt);

	while( (c = getc(fp)) != EOF ){
		/* need space? */
		if( (curindex + 1) >= arrindex ){
			buf = erealloc( buf, bufspace + BUFSIZ );
			bufspace += BUFSIZ;	/* update size */
			arrindex += ( BUFSIZ/sizeof(char) );
		}

		/* end of command? */
		if( c == '\n' )
			break;
		/* no, add to buffer */
		buf[curindex++] = c;
	}

	if( c == EOF && curindex == 0 )	/* EOF and no input */
		return NULL;
	buf[curindex] = '\0';
	return buf;
}

/*
 * splitline( parse a line into an array of strings )
 */
#define is_delim(x) ( (x) == ' ' || (x) == '\t' )

char **splitline( char *line )
/*
 * purpose: split a line into array of white-space separated tokens
 * returns: a NULL-terminated array of pointers to copies of the tokens
 * 	    or NULL if line if no tokens on the line
 *  action: traverse the array, locate strings, make copies
 *    note: strtok() could work, but we may want to add quotes later
 */
{
	char *addstr();
	char **args;
	int arrindex = 0;	/* spots in table */
	int bufspace = 0;	/* bytes in array */
	int curindex = 0;	/* slots used */
	char *cp = line; 	/* pos in string */
	char *start;
	int len;

	if( line == NULL )
		return NULL;

       	args      = emalloc(BUFSIZ);
	bufspace  = BUFSIZ;
	arrindex  = BUFSIZ/sizeof(char *);

	while( *cp != '\0' )
	{
		while( is_delim(*cp) )		/* skip leading spaces */
			cp++;
		if( *cp == '\0' )		/* quit at end-of-string */
			break;

		/* make sure the array has room (+1 for NULL) */
		if( (curindex + 1) >= arrindex ){
			args = erealloc( args, bufspace + BUFSIZ );
			bufspace += BUFSIZ;
			arrindex += ( BUFSIZ/sizeof(char *) );
		}

		/* mark start, then find end of word */
		start = cp;
		len   = 1;
		while( *++cp != '\0' && !(is_delim(*cp)) )
			len++;
		args[curindex++] = addstr( start, len );
	}
	args[curindex] = NULL;
	return args;
}

/*
 * purpose: constructor for strings
 * returns: a string, never NULL
 */
char *addstr( char *s, int l )
{
	char *rv = emalloc( l + 1 );
	rv[l] = '\0';
	strncpy( rv, s, l );
	return rv;
}

void freelist( char **list )
/*
 * purpose: free the list returned by splitline
 * returns: noting
 * action: free all strings in list and then free the list
 */
{
	char **cp = list;
	while( *cp )
		free( *cp++ );
	free(list);
}

void *emalloc( size_t n )
{
	void *rv;
	if( (rv = malloc(n)) == NULL )
		fatal( "out of memory", "", 1 );
	return rv;
}

void *erealloc( void *p, size_t n )
{
	void *rv;
	if( (rv = realloc(p, n)) == NULL )
		fatal( "realloc() failed", "", 1 );
	return rv;
}

void fatal( char *s1, char *s2, int n)
{
	fprintf( stderr, "Error: %s, %s\n", s1, s2 );
	exit(n);
}
