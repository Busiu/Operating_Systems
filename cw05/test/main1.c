#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

int main(int argc, char ** argv) {
    
	int fd[2];
	pipe(fd);
	pid_t child = fork();
	if(child == 0){
		close(fd[1]);
		char buff[100];
		read(fd[0], buff, sizeof(buff));
		printf("%s", buff);
		close(fd[0]);
		exit(1);
	}
	else{
		close(fd[0]);
		close(fd[1]);
		write(fd[1], "hello world\n", sizeof("hello world\n"));
	}

    return 0;
}

//(1) Jak zamkniemy strumien do zapisu, to proces macierzysty nie wysle nic do swojego potomka
