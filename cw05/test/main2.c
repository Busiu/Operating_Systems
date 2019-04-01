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
		close(fd[0]);
		read(fd[0], buff, sizeof(buff));
		printf("%s", buff);
		exit(1);
	}
	else{
		close(fd[0]);
		write(fd[1], "hello world\n", sizeof("hello world\n"));
		close(fd[1]);
	}

    return 0;
}

//(2) Jak zamkniemy strumien odczytu przed odczytem, to dany buffor pozostanie pusty, przez co na ekranie nie pojawi sie zupelnie nic
