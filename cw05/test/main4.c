#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>


int main(int argc, char ** argv) {
    
	int fd[2];
	pid_t child;

	pipe(fd);	
	child = fork();
	if(child == 0){
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
		execlp("wc", "wc", "-l", NULL);
	}
	else{
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		//execlp("ps", "ps", "aux", NULL);
		printf("hehehe\nddasdasd\ndasdasd\ndsadas\n");
	}
	
	return 0;
}
