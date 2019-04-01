#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

typedef int bool;
#define true 1
#define false 0

char * pathName;
char buffor[16384];
int file;

int main(int argc, char ** argv){

	if(argc != 2){
		printf("Prosze podac sciezke do potoku nazwanego!!!/n");
		return -1;
	}

	pathName = argv[1];
	
	//TWORZENIE POTOKU I JEGO OTWARCIE
	mkfifo(pathName, 0666);
	file = open(pathName, O_RDWR);

	while(1){
		read(file, buffor, 16384);
		if(buffor != NULL){
			printf("%s", buffor);
		}
		sleep(1);
	}	

    return 0;
}
