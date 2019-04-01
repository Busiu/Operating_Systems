#include "lib.h"

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>

//TWORZENIE I USUWANIE TABLICY
void createNewArray(struct tablica * tab, int rt, int rb){
    tab->rozmiar_tablicy = rt;
    tab->rozmiar_bloku = rb;
    tab->dane = (char**) calloc(rt, sizeof(char*));
    char ** tmp = tab->dane;
    for(int i = 0; i < rt; i++){
        *tmp = (char*) calloc(rb, sizeof(char));
        tmp++;
    }
    tab->dlugosc = (int*) calloc(rt, sizeof(int));
    int * tmp2 = tab->dlugosc;
    for(int i = 0; i < rt; i++){
        *tmp2 = 0;
        tmp2++;
    }
}

void deleteArray(struct tablica * tab){
    char ** tmp = tab->dane;
    for(int i = 0; i < tab->rozmiar_tablicy; i++){
        free(*tmp);
        tmp++;
    }
    free(tab->dane);
    free(tab->dlugosc);
}

//DODAWANIE I USUWANIE ELEMENTOW
void createNewBlock(struct tablica * tab, int indeks, int dlugosc){
    //JEZELI INDEKS JEST ZA DUZY
    if(indeks >= tab->rozmiar_tablicy){
        printf("Gdzie tak pedzisz, co?\n");
        return;
    }

    char * buffor;
    char * tmp;

    buffor = (char*) calloc(dlugosc, sizeof(char));
    tmp = buffor;
    for(int i = 0; i < dlugosc; i++){
        *tmp = (char) (rand() % 26 + 65);
        tmp++;
    }
    tab->dane[indeks] = buffor;
    tab->dlugosc[indeks] = dlugosc;
}

void deleteBlock(struct tablica * tab, int indeks){
    //JEZELI INDEKS JEST ZA DUZY
    if(indeks >= tab->rozmiar_tablicy){
        printf("Gdzie tak pedzisz, co?\n");
        return;
    }

    tab->dane[indeks] = realloc(tab->dane[indeks], tab->rozmiar_bloku);
    tab->dlugosc[indeks] = 0;
}

//WYSZUKIWANIE BLOKU
char* findBlock(struct tablica * tab, char * slowo, int dlugosc) {
    //OBLICZANIE WARTOSCI DANEGO SLOWA
    int sum1 = 0;
    char * tmp1 = slowo;
    for (int i = 0; i < dlugosc; i++) {
        sum1 += (int) *tmp1;
        tmp1++;
    }

    //WYZNACZANIE PIERWSZEGO KANDYDATA
    int i = 0, roznica, sum2;
    char * kandydat = NULL;
    for(;i < tab->rozmiar_tablicy; i++){
        if(tab->dlugosc[i] != 0) {
            tmp1 = tab->dane[i];
	    sum2 = 0;
            for (int j = 0; j < tab->dlugosc[i]; j++) {
                sum2 += (int) *tmp1;
                tmp1++;
            }
            roznica = abs(sum1 - sum2);
            kandydat = tab->dane[i];
            break;
        }
    }

    //SPRAWDZANIE RESZTY
    for(;i < tab->rozmiar_tablicy; i++){
        if(tab->dlugosc[i] != 0){
            tmp1 = tab->dane[i];
            sum2 = 0;
            for (int j = 0; j < tab->dlugosc[i]; j++) {
                sum2 += (int) *tmp1;
                tmp1++;
            }
            if(abs(sum1 - sum2) < roznica){
                roznica = abs(sum1 - sum2);
                kandydat = tab->dane[i];
            }
        }
    }

    return kandydat;
}
