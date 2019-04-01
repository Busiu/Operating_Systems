#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>


int main(int argc, char ** argv) {
    
	int fd1[2];
	int fd2[2];
	pipe(fd1);
	pipe(fd2);
	pid_t child = fork();
	if(child == 0){
		close(fd1[1]);
		close(fd2[0]);

		char buff[100];
		read(fd1[0], buff, sizeof(buff));
		strcat(buff, " hehehe");
		write(fd2[1], buff, strlen(buff));
	}
	else{
		close(fd1[0]);
		close(fd2[1]);

		char buff[100];
		write(fd1[1], "hello world", sizeof("hello world"));
		read(fd2[0], buff, sizeof(buff));
		printf("%s\n", buff);
	}

    return 0;
}
