#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

typedef enum{
false,
true
} bool;

//BLOKOWANIE DOSTEPU DO PAMIECI WSPOLDZIELONEJ
#define QUEUE 0
//BLOKOWANIE PROCESU GOLIBRODY, KIEDY TEN NIE MA CO ROBIC
#define BARBER 1

#define INT_PROJ 5923
#define BEG_OF_QUEUE 5

//ZMIENNE GLOBALNE
/* aktualnie strzyzony klient */
int client;
int numberOfSeats;
/* adres na wspolny blok pamieci */
pid_t * memAdd;	
/* identyfikator wspolnego bloku pamieci */
int memId;
/* identyfikator semafora */
sem_t * semaphor[2];
key_t key;

//FUNKCJE POMOCNICZE
void eradication(){
	if(munmap(memAdd, (numberOfSeats + BEG_OF_QUEUE) * sizeof(pid_t) == -1)){
		perror("Odlaczanie od przestrzeni adresowej procesu");
	}
	if(shm_unlink("MEM") == -1){
		perror("Usuwanie pamieci");
	}
	if(shm_unlink("BARBER") == -1 || sem_unlink("QUEUE") == -1){
		perror("Usuwanie semaforow");
	}
}

long whatTimeIsNow(){
	struct timespec ts;
	if(clock_gettime(CLOCK_MONOTONIC, &ts) == -1){
		perror("getTime");
		exit(1);
	}
	
	return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

//FUNKCJE ZWIAZANIE Z KOLEJKA
bool queueEmpty(int * tab) {
	if(tab[1] == 0){
		return true;
	}
    return false;
}

bool queueFull(int * tab) {
	if(tab[0] - BEG_OF_QUEUE == tab[1]){
		return true;
	}
    return false;
}

pid_t queuePop(int * tab){
	if(queueEmpty(tab)){
		return -1;
	}
	
	int index = tab[2];
	tab[1]--;

	//ZMIANA POCZATKU KOLEJKI
	tab[2]++;
	if(tab[2] == tab[0]){
		tab[2] = BEG_OF_QUEUE;
	}
	return tab[index];
}

bool queuePush(int * tab, pid_t clientId) {
    if(queueFull(tab)){
		return false;
	}

	tab[tab[3]] = clientId;
	//ZMIANA KONCA KOLEJKI
    tab[3]++;
	if(tab[3] == tab[0]){
		tab[3] = BEG_OF_QUEUE;
	}

    tab[1]++;

    return true;
}

void queueWrite(int * tab){
	if(queueEmpty(tab)){
		printf("Kolejka pusta!\n");
	}
	else{
		int index = tab[2];
		for(int i = 0; i < tab[1]; i++){
			printf("%i <-", tab[index]);
			index++;
			if(index == tab[0]){
				index = BEG_OF_QUEUE;
			}
		}
		printf("\n");
		printf("%i\n", tab[BEG_OF_QUEUE]);

		index = tab[2];
		for(int i = 0; i < tab[1]; i++){
			printf("%i <-", index);
			index++;
			if(index == tab[0]){
				index = BEG_OF_QUEUE;
			}
		}
		printf("\n");
		printf("%i\n", tab[BEG_OF_QUEUE]);
	}
}

//FUNKCJE FRYZJERSKIE
void cutting(int clientId) {
    printf("Czas: %ld, Golibroda: Zaczynamy golic klienta o id %d.\n", whatTimeIsNow(), clientId);
    printf("Czas: %ld, Golibroda: Konczymy golic klienta o id %d.\n", whatTimeIsNow() ,clientId);
	kill(clientId, SIGRTMIN);
}

int main(int argc, char** argv){
	if(argc != 2){
		printf("Prosze podac jeden argument, ktory jest liczba miejsc w kolejce!!!\n");
		exit(1);
	}
	numberOfSeats = atoi(argv[1]);
	atexit(eradication);
	
	//TWORZENIE SEMAFOROW
	semaphor[BARBER] = sem_open("BARBER", O_CREAT | O_RDWR, 0777, 0);
	semaphor[QUEUE] = sem_open("QUEUE", O_CREAT | O_RDWR, 0777, 1);

	//INICJALIZACJA WARTOSCI
 	if(semaphor[BARBER] == SEM_FAILED ||
	semaphor[QUEUE] == SEM_FAILED){
		perror("Tworzenie semaforow");
		exit(1);
	}

	//TWORZENIE WSPOLNEGO BLOKU PAMIECI
    	memId = shm_open("MEM", O_CREAT | O_RDWR, 0777);
    	if(memId == -1)	{
		perror("Tworzenie wspolnego blokiu pamieci");
		exit(1);
	}
	//OKRESLANIE ROZMIARU
	if(ftruncate(memId, (numberOfSeats + BEG_OF_QUEUE) * sizeof(pid_t)) == -1){
		perror("Okreslanie rozmiaru");
		exit(1);
	}

	//DOLACZANIE SEGMENTU PAMIECI WSPOLNEJ
    	memAdd = mmap(NULL, (numberOfSeats + BEG_OF_QUEUE) * sizeof(pid_t), PROT_READ | PROT_WRITE, MAP_SHARED, memId, 0);
    	if(memAdd == (void *) -1) {
		printf("Dolaczanie segmentu pamieci wspolnej");
		exit(1);
	}

	/*
	0 - dlugosc calej kolejki
	1 - liczba klientow w kolejce
	2 - aktualny indeks poczatku kolejki
	3 - aktualny indeks konca kolejki
	4 - pid klienta, ktory obudzil golibrode
	*/
	memAdd[0] = numberOfSeats + BEG_OF_QUEUE;
	memAdd[1] = 0;
	memAdd[2] = BEG_OF_QUEUE;
	memAdd[3] = BEG_OF_QUEUE;

	//USYPIAMY	
	printf("Czas: %ld, Golibroda: Ide w kime.\n", whatTimeIsNow());
	sem_wait(semaphor[BARBER]);
	while(1){
		printf("Czas: %ld, Golibroda: Oho! Ktos puka do mych drzwi.\n", whatTimeIsNow());
		//TUTAJ BIERZEMY PIERWSZEGO KLIENTA, KTORY OBUDZIL NASZEGO GOLIBRODE
		sem_wait(semaphor[QUEUE]);
		client = memAdd[4];
		//SIADANIE NA FOTELU
		sem_post(semaphor[QUEUE]);
		cutting(client);
		
		//TERAZ PRZECHODZIMY DO OBSLUGI KOLEJKI AZ DO MOMENTU, GDY NIKOGO NIE BEDZIE
		while(1){
			sem_wait(semaphor[QUEUE]);
			client = queuePop(memAdd);
			sem_post(semaphor[QUEUE]);
			if(client == -1){
				printf("Czas: %ld, Golibroda: Ide w kime.\n", whatTimeIsNow());
				sem_wait(semaphor[BARBER]);
				sem_wait(semaphor[BARBER]);
				break;
			}
			else{
				printf("Czas: %ld, Golibroda: Zapraszam klienta nr %i.\n", whatTimeIsNow(), client);
			 	cutting(client);
			}
		}
	}

	return 1;
}
