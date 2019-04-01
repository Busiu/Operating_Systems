#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

typedef int bool;
#define true 1
#define false 0

#define buffor_size 16384

char * pathName;
int N;

char buffor[buffor_size];
int file;

int main(int argc, char ** argv){

	if(argc != 3){
		printf("Prosze podac sciezke do potoku nazwanego oraz liczbe N!!!/n");
		return -1;
	}

	pathName = argv[1];
	N = atoi(argv[2]);
	
	//TWORZENIE POTOKU I JEGO OTWARCIE
	file = open(pathName, O_RDWR);

	//USTAWIANIE SRANDA
	int seed;
	time_t tt;
	seed = time(&tt);
	srand(seed);

	printf("%i\n", getpid());

	for(int i = 0; i < N; i++){
		FILE *f = popen("date", "r");
		fread(buffor, buffor_size, buffor_size, f);
		fwrite("hehehe", 6, 6, f);
		pclose(f);
		write(file, buffor, strlen(buffor));
		sleep(rand() % 4 + 2);
	}
	close(file);	

    return 0;
}
