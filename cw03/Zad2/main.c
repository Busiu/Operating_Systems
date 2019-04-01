#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

typedef int bool;
#define true 1
#define false 0

void execute(char * file_name){
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
			wait(&status);
			if(status != 0){
				printf("Wystapil blad z poleceniem %s. Trwa zamykanie programu!!!\n", arguments[0]);
				return;
			}
		}
	
    }

}

int main(int argc, char ** argv){
    //JEZELI ZLA LICZBA ARGUMENTOW
    if(argc != 2){
	printf("Prosze podac jeden argument!!!\n");
	return 0;
    }

    char * file_name = argv[1];

    //WYKONANIE FUNKCJI
    execute(file_name);

    return 0;
}
