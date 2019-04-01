#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>

typedef int bool;
enum {false, true};

/*	3 argument podawany jest w postaci xxxxx, gdzie x = t lub x = n.
	I-ty x z kolei odpowiada za obsluge i-tej funkcjonalnosci opisanej w tresci zadania.
*/

//INICJALIZACJA ZMIENNYCH GLOBALNYCH

//LICZBA OTRZYMANYCH ZAPYTAN
int numberOfAsks = 0;
//LICZBA ZAKONCZONYCH PROCESOW
int numberOfTerminations = 0;
//ID RODZICA 
int parentID;
//CZY WYSLANO POZWOLENIE POTOMKOWI?
bool isAllowed = false;
//PARAMETRY PROGRAMU
int N,M;
//FUNKCJONALNOSCI
char func[5];
//PID PROCESOW POTOMNYCH, KTORE WYSLALY ZAPYTANIE I CZEKAJA NA "WIECEJ CHETNYCH"
pid_t * childPID;
//SIGACTION
struct sigaction act;

//WYKORZYSTAMY JEDNA FUNKCJE NA OBSLUGE SIGUSR1 U POTOMKA I RODZICA
void sendClearance(int sigNum, siginfo_t * siginfo, void * context){

	//printf("%i\n", siginfo->si_pid);
	
	if(getpid() != parentID){
		isAllowed = true;
		return;
	}
	//WYPISYWANIE FUNKCJONALNOSCI
	if(func[1] == 't'){
		printf("Otrzymano prosbe o zezwolenie na dzialanie od procesu potomnego o ID: %i\n", siginfo->si_pid);
	}
	numberOfAsks++;
	if(numberOfAsks < M){
		childPID[numberOfAsks - 1] = siginfo->si_pid;
		return;
	}
	else if(numberOfAsks == M){
		childPID[numberOfAsks - 1] = siginfo->si_pid;
		for(int i = 0; i < M; i++){
			//WYPISYWANIE FUNKCJONALNOSCI
			if(func[2] == 't'){
				printf("Wyslano zezwolenie do procesu potomnego o ID: %i\n", childPID[i]);
			}
			kill(childPID[i], SIGUSR1);
		}
	}
	else{
		//WYPISYWANIE FUNKCJONALNOSCI
		if(func[2] == 't'){
			printf("Wyslano zezwolenie do procesu potomnego o ID: %i\n", siginfo->si_pid);
		}
		kill(siginfo->si_pid, SIGUSR1);
	}
		
}

void receiveRTSignal(int sigNum, siginfo_t * siginfo, void * context){
	numberOfTerminations++;
	//WYPISYWANIE FUNKCJONALNOSCI
	if(func[3] == 't'){
		printf("Otrzymano sygnal czasu rzeczystego o ID %i\n", siginfo->si_signo);
	}
}

void interupt(){
	if(getpid() == parentID){
		printf("Trwa zakanczanie pracy...\n");
		for(int i = 0; i < N; i++){
			kill(childPID[i], SIGINT);
		}
		exit(1);
	}
	else{
		exit(1);
	}
}

int main(int argc, char ** argv) {
	//INICJALIZACJA ZMIENNYCH
	if(argc < 3){
		printf("No ale bez argumentow to ciezko bedzie :(/n");
		return 0;
	}
	//DOMYSLNIE CHCEMY WYSWIETLAC WSZYSTKO
	if(argc == 3){
		for(int i = 0; i < 5; i++){
			func[i] = 't';
		}
	}
	//ZCZYTYWANIE FUNKCJONALNOSCI
	if(argc > 3){
		for(int i = 0; i < 5; i++){
			func[i] = argv[3][i];
		}
	}
	N = atoi(argv[1]);
	M = atoi(argv[2]);
	int sleepTime;
	childPID = (int*) calloc(N, sizeof(int));
	parentID = getpid();

	for(int i = 0; i < 32; i++){
		act.sa_flags = SA_SIGINFO;
		act.sa_sigaction = receiveRTSignal;
		sigemptyset(&act.sa_mask);
		sigaction(SIGRTMIN + i, &act, NULL);
	}

	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = sendClearance;
	sigemptyset(&act.sa_mask);
	sigaction(SIGUSR1, &act, NULL);

	signal(SIGINT, interupt);

	for(int i = 0; i < N; i++){
		childPID[i] = fork();
		//POTOMEK
		if(childPID[i] == 0){
			sleep(i);
			srand(time(NULL) + i);
			sleepTime = 4;

			//printf("HEHE MLODY\n");

			//SPANKO
			sleep(sleepTime);

			//WYSYLANIE POZWOLENIA
			kill(getppid(), SIGUSR1);

			//CZEKANIE NA UZYSKANIE POZWOLENIA
			while(1){
				//printf("%i\n", getpid());
				if(isAllowed){
					break;
				}
				sleep(1);
			}

			//WYSYLANIE LOSOWEGO SYGNALU
			kill(getppid(), SIGRTMIN + rand() % 10);
			
			//WYPISYWANIE FUNKCJONALNOSCI
			if(func[4] == 't'){
				printf("Proces potomny o ID %i spal przez %i sekund\n", getpid(), sleepTime);
			}
						
			//ZAMYKANIE PROCESU
			exit(1);
		}
		//RODZIC
		else{
			//WYPISYWANIE FUNKCJONALNOSCI
			if(func[0] == 't'){
				printf("Stworzono proces potomny o ID: %i\n", childPID[i]);
			}
		}
	}

	while(numberOfTerminations < N){
		sleep(1);
	}
	

    return 0;
}
