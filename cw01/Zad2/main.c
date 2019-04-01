#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <stdint.h>
#include "lib.h"

struct tablica tab;

clock_t start_cz;
clock_t koniec_cz;
struct tms start;
struct tms koniec;

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

    for(int x = 0; x < liczba_operacji; x++){

        startClock();
        printf("%i\n", indeks);
        //TWORZENIE TABLICY
        if(strcmp(argv[indeks], "stworz") == 0){
            printf("Tworzenie nowej tablicy... \n");
            createNewArray(&tab, atoi(argv[indeks + 1]), atoi(argv[indeks + 2]));
            indeks += 3;
        }

        //WYPELNIENIE TABLICY
        else if(strcmp(argv[indeks], "wypelnij") == 0){
            printf("Wypelnianie tablicy... \n");
            char * buffor;
            char * tmp;

            for(int i = 0; i < (&tab)->rozmiar_tablicy; i++){
                buffor = (char*) calloc((&tab)->rozmiar_bloku, sizeof(char));
                tmp = buffor;
                for(int j = 0; j < (&tab)->rozmiar_bloku; j++){
                    *tmp = (char) (rand() % 26 + 65);
                    tmp++;
                }
                createNewBlock(&tab, i, (&tab)->rozmiar_bloku);
                free(buffor);
            }

            indeks += 1;
        }

        //NAJBARDZIEJ PODOBNY ELEMENT
        else if(strcmp(argv[indeks], "wyszukaj") == 0) {
            char *best = findBlock(&tab, argv[indeks + 1], atoi(argv[indeks + 2]));
            printf("Najbardziej podobny element do elementu \"%s\" to element: %s \n", argv[indeks + 1], best);
            indeks += 3;
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
                deleteBlock(&tab, i);
            }
            for(int i = lewy; i <= prawy; i++){
                createNewBlock(&tab, i, (&tab)->rozmiar_bloku);
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
                deleteBlock(&tab, i);
                createNewBlock(&tab, i, (&tab)->rozmiar_bloku);
            }
            indeks += 3;
        }

        endClock();
    }

    //deleteArray(&tab);
    
    return 0;
}
