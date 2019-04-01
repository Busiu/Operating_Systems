#ifndef ZAD1A_LIBRARY_H
#define ZAD1A_LIBRARY_H

struct tablica{
    char ** dane;
    int * dlugosc;
    int rozmiar_tablicy;
    int rozmiar_bloku;
};

//TWORZENIE I USUWANIE TABLICY
void createNewArray(struct tablica * tab, int rt, int rb);
void deleteArray(struct tablica * tab);

//DODAWANIE I USUWANIE ELEMENTOW
void createNewBlock(struct tablica * tab, int indeks, int dlugosc);
void deleteBlock(struct tablica * tab, int indeks);

//WYSZUKIWANIE BLOKU
char* findBlock(struct tablica * tab, char * slowo, int dlugosc);

#endif