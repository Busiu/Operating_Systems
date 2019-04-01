#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int i = 1;
int messageFlag = 0;

void sendTSTP(pid_t child){
	if(messageFlag == 0){
		kill(child, SIGINT);
		printf("Oczekuje na CTRL+Z - kontynuacja albo CTRL+C - zakonczenie programu\n");
	}
	messageFlag = (messageFlag + 1) % 2;

}

void sendINT(pid_t child){
	printf("Odebrano sygnal SIGINT\n");
	kill(child, SIGINT);
	i = 0;
}
int main(int argc, char ** argv) {
	pid_t child;

    while(i > 0){
		if(messageFlag == 0){
			child = fork();
		}

		if(child == 0){
			execl("./skrypt.sh","./skrypt.sh",NULL);
		}
		else{
			//DEFINIOWANIE SYGNALOW
			signal(SIGTSTP, sendTSTP);
			struct sigaction act;
			act.sa_handler = sendINT;
			sigemptyset(&act.sa_mask);
			act.sa_flags = 0;
			sigaction(SIGINT, &act, NULL);
			///////////////////////////////////
			pause();

			
		}
	}

    return 0;
}
