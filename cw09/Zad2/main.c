#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
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

#define max_size 100000
#define wyniki "Times.txt"

typedef enum{
false,
true
} bool;

//ZMIENNE GLOBALNE
bool isEmpty = true;
bool isFull = false;
bool isRead = false;
	/*
	** 0 - MNIEJSZE OD L
	** 1 - ROWNE L
	** 2 - WIEKSZE OD L
	*/
int fossickingMode;
	/*
	** 0 - TRYB UPROSZCZONY
	** 1 - TRYB OPISOWY
	*/
int writingMode;
int P, K, N, L, nk;
int bufferIndexConsumer = 0;
int bufferIndexProducer = 0;
const int elementSize = 1024;
char fileName[64];
char ** buffer;
FILE * reader;
FILE * confFile;
pthread_t * threadsProducer;
pthread_t * threadsConsumer;
pthread_t threadTimer;
sem_t buffer_mutex;

struct timeval t1;
struct timeval t2;

void startTime(){
	gettimeofday(&t1, NULL);
}

double endTime(){
	gettimeofday(&t2, NULL);
	double elapsedTime = t2.tv_sec - t1.tv_sec;
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000000.0;	

	return elapsedTime;
}


void * consuming(void * arg){
	int index = *((int *) arg);
	while(true){
		sem_wait(&buffer_mutex);
		//WARUNEK WYJSCIA
		if(isRead && nk == 0){
			sem_post(&buffer_mutex);
			break;
		}
		if(isEmpty){
			sem_post(&buffer_mutex);
			//printf("hehe\n");
			continue;
		}

		char tmp[1000];
		int tmpIndexConsumer = bufferIndexConsumer;
		strcpy(tmp, buffer[bufferIndexConsumer]);

		bufferIndexConsumer = (bufferIndexConsumer + 1) % N;
		isFull = false;
		if(bufferIndexProducer == bufferIndexConsumer){
			isEmpty = true;
		}

		sem_post(&buffer_mutex);

		printf("Przeczytales z indeksu: %i\n", tmpIndexConsumer);
		int len = strlen(tmp);
		switch(fossickingMode){
			case 0:{
				if(L >= len){
					printf("Moj index to: %i, a linijka prezentuje sie nastepujaco: %s", index, tmp);
				}
				break;
			}
			case 1:{
				if(L == len){
					printf("Moj index to: %i, a linijka prezentuje sie nastepujaco: %s", index, tmp);
				}
				break;
			}
			case 2:{
				if(L <= len){
					printf("Moj index to: %i, a linijka prezentuje sie nastepujaco: %s", index, tmp);
				}
				break;
			}
			default:{
				printf("Tryb wyszukiwania powinien byc liczba calkowita z przedzialu od 0 do 2!\n");
				break;
			}
		}

	}
	return NULL;
}

void * producing(void * arg){
	int index = *((int *) arg);
	while(true){
		//KORZYSTANIE ZE WSPOLNEJ PAMIECI
		sem_wait(&buffer_mutex);

		if(isFull){
			sem_post(&buffer_mutex);
			//printf("haha\n");
			continue;
		}
		//WARUNEK WYJSCIA
		char tmp[elementSize];
		char * result = fgets(tmp, elementSize, reader);
		int tmpIndexProducer = bufferIndexProducer;
		if(result == NULL && nk == 0){
			isRead = true;
			sem_post(&buffer_mutex);
			break;
		}
	
		strcpy(buffer[bufferIndexProducer], tmp);

		bufferIndexProducer = (bufferIndexProducer + 1) % N;
		isEmpty = false;
		if(bufferIndexProducer == bufferIndexConsumer){
			isFull = true;
		}
		//ZWALNIANIE WSPOLNEJ PAMIECI
		sem_post(&buffer_mutex);

		if(writingMode == 1){
			printf("Moj index to: %i. Zajeto miejsce %i. Wpisano: %s", index, tmpIndexProducer, tmp);
		}
	}
	return NULL;
}

void * checkTime(void * arg){
	startTime();
	while(true){
		double elapsedTime = endTime();
		if(elapsedTime > nk){
			for(int i = 0; i < P; i++){
				pthread_cancel(threadsProducer[i]);
			}
			for(int i = 0; i < K; i++){
				pthread_cancel(threadsConsumer[i]);
			}
			printf("Watki producentow i konsumentow zostaly wylaczone\n");
			break;
		}	
	}

	return NULL;
}

void intHandler(){
	printf("Otrzymano sygnal SIGINT - wylaczam sie!\n");
	exit(1);
}

int main(int argc, char** argv){
	//SPRAWDZANIE LICZBY ARGUMENTOW
	if(argc != 2){
		printf("Prosze podac nazwe pliku konfiguracyjnego\n");
		exit(1);
	}

	signal(SIGINT, intHandler);

	//OTWIERANIE PLIKU KONFIGURACYJNEGO
	confFile = fopen(argv[1], "r");
	if(confFile == NULL){
		perror("Otwieranie pliku konfiguracyjnego");
		exit(1);
	}

	//ZCZYTYWANIE ARGUMENTOW
	char * buf = NULL;
	size_t size = 0;
	getline(&buf, &size, confFile);
	P = atoi(buf);
	getline(&buf, &size, confFile);
	K = atoi(buf);
	getline(&buf, &size, confFile);
	N = atoi(buf);
	getline(&buf, &size, confFile);
	strcpy(fileName, buf);
	fileName[strcspn(fileName, "\n")] = 0;
	getline(&buf, &size, confFile);
	L = atoi(buf);
	getline(&buf, &size, confFile);
	fossickingMode = atoi(buf);
	getline(&buf, &size, confFile);
	writingMode = atoi(buf);
	getline(&buf, &size, confFile);
	nk = atoi(buf);

	//OTWIERANIE PLIKU Z TEKSTEM
	reader = fopen(fileName, "r");
	if(reader == NULL){
		perror("Otwieranie pliku tekstowego");
		exit(1);
	}	

	//printf("debug2\n");
	
	//ALOKOWANIE TABLICY BUFFORA
	buffer = (char **) malloc(N * sizeof(char *));
	for(int i = 0; i < N; i++){
		buffer[i] = (char *) calloc(elementSize, sizeof(char));
	}

	//INICJALIZACJA SEMAFORA
	sem_init(&buffer_mutex, 0, 1);

	//ODPALANIE WATKOW PRODOCENTOW
	threadsProducer = (pthread_t *) malloc (P * sizeof(pthread_t));
	for(int i = 0; i < P; i++){
		int *arg = malloc(sizeof(*arg));
		*arg = i;
		pthread_create(&threadsProducer[i], NULL, producing, (void *) arg);
	}

	//printf("debug4\n");

	//ODPALANIE WATKOW KONSUMENTOW
	threadsConsumer = (pthread_t *) malloc (K * sizeof(pthread_t));
	for(int i = 0; i < K; i++){
		int *arg = malloc(sizeof(*arg));
		*arg = i;
		pthread_create(&threadsConsumer[i], NULL, consuming, (void *) arg);
	}

	//printf("debug5\n");

	//ODPALANIE LICZNIKA
	if(nk != 0){
		pthread_create(&threadTimer, NULL, checkTime, NULL);
	}

	//CZEKANIE, AZ PRODUCENCI SKONCZA ROBOTE
	for(int i = 0; i < P; i++){
		pthread_join(threadsProducer[i], NULL);
	}

	//printf("debug6\n");

	//CZEKANIE, AZ KONSUMENCI SKONCZA ROBOTE
	for(int i = 0; i < K; i++){
		pthread_join(threadsConsumer[i], NULL);
	}

	//printf("debug7\n");

	//CZEKANIE, AZ TIMER ZAKONCZY ROBOTE
	if(nk != 0){
		pthread_join(threadTimer, NULL);
	}

	//ZWALNIANIE PAMIECI
	for(int i = 0; i < N; i++){
		free(buffer[i]);
	}
	free(buffer);

	free(threadsConsumer);
	free(threadsProducer);

	fclose(reader);

	//ZWALNIANIE SEMAFORA
	sem_close(&buffer_mutex);

	//printf("debug8\n");

	return 1;
}
