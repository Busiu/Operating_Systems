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
key_t key;
char * buffor[maxLineSize];
char * delimiter = " ";
int currentIndex = 0;
int id;
int indexes[maxNumberOfClients];
int size = sizeof(struct order) - sizeof(long);
int tmpPid;
int tmpQid;
struct msqid_ds stats;
struct order order;

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
	msgctl(id, IPC_RMID, NULL);
	printf("Serwer usunal kolejke o IPC ID %i.\n", id);
	for(int i = 0; i < currentIndex; i++){
		printf("Wylaczamy klienta o numerze %i.\n", i);
		kill(indexes[i] ,SIGINT);
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
	
	//TWORZENIE KLUCZA
	char * home = getenv("HOME");
	key = ftok(home, 7654);
	if(key == -1){
		perror("Tworzenie klucza");
		exit(1);
	}

	//TWORZENIE KOLEJKI SERWERA
	id = msgget(key,IPC_CREAT | 0600);

	while(1){
		msgrcv(id, &order, size, 0, 0);
		//printf("%s\n", order.type);
		if(order.type == NEW){
			tmpPid = order.pid;
			tmpQid = order.qid;
			order.pid = getpid();
			order.qid = currentIndex;
			indexes[currentIndex] = tmpPid;
			currentIndex++;
			msgsnd(tmpQid, &order, size, 0);			
		}
		else if(order.type == CALC){
			tmpQid = order.qid;
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
			order.qid = calc(buffor[0], atoi(buffor[1]), atoi(buffor[2]));
			msgsnd(tmpQid, &order, size, 0);			
		}
		else if(order.type == MIRROR){
			tmpQid = order.qid;
			order.pid = getpid();
			order.qid = id;
			//KONKRETNA FUNKCJA
			strmirror(order.value);
			msgsnd(tmpQid, &order, size, 0);			
		}	
		else if(order.type == TIME){
			tmpQid = order.qid;
			order.pid = getpid();
			order.qid = id;
			//KONKRETNA FUNKCJA
			time_t period;
			struct tm *info;
			time(&period);
			info = localtime(&period);
			strftime(order.value, maxContentSize, "%x - %I:%M:%S:%p", info);
			msgsnd(tmpQid, &order, size, 0);			
		}
		else if(order.type == END){
			break;
		}
		sleep(1);
	}
	while(1){
		if(msgrcv(id, &order, size, 0, IPC_NOWAIT) == -1){
			printf("Kolejka jest juz pusta!!!\n");
			break;
		}
		if(order.type == NEW){
			tmpQid = order.qid;
			order.pid = getpid();
			order.qid = currentIndex;
			indexes[currentIndex] = tmpQid;
			currentIndex++;
			msgsnd(tmpQid, &order, size, 0);			
		}
		else if(order.type == TIME){
			tmpQid = order.qid;
			order.pid = getpid();
			order.qid = id;
			//KONKRETNA FUNKCJA
			time_t period;
			struct tm *info;
			time(&period);
			info = localtime(&period);
			strftime(order.value, maxContentSize, "%x - %I:%M:%S:%p", info);
			msgsnd(tmpQid, &order, size, 0);			
		}
		else if(order.type == END){
			continue;
		}
		
	}	

    return 0;
}
