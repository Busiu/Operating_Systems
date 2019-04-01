#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <unistd.h>

typedef int bool;
#define true 1
#define false 0

void execute(char * file_name, int cpu_limit, int memory_limit){
    //INICJALIZACJA ZMIENNYCH
    FILE * file = fopen(file_name, "r");
    int max_size = 1024;
    int max_arguments = 16;
    char * line = (char*) calloc(max_size, sizeof(char));
    char * delimiter = " ";
	char * buffer[max_arguments];
    char ** arguments;
    int i;
    int status;
    int flag;
    pid_t child_pid;
	struct rlimit rlp1;
	struct rlimit rlp2;
	//INFO O ZASOBACH PROCESU
	struct rusage rusage_before;
	struct rusage rusage_after;
    
    //JEZELI NIE UDALO SIE OTWORZYC PLIKU
    if(file == NULL){
		printf("Nie udalo sie otworzyc pliku!!!\n");
		return;
    }
//	printf("LOL\n");

    //WYKONYWANIE POLECEN Z PLIKU
    while(fgets(line, max_size, file) != NULL){
		i = 0;
		buffer[i] = strtok(line, delimiter);
		
		//ZCZYTYWANIE ARGUMENTOW
		while(buffer[i] != NULL){
			i++;
			if(i == max_arguments){
				printf("Maksymalna liczba argumentow to %i", max_arguments);
				fclose(file);
				return;
			}
			buffer[i] = strtok(NULL, delimiter);
		}
		arguments = (char**) calloc(i + 1, sizeof(char*));	
		buffer[i - 1][strlen(buffer[i - 1]) - 1] = '\0';
		for(int j = 0; j < i; j++){
			arguments[j] = buffer[j];	
		}
		arguments[i] = NULL;

		//TWORZENIE NOWEGO PROCESU
		child_pid = fork();
		if(child_pid == 0){
			//USTAWIANIE LIMITOW
			rlp1.rlim_cur = cpu_limit;
			rlp1.rlim_max = cpu_limit;
			rlp2.rlim_cur = 1024 * 1024 * memory_limit;
			rlp2.rlim_max = 1024 * 1024 * memory_limit;
			setrlimit(RLIMIT_CPU, &rlp1);
			setrlimit(RLIMIT_AS, &rlp2);
			//WYKONYWANIE POLECENIA
			flag = execvp(arguments[0], arguments);

			//JEZELI BLAD
			if(flag == -1){
				printf("Nie znaleziono polecenia!!!\n");
			}
			free(arguments);
			exit(1);
		}
		else{
			getrusage(RUSAGE_CHILDREN, &rusage_before);
			wait(&status);
			//WYPISYWANIE UZYWANYCH ZASOBOW
			getrusage(RUSAGE_CHILDREN, &rusage_after);
			printf("Polecenie: %s, czas uzytkownika: %jd ms, czas systemowy: %jd ms, maksymalne uzycie pamieci: %jd kB\n",
			arguments[0],
			(rusage_after.ru_utime.tv_usec - rusage_before.ru_utime.tv_usec) / 1000 +
			(rusage_after.ru_utime.tv_sec - rusage_before.ru_utime.tv_sec) * 1000,	
		 	(rusage_after.ru_stime.tv_usec - rusage_before.ru_stime.tv_usec) / 1000 +
			(rusage_after.ru_stime.tv_sec - rusage_before.ru_stime.tv_sec) * 1000,
			(rusage_after.ru_maxrss - rusage_before.ru_maxrss));
			if(status != 0){
				printf("Wystapil blad z poleceniem %s. Najprawdopodobniej przekroczono ktorys z zasobow!. Trwa zamykanie programu!!!\n", arguments[0]);
				free(arguments);
				return;
			}
			free(arguments);
		}
	
    }

}

int main(int argc, char ** argv){
    //JEZELI ZLA LICZBA ARGUMENTOW
    if(argc != 4){
	printf("Prosze podac trzy argumenty!!!\n");
	return 0;
    }

    char * file_name = argv[1];
	int cpu_limit = atoi(argv[2]);
	int memory_limit = atoi(argv[3]);

    //WYKONANIE FUNKCJI
    execute(file_name, cpu_limit, memory_limit);

    return 0;
}
