#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
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
struct order order;
struct mq_attr attr;
bool hasRead;
int clientId;
int clientNumber;
int i;
int serverId;
int size = sizeof(struct order);
char argument[maxContentSize];
char * buffor[maxBufforSize];
char clientQueue[maxContentSize];
char * delimiter = " ";
char * fileName;
char line[maxLineSize];
char key[10];
FILE * file;

void quietus(){
	mq_close(clientId);
	mq_unlink(clientQueue);
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

	//DEFINOWANIE ATRYBUTOW KOLEJKI
	attr.mq_flags = 0;
	attr.mq_maxmsg = maxSizeOfQueue;
	attr.mq_msgsize = size;
	attr.mq_curmsgs = 0;

	//TWORZENIE KOLEJKI KLIENTA
	memset(clientQueue, 0, maxContentSize);
	sprintf(clientQueue, "/%d", getpid());
	clientId = mq_open(clientQueue, O_RDWR | O_CREAT, 0755, &attr);
	if(clientId == -1){
		perror("Tworzenie kolejki klienta");
		//printf("%s\n", strerror(errno));
		exit(1);
	}

	//OTWIERANIE ISTNIEJACEJ JUZ KOLEJKI SERWERA
	serverId = mq_open(serverQueue, O_WRONLY, 0644, &attr);
	if(serverId == -1){
		perror("Otwieranie istniejacej juz kolejki serwera");
		exit(1);
	}

	//WYSYLAMY INFO, ZE ISTNIEJEMY DO SERWERA
	order.type = NEW;
	strcpy(order.value, clientQueue);
	order.where = clientId;
	order.pid = getpid();
	mq_send(serverId, (char *) &order, size, 0);
	mq_receive(clientId, (char *) &order, size, NULL);
	clientNumber = order.where;
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
		order.where = clientNumber;
		order.pid = getpid();
		strcpy(order.value, argument);
		mq_send(serverId, (char *)&order, size, 0);
	
		//JEZELI CHCEMY ZAKONCZYC
		if(order.type == END){
			break;
		}

		//PROBA ZCZYTANIA
		hasRead = false;
		for(int j = 0; j < 20; j++){
			if(mq_receive(clientId, (char *) &order, size, NULL) == -1){
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
			printf("%i\n", order.where);
		}
		else{
			printf("Nie dalem rady zczytac, sry :( \n");
		}
	}

	//ZAMYKANIE PLIKU Z KOMUNIKATAMI
	fclose(file);

    return 0;
}
