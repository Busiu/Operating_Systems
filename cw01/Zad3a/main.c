#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <stdint.h>
#include <time.h>
#include "lib.h"

struct tablica tab;
struct tablica * pointer = &tab;

clock_t start_cz;
clock_t koniec_cz;
struct tms start;
struct tms koniec;

char * wybraniec;

void startClock(){
    start_cz = times(&start);
}

void endClock(){
    koniec_cz = times(&koniec);
    printf("Czas rzeczywisty: %jd ms\n", koniec_cz - start_cz);
    printf("Czas systemowy: %jd ms\n", koniec.tms_stime - start.tms_stime);
    printf("Czas uzytkownika: %jd ms\n", koniec.tms_utime - start.tms_utime);
}

int main(int argc, char ** argv) {
    if(argc < 5){
        printf("Prosze podac wiecej argumentow!!!\n");
        return 0;
    }
    int liczba_operacji = atoi(argv[1]);
    int indeks = 2;

    //USTAWIANIE RANDA
    int zarodek;
    time_t tt;
    zarodek = time(&tt);
    srand(zarodek);

    for(int x = 0; x < liczba_operacji; x++){

        startClock();
        //TWORZENIE TABLICY
        if(strcmp(argv[indeks], "stworz") == 0){
            printf("Tworzenie nowej tablicy... \n");
            createNewArray(pointer, atoi(argv[indeks + 1]), atoi(argv[indeks + 2]));
            indeks += 3;
		
	    //STWORZENIE WYBRANCA DO FUNKCJI "FIND BLOCK"
	    wybraniec = (char*) calloc(pointer->rozmiar_bloku, sizeof(char));
	    char * tmp = wybraniec;
	    for(int i = 0; i < pointer->rozmiar_bloku; i++){
	        *tmp = (char) (rand() % 26 + 65);
	        tmp++;
	    }	
        }

        //WYPELNIENIE TABLICY
        else if(strcmp(argv[indeks], "wypelnij") == 0){
            printf("Wypelnianie tablicy... \n");
            for(int i = 0; i < pointer->rozmiar_tablicy; i++){
                createNewBlock(pointer, i, pointer->rozmiar_bloku);
            }

            indeks += 1;
        }

        //NAJBARDZIEJ PODOBNY ELEMENT
        else if(strcmp(argv[indeks], "wyszukaj") == 0) {
            char *best = findBlock(pointer, wybraniec, pointer->rozmiar_bloku);
            printf("Najbardziej podobny element do elementu o nazwie \"wybraniec\" to element: %s \n", best);
            indeks += 1;
        }

        //USUNIECIE I DODANIE KOLEJNO
        else if(strcmp(argv[indeks], "kolejno") == 0){
            int lewy = atoi(argv[indeks + 1]);
            int prawy = atoi(argv[indeks + 2]);
            printf("Usuniecie i dodanie kolejno bloków z przedziału [%i,%i] \n", lewy, prawy);

            if(lewy > prawy){
                printf("Zle ustawione przedzialy kolego ;) \n");
                break;
            }

            for(int i = lewy; i <= prawy; i++){
                deleteBlock(pointer, i);
            }
            for(int i = lewy; i <= prawy; i++){
                createNewBlock(pointer, i, pointer->rozmiar_bloku);
            }
            indeks += 3;
        }

        //USUNIECIE I DODANIE NA PRZEMIAN
        else if(strcmp(argv[indeks], "na_przemian") == 0){
            int lewy = atoi(argv[indeks + 1]);
            int prawy = atoi(argv[indeks + 2]);
            printf("Usuniecie i dodanie na przemian bloków z przedziału [%i,%i] \n", lewy, prawy);

            if(lewy > prawy){
                printf("Zle ustawione przedzialy kolego ;) \n");
                break;
            }

            for(int i = lewy; i <= prawy; i++){
                deleteBlock(pointer, i);
                createNewBlock(&tab, i, pointer->rozmiar_bloku);
            }
            indeks += 3;
        }

        endClock();
    }

    deleteArray(&tab);
    
    return 0;
}
