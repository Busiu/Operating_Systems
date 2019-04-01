#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int i = 1;
int messageFlag = 0;

void messageINT(){
	printf("Odebrano sygnal SIGINT\n");
	exit(0);
}

void messageTSTP(){
	messageFlag = (messageFlag + 1) % 2;
}

void sendTSTP(pid_t child){
	kill(child, SIGTSTP);
}

void sendINT(pid_t child){
	kill(child, SIGINT);
	i = 0;
}
int main(int argc, char ** argv) {
	
    
	pid_t child = fork();
	
	if(child == 0){
		while(1){
			signal(SIGTSTP, messageTSTP);
			signal(SIGINT, messageINT);
			if(messageFlag == 0){
				//printf("LOL\n");
				execl("./skrypt.sh", "./skrypt.sh", NULL);
				//sleep(1);
			}
			else{
				printf("Oczekuje na CTRL+Z - kontynuacja albo CTRL+C - zakonczenie programu\n");
				pause();
			}
			printf("hehe\n");			
		}
	}
	else{
		while(i > 0){
			signal(SIGTSTP, sendTSTP);
			struct sigaction act;
			act.sa_handler = sendINT;
			sigemptyset(&act.sa_mask);
			act.sa_flags = 0;
			sigaction(SIGINT, &act, NULL);
			sleep(1);
		}	
	}

    return 0;
}
