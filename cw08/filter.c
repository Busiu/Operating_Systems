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

#define max_size 100000
#define wyniki "Times.txt"

typedef enum{
false,
true
} bool;

//ZMIENNE GLOBALNE
FILE * picture;
FILE * filter;
FILE * filtered;
FILE * timeResults;
int ** pixelPicture;
double ** pixelFilter;
int ** pixelFiltered;
int * tmpResult;
//N - liczba wierszy, M - liczba kolumn, C - rozmiar filtru
int N, M, C;

char * delimiter = " ";
char line[max_size];

int numberOfThreads;
pthread_t * threads;

struct timeval t1;
struct timeval t2;
struct tms start;
struct tms end;

void startTime(){
    times(&start);
	gettimeofday(&t1, NULL);
}

void endTime(){
    times(&end);
	gettimeofday(&t2, NULL);
	double elapsedTime = t2.tv_sec - t1.tv_sec;
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000000.0;
	fprintf(timeResults,"Rozmiar filtru: %i, liczba watkow: %i\n", C, numberOfThreads);	
	fprintf(timeResults,"Czas rzeczywisty: %f s\n", elapsedTime);
    fprintf(timeResults,"Czas uzytkownika: %ld ms\n", end.tms_utime - start.tms_utime);
	fprintf(timeResults,"Czas systemowy: %ld ms\n", end.tms_stime - start.tms_stime);
	fprintf(timeResults,"\n");
}
int maximum(int a, int b, int ceil){
	int max = a;
	if(b > a) max = b;
	if(max > ceil) return ceil;
	return max;
}

void writeArray(double ** tab, int N, int M){
	for(int i = 0; i < N; i++){
		for(int j = 0; j < M; j++){
			printf("%f ", tab[i][j]);
		}
		printf("\n");
	}
}

//CALLOC WYPELNIA TABLICE ZERAMI, A MALLOC NIE -> MALLOC JEST SZYBSZY
double * detachLineDouble(char * line, int number){
	double * result;
	result = (double *) malloc (number * sizeof(double));
	result[0] = atof(strtok(line, delimiter));
	for(int i = 1; i < number; i++){
		result[i] = atof(strtok(NULL, delimiter));
	}
	return result;
}
int * detachLineInt(char * line, int number){
	int * result;
	result = (int *) malloc (number * sizeof(int));
	result[0] = atoi(strtok(line, delimiter));
	for(int i = 1; i < number; i++){
		result[i] = atoi(strtok(NULL, delimiter));
	}
	return result;
}

void * computing(void * arg){
	int index = *((int *) arg);
	for(int i = index; i < N; i += numberOfThreads){
		for(int j = 0; j < M; j++){
			//TUTAJ ZACZYNAMY OBLICZENIA DLA POSZCZEGOLNEGO PIKSELA
			pixelFiltered[i][j] = 0;
			for(int k = 0; k < C; k++){
				for(int l = 0; l < C; l++){
					//pixelFiltered[i][j] += round(pixelPicture[maximum(0, (int)(i - ceil(C/2.0) + k), C - 1)][maximum(0, (int)(j - ceil(C/2.0) + l), C - 1)] * pixelFilter[i][j]);
					pixelFiltered[i][j] += pixelPicture[maximum(0, (int)(i - ceil(C/2.0) + k), C - 1)][maximum(0, (int)(j - ceil(C/2.0) + l), C - 1)] * pixelFilter[k][l];
				}			
			}
			pixelFiltered[i][j] = round(pixelFiltered[i][j]);
		}
	}
	free(arg);
	
	return NULL;
}

int main(int argc, char** argv){
	//SPRAWDZANIE LICZBY ARGUMENTOW
	if(argc != 5){
		printf("Prosze podac 4 argumenty i wrocic ponownie!\n");
		exit(1);
	}

	//OTWIERANIE OBRAZU, FILTRU I PLIKU Z WYNIKAMI CZASOWYMI
	numberOfThreads = atoi(argv[1]);
	picture = fopen(argv[2], "r");
	if(picture == NULL){
		perror("Otwieranie obrazu");
		exit(1);
	}	
	filter = fopen(argv[3], "r");
	if(filter == NULL){
		perror("Otwieranie filtru");
		exit(1);
	}
	timeResults = fopen(wyniki, "ab+");
	if(timeResults == NULL){
		perror("Otwieranie pliku z wynikami czasowymi");
		exit(1);
	}

	//WCZYTYWANIE OBRAZU
	int currentLine = 0;
	while(fgets(line, max_size, picture) != NULL){
		if(currentLine == 1){
			int * tmp = detachLineInt(line, 2);
			M = tmp[0];
			N = tmp[1];

			pixelPicture = (int **) malloc (N * sizeof(int *));
		}
		else if (currentLine > 2 && currentLine < N + 3){
			int * tmp = detachLineInt(line, M);
			pixelPicture[currentLine - 3] = (int *) malloc (N * sizeof(int));
			pixelPicture[currentLine - 3] = tmp;
		}
		currentLine++;
	}

	//WCZYTYWANIE FILTRU
	currentLine = 0;
	while(fgets(line, max_size, filter) != NULL){
		if(currentLine == 0){
			int * tmp = detachLineInt(line, 1);
			C = tmp[0];

			pixelFilter = (double **) malloc (C * sizeof(double *));
		}
		else if (currentLine > 0 && currentLine < C + 1){
			double * tmp = detachLineDouble(line, C);
			pixelFilter[currentLine - 1] = (double *) malloc (C * sizeof(double));
			pixelFilter[currentLine - 1] = tmp;
		}
		currentLine++;
	}

	//TWORZENIE TABLICY DLA WYNIKU OPERACJI FILTROWANIA
	pixelFiltered = (int **) calloc (N , sizeof(int *));
	for(int i = 0; i < N; i++){
		pixelFiltered[i] = (int *) calloc (M , sizeof(int));
	}
	
	//ZACZYNAMY ODMIERZAC CZAS
	startTime();

	//ODPALANIE WATKOW
	threads = (pthread_t *) malloc (numberOfThreads * sizeof(pthread_t));
	for(int i = 0; i < numberOfThreads; i++){
		int *arg = malloc(sizeof(*arg));
		*arg = i;
		pthread_create(&threads[i], NULL, computing, (void *) arg);
	}

	//CZEKANIE, AZ SKONCZA ROBOTE
	for(int i = 0; i < numberOfThreads; i++){
		pthread_join(threads[i], NULL);
	}

	//KONCZYMY MIERZYC CZAS
	endTime();	

	//OTWIERANIE PLIKU WYNIKOWEGO
	filtered = fopen(argv[4], "w");
	if(filtered == NULL){
		perror("Otwieranie pliku wynikowego");
		exit(1);
	}

	//ZAPIS DO PLIKU WYNIKOWEGO
	fprintf(filtered, "P2\n");
	fprintf(filtered, "%i %i\n", M, N);
	fprintf(filtered, "255\n");
	for(int i = 0; i < N; i++){
		for(int j = 0; j < M; j++){
			fprintf(filtered, "%i ", pixelFiltered[i][j]);
		}
		fprintf(filtered, "\n");
	}

	//ZWALNIANIE ZASOBOW - OBRAZY
	for(int i = 0; i < N; i++){
		free(pixelPicture[i]);
		free(pixelFiltered[i]);
	}
	free(pixelPicture);
	free(pixelFiltered);

	//ZWALNIANIE ZASOBOW - FILTR
	for(int i = 0; i < C; i++){
		free(pixelFilter[i]);
	}
	free(pixelFilter);

	return 1;
}
