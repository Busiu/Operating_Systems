#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>

typedef int bool;
enum {false, true};

int L;
int type;
int numberOfSentSignals = 0;
int numberOfAcquiredSignalsByChild = 0;
int numberOfAcquiredSignalsByParent = 0;
pid_t mainPID;
pid_t child;
sigset_t sigset;

void intHandler(){
	printf("Zakanczam prace obu procesow...\n");
	kill(child, SIGINT);
	exit(1);
}

//TYPE 1
void type1Function1(){
	printf("sygnal1\n");
	//GDY PROCES GLOWNY OTRZYMA SYGNAL
	if(getpid() == mainPID){
	//NA RAZIE NIC NIE ROBIMY
		numberOfAcquiredSignalsByParent++;
		return;
	}
	//GDY DZIECKO OTRZYMA SYGNAL
	else{
		numberOfAcquiredSignalsByChild++;
		kill(getppid(), SIGUSR1);
	}
}
void type1Function2(){
	//TYLKO DZIECKO BEDZIE OTRZYMYWAC TEN SYGNAL
	//printf("sygnal2\n");
	numberOfAcquiredSignalsByChild++;
	printf("Liczba sygnalow otrzymanych przez dziecko: %i\n", numberOfAcquiredSignalsByChild);
	raise(SIGINT);
}
void type1(){
	signal(SIGUSR1, type1Function1);
	signal(SIGUSR2, type1Function2);
	
	child = fork();
	//DZIECKO
	if(child == 0){
		//TWORZYMY ZBIOR ZAWIERAJACY JEDYNE SYGNALY, KTORE MAJA BYC BRANE NA POWAZNIE
		sigfillset(&sigset);
		sigdelset(&sigset, SIGINT);
		sigdelset(&sigset, SIGUSR1);
		sigdelset(&sigset, SIGUSR2);
		sigprocmask(SIG_BLOCK, &sigset, NULL);
		for(int i = 0; i < 60; i++){
			sleep(1);
		}
		exit(1);
	}
	//RODZIC
	else{
		signal(SIGINT, intHandler);
		for(int i = 0; i < L; i++){
			kill(child, SIGUSR1);
			numberOfSentSignals++;
		}
		kill(child, SIGUSR2);
		numberOfSentSignals++;
		sleep(1);
		printf("Liczba sygnałow wyslanych: %i\nLiczba sygnalow otrzymanych przez rodzica: %i\n", 			numberOfSentSignals, numberOfAcquiredSignalsByParent);
	}
}
///////////////////////////////////

//TYPE 2
void type2Function1(){
	//printf("sygnal1\n");
	//GDY PROCES GLOWNY OTRZYMA SYGNAL
	if(getpid() == mainPID){
	//NA RAZIE NIC NIE ROBIMY
		numberOfAcquiredSignalsByParent++;
		return;
	}
	//GDY DZIECKO OTRZYMA SYGNAL
	else{
		numberOfAcquiredSignalsByChild++;
		kill(getppid(), SIGUSR1);
	}
}
void type2Function2(){
	//TYLKO DZIECKO BEDZIE OTRZYMYWAC TEN SYGNAL
	//printf("sygnal2\n");
	numberOfAcquiredSignalsByChild++;
	printf("Liczba sygnalow otrzymanych przez dziecko: %i\n", numberOfAcquiredSignalsByChild);
	raise(SIGINT);
}
void type2(){
	signal(SIGUSR1, type2Function1);
	signal(SIGUSR2, type2Function2);
	
	child = fork();
	//DZIECKO
	if(child == 0){
		//TWORZYMY ZBIOR ZAWIERAJACY JEDYNE SYGNALY, KTORE MAJA BYC BRANE NA POWAZNIE
		sigfillset(&sigset);
		sigdelset(&sigset, SIGINT);
		sigdelset(&sigset, SIGUSR1);
		sigdelset(&sigset, SIGUSR2);
		sigprocmask(SIG_BLOCK, &sigset, NULL);
		for(int i = 0; i < 60; i++){
			sleep(1);
		}
		exit(1);
	}
	//RODZIC
	else{
		signal(SIGINT, intHandler);
		for(int i = 0; i < L; i++){
			kill(child, SIGUSR1);
			numberOfSentSignals++;
			pause();
		}
		kill(child, SIGUSR2);
		numberOfSentSignals++;
		sleep(1);
		printf("Liczba sygnałow wyslanych: %i\nLiczba sygnalow otrzymanych przez rodzica: %i\n", 			numberOfSentSignals, numberOfAcquiredSignalsByParent);
	}
}
///////////////////////////////////

//TYPE 3
void type3Function1(){
	//printf("sygnal1\n");
	//GDY PROCES GLOWNY OTRZYMA SYGNAL
	if(getpid() == mainPID){
	//NA RAZIE NIC NIE ROBIMY
		numberOfAcquiredSignalsByParent++;
		return;
	}
	//GDY DZIECKO OTRZYMA SYGNAL
	else{
		numberOfAcquiredSignalsByChild++;
		kill(getppid(), SIGRTMIN);
	}
}
void type3Function2(){
	//TYLKO DZIECKO BEDZIE OTRZYMYWAC TEN SYGNAL
	//printf("sygnal2\n");
	numberOfAcquiredSignalsByChild++;
	printf("Liczba sygnalow otrzymanych przez dziecko: %i\n", numberOfAcquiredSignalsByChild);
	raise(SIGINT);
}
void type3(){
	signal(SIGRTMIN, type3Function1);
	signal(SIGRTMIN + 1, type3Function2);
	
	child = fork();
	//DZIECKO
	if(child == 0){
		//TWORZYMY ZBIOR ZAWIERAJACY JEDYNE SYGNALY, KTORE MAJA BYC BRANE NA POWAZNIE
		sigfillset(&sigset);
		sigdelset(&sigset, SIGINT);
		sigdelset(&sigset, SIGRTMIN);
		sigdelset(&sigset, SIGRTMIN + 1);
		sigprocmask(SIG_BLOCK, &sigset, NULL);
		for(int i = 0; i < 60; i++){
			sleep(1);
		}
		exit(1);
	}
	//RODZIC
	else{
		signal(SIGINT, intHandler);
		for(int i = 0; i < L; i++){
			kill(child, SIGRTMIN);
			numberOfSentSignals++;
			pause();
		}
		kill(child, SIGRTMIN + 1);
		numberOfSentSignals++;
		sleep(1);
		printf("Liczba sygnałow wyslanych: %i\nLiczba sygnalow otrzymanych przez rodzica: %i\n", 			numberOfSentSignals, numberOfAcquiredSignalsByParent);
	}
}
///////////////////////////////////



int main(int argc, char ** argv) {

	L = atoi(argv[1]);
	type = atoi(argv[2]);
	mainPID = getpid();
	printf("Wybrano tryb numer: %i\n", type);
	switch(type){
		case 1:
			type1();
			break;	
		case 2:
			type2();
			break;
		case 3:
			type3();
			break;
		default:
			printf("Dobry argument podales ;)\n");
			break;
	}

    return 0;
}
