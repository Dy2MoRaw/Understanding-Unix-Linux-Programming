#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<string.h>
#include<utmp.h>
#include<fcntl.h>

int logout(const char *);

int main(int argc, char *argv[]){
	const char *str = "argv[1]";
	logout(str);

	return 0;
}

/*
 * logout
 * 	marks a utmp record as logged out
 * 	does not blank username or remote host
 * 	returns -1 onerror, 0 on success
 */
int logout(const char *utline)
{
	int fd;
	struct utmp rec;
	int utsize = sizeof(struct utmp);
	int retval = -1;

	if((fd = open(UTMP_FILE, O_RDWR)) == -1 ){
		perror("file open fail");
		return -1;
	}

	while(read(fd, &rec, utsize) == utsize){
		if(strncmp(rec.ut_line, utline, sizeof(rec.ut_line)) == 0)
		{
			rec.ut_type = DEAD_PROCESS;
			if(time((time_t *)&rec.ut_time) != -1)
				if(lseek(fd, -utsize, SEEK_CUR) != -1)
					if(write(fd, &rec, utsize) == utsize)
						retval = 0;
			break;
		}
	}

	if(close(fd) == -1){
		perror("file close fail");
		retval = -1;
	}

	return retval;
}
