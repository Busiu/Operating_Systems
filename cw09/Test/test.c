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
int x;
pthread_mutex_t x_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutexattr_t attr;
int type;

int numberOfThreads;
pthread_t * threads;

void * computing(){
	pthread_mutex_lock(&x_mutex);
	pthread_mutex_lock(&x_mutex);
	x++;
	printf("Wartosc x wynosi na ten moment: %i, tID: %li\n", x, pthread_self());
	pthread_mutex_unlock(&x_mutex);

	return NULL;
}

int main(int argc, char** argv){
	//SPRAWDZANIE LICZBY ARGUMENTOW
	if(argc != 2){
		printf("Prosze podac 1 argument i wrocic ponownie!\n");
		exit(1);
	}
	numberOfThreads = atoi(argv[1]);
	x = 0;

	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&x_mutex, &attr);

	pthread_mutexattr_gettype(&attr, &type);
	if(PTHREAD_MUTEX_RECURSIVE == type){
		printf("Dobry atrybut wlozony zostal\n");
	}
	else{
		printf("Cos sie popsulo\n");
	}
	
	//ODPALANIE WATKOW
	threads = (pthread_t *) malloc (numberOfThreads * sizeof(pthread_t));
	for(int i = 0; i < numberOfThreads; i++){
		pthread_create(&threads[i], NULL, computing, NULL);
	}

	//CZEKANIE, AZ SKONCZA ROBOTE
	for(int i = 0; i < numberOfThreads; i++){
		pthread_join(threads[i], NULL);
	}

	printf("Wartosc x wynosi: %i\n", x);

	return 1;
}
