#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

typedef enum{
false,
true
} bool;

//ZMIENNE GLOBALNE
FILE * file;
char string1[5] = "hehe\n";
char string2[21] = "hohohohohohohohohoho\n";
char string3[51] = "hihihihihihihihihihihihihihihihihihihihihihihihihi\n";
int noLines;

int main(int argc, char** argv){
	//SPRAWDZANIE LICZBY ARGUMENTOW
	if(argc != 3){
		printf("Prosze podac nazwe pliku i liczbe linijek\n");
		exit(1);
	}

	noLines = atoi(argv[2]);
	file = fopen(argv[1], "w");
	if(file == NULL){
		perror("Otwieranie pliku wynikowego");
		exit(1);
	}

	
	//ZAPIS DO PLIKU WYNIKOWEGO
	for(int i = 1; i <= noLines; i++){
		fprintf(file, "%i ", i);
		if(i % 3 == 1){
			fprintf(file, string1);
		}
		else if(i % 3 == 2){
			fprintf(file, string2);
		}
		else{
			fprintf(file, string3);
		}
	}

	fclose(file);

	return 1;
}
