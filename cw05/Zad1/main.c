#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>

typedef int bool;
#define true 1
#define false 0

void execute(char * file_name){
    //INICJALIZACJA ZMIENNYCH
    FILE * file = fopen(file_name, "r");
    int max_size = 1024;
	int max_pipes = 16;
    int max_arguments = 16;
    char * line = (char*) calloc(max_size, sizeof(char));
    char * delimiter = "|";
	char * delimiter2 = " ";
	//TRZYMA W SOBIE WSZYSTKIE LINIE
    char * buffer[max_pipes];
	//TRZYMA W SOBIE WSZYSTKIE FUNCKJE Z DANEJ LINII
	char * buffer2[max_arguments];
	//BUFFOR DO ODCZYTANIA KONCOWEGO WYNIKU
	char result[2048];
    int numberOfFunctions;
	int numberOfArguments;
    int status;
	//PIPE'Y
	int ** fd;
	int ** tmp;
    pid_t child_pid;

    
    //JEZELI NIE UDALO SIE OTWORZYC PLIKU
    if(file == NULL){
		printf("Nie udalo sie otworzyc pliku!!!\n");
		return;
    }

    //WYKONYWANIE POLECEN Z PLIKU
    while(fgets(line, max_size, file) != NULL){
		numberOfFunctions = 0;
		buffer[numberOfFunctions] = strtok(line, delimiter);
		
		//ZCZYTYWANIE PROGRAMOW POMIEDZY PIPE'AMI
		while(buffer[numberOfFunctions] != NULL){
			numberOfFunctions++;
			if(numberOfFunctions == max_pipes - 1){
				printf("Maksymalna liczba funkcji w tych pipe'ach wynosi %i", max_pipes - 1);
				fclose(file);
				return;
			}
			buffer[numberOfFunctions] = strtok(NULL, delimiter);
		}	
		buffer[numberOfFunctions - 1][strlen(buffer[numberOfFunctions - 1]) - 1] = '\0';

		//TWORZENIE TABILCY PIPE'OW
		fd = (int**) calloc(numberOfFunctions, sizeof(int*));
		tmp = fd;
		for(int i = 0; i < numberOfFunctions; i++){
			*tmp = (int*) calloc(2, sizeof(int));
			tmp++;
		}

		//OTWIETANIE PIPE'OW
		for(int i = 0; i < numberOfFunctions; i++){
			pipe(fd[i]);
		}


		//TWORZENIE NOWYCH PROCESOW

		for(int i = 0; i < numberOfFunctions; i++){
			child_pid = fork();
			if(child_pid == 0){
				//ZAMYKANIE STRUMIENIA WEJSCIA
				close(fd[i][0]);

				//ZCZYTYWANIE PROGRAMOW
				numberOfArguments = 0;
				buffer2[numberOfArguments] = strtok(buffer[i], delimiter2);						
				while(buffer2[numberOfArguments] != NULL){
					numberOfArguments++;
					if(numberOfArguments == max_arguments){
						printf("Maksymalna liczba argumentow wynosi %i", max_arguments);
						return;
					}
					buffer2[numberOfArguments] = strtok(NULL, delimiter2);
				}
				buffer2[numberOfArguments + 1] = NULL;

				/*printf("HEHE %i funkcja teraz cisnie\n", i);
				for(int j = 0; j < numberOfArguments + 1; j++){
					printf("%s,\n", buffer2[j]);
				}*/

				//PRZEKIEROWYWANIE STRUMIENIA WEJSCIA NA WYJSCIE POPRZEDNIEGO DZIECKA
				//PIERWSZE ZCZYTUJE ZE STANDARDOWEGO WEJSCIA
				if(i != 0){
					dup2(fd[i - 1][0], STDIN_FILENO);
				}
				
				//PRZEKIEROWYWANIE STRUMIENIA WYJSCIA NA WEJSCIE KOLEJNEGO DZIECKA
				dup2(fd[i][1], STDOUT_FILENO);

				//WYKONYWANIE PLIKU
				execvp(buffer2[0], buffer2);
				exit(1);
				
			}
			else{	
				close(fd[i][1]);
				wait(&status);
				if(status != 0){
					printf("cos sie popsulo\n");
					return;
				}
				if(i == numberOfFunctions - 1){
					read(fd[i][0], &result, 2048);
					printf("%s", result);
				}
			}

		}

		//USUWANIE TABILCY PIPE'OW
		tmp = fd;
		for(int i = 0; i < numberOfFunctions; i++){
			free(*tmp);
			tmp++;
		}
		tmp = fd;
		free(tmp);
		
	
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
