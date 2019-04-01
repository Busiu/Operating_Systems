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
bool isCut = false;
//LICZBA STRZYZEN KLIENTA
int cuts = 0;
int numberOfClients;
int numberOfCuts;
int * memAdd;
int memId;
int memSize;
int pleasedCustomers = 0;
int semId;
key_t key;
pid_t child;
sem_t * semaphor[2];

//FUNKCJE POMOCNICZE
long whatTimeIsNow(){
	struct timespec ts;
	if(clock_gettime(CLOCK_MONOTONIC, &ts) == -1){
		perror("getTime");
		exit(1);
	}
	
	return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

void cutHandler(int signum){
	isCut = true;
}

/*void sitHandler(int signum){
	printf("Czas: %ld, Klient o numerze %d: HOHO, ale wygodne krzeselko!\n", whatTimeIsNow(), getpid());
}*/

void intHandler(int signum){
	printf("Uzyto SIGINT - klienci uciekaja w poplochu!\n");
	exit(1);
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

void queueIndexes(int * tab){
	printf("%i, %i\n", tab[2], tab[3]);
}

//FUNKCJE FRYZJERSKIE
bool visitBarber(){
	bool status;
	sem_wait(semaphor[QUEUE]);
    int barberStatus;
	sem_getvalue(semaphor[BARBER], &barberStatus);	
    if(barberStatus == -1) {
        perror("Pobieranie semafora golibrody");
		exit(1);
    }

	//GDY GOLIBRODA NIC NIE ROBI
    if(barberStatus == 0) {
        printf("Czas: %ld, Klient o numerze %d: Fryzjer sie opierdziela, wiec wbijam!\n", whatTimeIsNow(), getpid());	
		//BUDZENIE GOLIBRODY - 2 RAZY, ABY SEMAFOR PO DEKREMENTACJI WYNIOSL 1
        sem_post(semaphor[BARBER]);
        sem_post(semaphor[BARBER]);
		//SIADAMY NA KRZESLE
		memAdd[4] = getpid();
        status = true;
    } else {
        if(!queuePush(memAdd,getpid())) {
            printf("Czas: %ld, Klient o numerze %d: Nie ma miejsca w kolejce, wiec musze wyjsc. :(\n", whatTimeIsNow(), getpid());
            status = false;
        } else {
            printf("Czas: %ld, Klient o numerze %d: Golibroda goli brode, ale przynajmniej jest miejsce w kolejce! :)\n", whatTimeIsNow(), getpid());
            status = true;
        }
    }

	sem_post(semaphor[QUEUE]);
    return status;
}

void exist(){
	while(cuts < numberOfCuts){
		//printf("%i\n", getpid());
		isCut = false;
		int succeded = visitBarber();
		if(succeded){
			while(!isCut){
				
			}
            cuts++;
            printf("Czas: %ld, Klient o numerze %d: HAHA, WPADL POD KOSIARE XD po raz %i\n", whatTimeIsNow(), getpid(), cuts);
		}
	}
	
	//ODLACZANIE SEMAFOROW
	if(sem_close(semaphor[BARBER]) == -1 || sem_close(semaphor[QUEUE]) == -1){
		perror("Odlaczanie semaforow");
		exit(1);
	}
	//ODLACZANIE PAMIECI WSPOLDZIELONEJ OD PROCESU
	if(munmap(memAdd, memSize * sizeof(pid_t)) == -1){
		perror("Odlaczanie pamieci wspoldzielonej od procesu");
		exit(1);
	}
}

int main(int argc, char** argv){	
	if(argc != 3){
		printf("Ale prosze nie podawac argumentow!!!\n");
		exit(1);
	}
	signal(SIGINT, intHandler);
	signal(SIGRTMIN, cutHandler);		

	numberOfClients = atoi(argv[1]);
	numberOfCuts = atoi(argv[2]);

	//DOLACZANIE SEMAFOROW
	semaphor[BARBER] = sem_open("BARBER", O_RDWR, 0777, 0);
	semaphor[QUEUE] = sem_open("QUEUE", O_RDWR, 0777, 1);
 	if(semaphor[BARBER] == SEM_FAILED ||
	semaphor[QUEUE] == SEM_FAILED){
		perror("Tworzenie semaforow");
		exit(1);
	}

	//OTWIERANIE WSPOLNEGO BLOKU PAMIECI
    memId = shm_open("MEM", O_RDWR, 0777);
    if(memId == -1)	{
		perror("Tworzenie wspolnego blokiu pamieci");
		exit(1);
	}
	
	//DOLACZANIE SEGMENTU PAMIECI WSPOLNEJ
    memAdd = mmap(NULL, sizeof(pid_t), PROT_READ | PROT_WRITE, MAP_SHARED, memId, 0);
    if(memAdd == (void *) -1) {
		printf("Dolaczanie segmentu pamieci wspolnej");
		exit(1);
	}
	//POBIERANIE WIELKOSCI TABLICY
	memSize = memAdd[0];
	//UPDATE
    memAdd = mmap(NULL, memSize * sizeof(pid_t), PROT_READ | PROT_WRITE, MAP_SHARED, memId, 0);
    if(memAdd == (void *) -1) {
		printf("Update segmentu pamieci wspolnej");
		exit(1);
	}

	//TWORZENIE KLIENTOW
	for(int i = 0; i < numberOfClients; i++){
		child = fork();
		if(child == 0){
			exist();
			exit(1);
		}
		else{
		}
	}

	//CZEKAMY, AZ WSZYSCY KLIENCI ZOSTANA OBSLUZENI JAK NALEZY
	while(pleasedCustomers < numberOfClients){
		wait(NULL);
        	pleasedCustomers++;
		printf("Czas: %ld, Zadowoleni klienci: %i\n", whatTimeIsNow(), pleasedCustomers);
	}
	printf("Czas: %ld, Wszyscy klienci sa w koncu zadowoleni ze swoich brod i fryzur! :D\n", whatTimeIsNow());

	return 1;
}
