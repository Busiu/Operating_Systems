#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "naglowkowy.h"

//ZMIENNE
struct msqid_ds stats;
struct order order;
bool hasRead;
int clientId;
int clientNumber;
int i;
int serverId;
int size = sizeof(struct order) - sizeof(long);
char argument[maxContentSize];
char * buffor[maxBufforSize];
char * delimiter = " ";
char * fileName;
char line[maxLineSize];
FILE * file;
key_t key;

void quietus(){
	msgctl(clientId ,IPC_RMID, NULL);
	printf("Klient usunal kolejke o IPC ID %i.\n", clientId);
	exit(1);
}

void inthandler(){
	exit(1);
}

int main(int argc, char ** argv){

	//DEFINIUJEMY FUNKCJE KONCOWA
	atexit(quietus);
	signal(SIGINT, inthandler);

	//NAZWA PLIKU Z ARGUMENTAMI
	fileName = argv[1];
	
	//TWORZENIE KLUCZA
	char * home = getenv("HOME");

	//TWORZENIE KOLEJKI KLIENTA
	key = ftok(home, getpid()%200);
	if(key == -1){
		perror("Tworzenie kolejki klienta");
		exit(1);
	}
	clientId = msgget(key,IPC_CREAT | IPC_PRIVATE | 0600);

	//OTWIERANIE ISTNIEJACEJ JUZ KOLEJKI SERWERA
	key = ftok(home, 7654);
	if(key == -1){
		perror("Otwieranie istniejacej juz kolejki serwera - klucz");
		exit(1);
	}

	serverId = msgget(key, 0600);
	if(serverId == -1){
		perror("Otwieranie istniejacej juz kolejki serwera - otwarcie");
		exit(1);
	}
	//WYSYLAMY INFO, ZE ISTNIEJEMY DO SERWERA
	order.type = NEW;
	strcpy(order.value, "HEHE");
	order.qid = clientId;
	order.pid = getpid();
	msgsnd(serverId, &order, size, 0);
	msgrcv(clientId, &order, size, 0, 0);
	clientNumber = order.qid;
	printf("Jestem %i klientem z kolei :)\n", clientNumber);

	//OTWIERANIE PLIKU Z KOMUNIKATAMI
	file = fopen(fileName, "r+");
	
	if(file == NULL){
		printf("Cos sie zjebalo\n");
	}

	//WYSYLAMY ZA JEDNYM RAZEM TYLKO JEDNO ZLECENIE
	while(fgets(line, maxLineSize, file) != NULL){
		i = 0;
		buffor[i] = strtok(line, delimiter);
	
		//ZCZYTYWANIE ARGUMENTOW
		while(buffor[i] != NULL){
			i++;
			if(i == maxBufforSize){
				printf("Za duzo argow\n");
				break;
			}
			buffor[i] = strtok(NULL, delimiter);
		}
		buffor[i - 1][strlen(buffor[i - 1]) - 1] = '\0';

		if(strcmp(buffor[0], "NEW") == 0){
			order.type = NEW;
		}
		else if(strcmp(buffor[0], "CALC") == 0){
			order.type = CALC;
		}
		else if(strcmp(buffor[0], "MIRROR") == 0){
			order.type = MIRROR;
		}
		else if(strcmp(buffor[0], "TIME") == 0){
			order.type = TIME;
		}
		else if(strcmp(buffor[0], "END") == 0){
			order.type = END;
		}
		else{
			printf("Nadeslany typ: %s\n", buffor[0]);
			exit(1);
		}
		//ZAPISYWANIE ARGUMENTOW DO JEDNEGO STRINGA
		strcpy(argument, "");
		for(int j = 1; j < i; j++){
			strcat(argument, buffor[j]);
			if(j != i - 1)
				strcat(argument, " ");
		}
		order.qid = clientId;
		order.pid = getpid();
		strcpy(order.value, argument);
		msgsnd(serverId, &order, size, 0);
	
		//JEZELI CHCEMY ZAKONCZYC
		if(order.type == END){
			break;
		}

		//PROBA ZCZYTANIA
		hasRead = false;
		for(int j = 0; j < 20; j++){
			if(msgrcv(clientId, &order, size, 0, IPC_NOWAIT) == -1){
				sleep(1);
			}
			else{
				hasRead = true;
				break;
			}		
		}
		//WYPISYWANIE WARTOSCI
		if(hasRead && order.type != CALC){
			printf("%s\n", order.value);
		}
		else if(hasRead){
			printf("%i\n", order.qid);
		}
		else{
			printf("Nie dalem rady zczytac, sry :( \n");
		}
	}

	//ZAMYKANIE PLIKU Z KOMUNIKATAMI
	fclose(file);

    return 0;
}
