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
char * buffor[maxLineSize];
char * delimiter = " ";
int currentIndex = 0;
int id;
int pidIndexes[maxNumberOfClients];
mqd_t queueIndexes[maxNumberOfClients];
char queueNames[maxNumberOfClients][20];
int size = sizeof(struct order);
int tmpPid;
int tmpWhere;
struct order order;
struct mq_attr attr;
struct timespec absTimeout;

int calc(char * type, int a, int b){
	if(strcmp(type, "ADD") == 0){
		return a + b;
	}
	if(strcmp(type, "SUB") == 0){
		return a - b;
	}
	if(strcmp(type, "MUL") == 0){
		return a * b;
	}
	if(strcmp(type, "DIV") == 0){
		return a / b;
	}
	return -69;
}	

void strmirror(char * a){
	int len = strlen(a);
	char tmp;
	for(int i = 0; i < len / 2; i++){
		tmp = a[i];
		a[i] = a[len - 1 - i];
		a[len - 1 - i] = tmp;
	}
	//printf("%s %i %i\n", a, len, (int)a[len-1]);
}

void quietus(){
	mq_close(id);
	mq_unlink(serverQueue);
	printf("Serwer usunal kolejke o IPC ID %i.\n", id);
	for(int i = 0; i < currentIndex; i++){
		printf("Wylaczamy klienta o numerze %i.\n", i);
		kill(pidIndexes[i] ,SIGINT);
	}
	exit(1);
}
void inthandler(){
	exit(1);
}

int main(int argc, char ** argv){

	//DEFINIUJEMY FUNKCJE KONCOWA
	atexit(quietus);
	signal(SIGINT, inthandler);

	//DEFINOWANIE ATRYBUTOW KOLEJKI
	attr.mq_flags = 0;
	attr.mq_maxmsg = maxSizeOfQueue;
	attr.mq_msgsize = size;
	attr.mq_curmsgs = 0;

	//TWORZENIE KOLEJKI SERWERA
	id = mq_open(serverQueue, O_RDWR | O_CREAT, 0755, &attr);
	if(id == -1){
		perror("Tworzenie kolejki serwera");
		exit(1);
	}

	while(1){
		mq_receive(id, (char *) &order, size, NULL);
		if(order.type == NEW){
			order.where = currentIndex;
			pidIndexes[currentIndex] = order.pid;

			//OTWIERANIE KOLEJKI KLIENTA
			queueIndexes[currentIndex] = mq_open(order.value, O_RDWR);
			if(queueIndexes[currentIndex] == -1){
				perror("Otwieranie kolejki klienta");
				//printf("%s\n", strerror(errno));
				exit(1);
			}
			strcpy(queueNames[currentIndex], order.value); 
			currentIndex++;

			mq_send(queueIndexes[currentIndex - 1], (char *) &order, size, 0);			
		}
		else if(order.type == CALC){
			tmpWhere = order.where;
			order.pid = getpid();

			//ZCZYTYWANIE ARGUMENTOW
			int i = 0;
			buffor[i] = strtok(order.value, delimiter);
			while(buffor[i] != NULL){
				i++;
				if(i == maxBufforSize){
					printf("Za duzo argow\n");
					break;
				}
				buffor[i] = strtok(NULL, delimiter);
			}
			order.where = calc(buffor[0], atoi(buffor[1]), atoi(buffor[2]));
			mq_send(queueIndexes[tmpWhere], (char *) &order, size, 0);			
		}
		else if(order.type == MIRROR){
			tmpWhere = order.where;
			order.pid = getpid();
			order.where = id;
			//KONKRETNA FUNKCJA
			strmirror(order.value);
			mq_send(queueIndexes[tmpWhere], (char *) &order, size, 0);			
		}	
		else if(order.type == TIME){
			tmpWhere = order.where;
			order.pid = getpid();
			order.where = id;
			//KONKRETNA FUNKCJA
			time_t period;
			struct tm *info;
			time(&period);
			info = localtime(&period);
			strftime(order.value, maxContentSize, "%x - %I:%M:%S:%p", info);
			mq_send(queueIndexes[tmpWhere], (char *) &order, size, 0);						
		}
		else if(order.type == END){
			break;
		}
		sleep(1);
	}
	while(1){
		absTimeout.tv_sec = 0;
		absTimeout.tv_nsec = 28;
		if(mq_timedreceive(id, (char *) &order, size, NULL, &absTimeout) < 0){
			printf("Kolejka jest juz pusta!!!\n");
			break;
		}
		if(order.type == NEW){
			order.where = currentIndex;
			pidIndexes[currentIndex] = order.pid;

			//OTWIERANIE KOLEJKI KLIENTA
			printf("%s\n", order.value);
			queueIndexes[currentIndex] = mq_open(order.value, O_RDWR);
			if(queueIndexes[currentIndex] == -1){
				perror("Otwieranie kolejki klienta");
				//printf("%s\n", strerror(errno));
				exit(1);
			}
			strcpy(queueNames[currentIndex], order.value); 
			currentIndex++;

			mq_send(queueIndexes[currentIndex - 1], (char *) &order, size, 0);			
		}
		else if(order.type == CALC){
			tmpWhere = order.where;
			order.pid = getpid();

			//ZCZYTYWANIE ARGUMENTOW
			int i = 0;
			buffor[i] = strtok(order.value, delimiter);
			while(buffor[i] != NULL){
				i++;
				if(i == maxBufforSize){
					printf("Za duzo argow\n");
					break;
				}
				buffor[i] = strtok(NULL, delimiter);
			}
			order.where = calc(buffor[0], atoi(buffor[1]), atoi(buffor[2]));
			mq_send(queueIndexes[tmpWhere], (char *) &order, size, 0);			
		}
		else if(order.type == MIRROR){
			tmpWhere = order.where;
			order.pid = getpid();
			order.where = id;
			//KONKRETNA FUNKCJA
			strmirror(order.value);
			mq_send(queueIndexes[tmpWhere], (char *) &order, size, 0);			
		}	
		else if(order.type == TIME){
			tmpWhere = order.where;
			order.pid = getpid();
			order.where = id;
			//KONKRETNA FUNKCJA
			time_t period;
			struct tm *info;
			time(&period);
			info = localtime(&period);
			strftime(order.value, maxContentSize, "%x - %I:%M:%S:%p", info);
			mq_send(queueIndexes[tmpWhere], (char *) &order, size, 0);						
		}
		else if(order.type == END){
			continue;
		}
		
	}	

    return 0;
}
