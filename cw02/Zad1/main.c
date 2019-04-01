#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <sys/times.h>
#include <time.h>
#include <sys/stat.h>

struct tms start;
struct tms end;

void start_clock(){
    times(&start);
}

void end_clock(char * tryb){
    times(&end);
    printf("%s: ", tryb);
    printf("Czas uzytkownika: %jd ms, Czas systemowy: %jd ms\n", end.tms_utime - start.tms_utime, end.tms_stime - start.tms_stime);
}

int main(int argc, char ** argv) {

    if(strcmp(argv[1], "generate") == 0){
        //INICJALIZACJA ZMIENNYCH
        char * nazwa_pliku;
        nazwa_pliku = argv[2];
        int rozmiar_rekordu = atoi(argv[4]);
        int rozmiar_tablicy = atoi(argv[3]);

        //TWORZENIE RANDA
        int zarodek;
        time_t tt;
        zarodek = time(&tt);
        srand(zarodek);

        //TWORZENIE PLIKU
	creat(nazwa_pliku, S_IRUSR | S_IWUSR);

        //TWORZENIE REKORDOW
        char ** rekordy;
        char ** tmp;
        char * tmp2;
        rekordy = (char**) calloc(rozmiar_tablicy, sizeof(char*));
        tmp = rekordy;
        int i, j;
        for(i = 0; i < rozmiar_tablicy; i++){
            *tmp = (char*) calloc(rozmiar_rekordu, sizeof(char));
            tmp2 = *tmp;
            for(j = 0; j < rozmiar_rekordu - 1; j++){
                *tmp2 = (char) (rand() % 26 + 65);
                tmp2++;
            }
            *tmp2 = '\n';
            tmp++;
        }
        tmp = rekordy;

        //ZAPISYWANIE DO PLIKU;
        FILE * file = fopen(nazwa_pliku, "a+");
        for(i = 0; i < rozmiar_tablicy; i++){
            tmp2 = *tmp;
            fwrite(tmp2, 1, rozmiar_rekordu, file);
            //fwrite("\n", 1, 1, file);
            tmp++;
        }
        fclose(file);
    }

    if(strcmp(argv[1], "sort") == 0){
        //INICJALIZACJA ZMIENNYCH
        char * nazwa_pliku;
        char * tryb;
        nazwa_pliku = argv[2];
        tryb = argv[5];
        int rozmiar_rekordu = atoi(argv[4]);
        int rozmiar_tablicy = atoi(argv[3]);

        start_clock();

        if(strcmp(tryb, "lib") == 0){
            //OTWIERANIE PLIKU
            FILE * file = fopen(nazwa_pliku, "r+");

            //INICJALIZACJA ZMIENNYCH
            off_t offset;
            char * buffor1 = (char*) calloc(rozmiar_rekordu, sizeof(char));
            char * buffor2 = (char*) calloc(rozmiar_rekordu, sizeof(char));
            int i, j;

            //SORTOWANIE
            for(i = 1; i < rozmiar_tablicy; i++) {
                offset = (i - 1) * rozmiar_rekordu;
                fseek(file, offset, SEEK_SET);
                for (j = i - 1; j >= 0; j--) {
                    fread(buffor1, sizeof(char), rozmiar_rekordu, file);
                    fread(buffor2, sizeof(char), rozmiar_rekordu, file);
                    if ((int) buffor2[0] < (int) buffor1[0]){
                        fseek(file, offset, SEEK_SET);
                        fwrite(buffor2, sizeof(char), rozmiar_rekordu, file);
                        fwrite(buffor1, sizeof(char), rozmiar_rekordu, file);
                        offset -= rozmiar_rekordu;
                        fseek(file, offset, SEEK_SET);
                    }
                    else{
                        break;
                    }
                }
            }
            //ZAMYKANIE PLIKU
            fclose(file);
        }

        else if(strcmp(tryb, "sys") == 0){
            //OTWIERANIE PLIKU
            int file = open(nazwa_pliku, O_RDWR);

            //INICJALIZACJA ZMIENNYCH
            off_t offset;
            char * buffor1 = (char*) calloc(rozmiar_rekordu, sizeof(char));
            char * buffor2 = (char*) calloc(rozmiar_rekordu, sizeof(char));
            int i, j;

            //SORTOWANIE
            for(i = 1; i < rozmiar_tablicy; i++) {
                offset = (i - 1) * rozmiar_rekordu;
                lseek(file, offset, SEEK_SET);
                for (j = i - 1; j >= 0; j--) {
                    read(file, buffor1, rozmiar_rekordu);
                    read(file, buffor2, rozmiar_rekordu);
                    if ((int) buffor2[0] < (int) buffor1[0]){
                        lseek(file, offset, SEEK_SET);
                        write(file, buffor2, rozmiar_rekordu);
                        write(file, buffor1, rozmiar_rekordu);
                        offset -= rozmiar_rekordu;
                        lseek(file, offset, SEEK_SET);
                    }
                    else{
                        break;
                    }
                }
            }
            //ZAMYKANIE PLIKU
            close(file);
        }

        end_clock(tryb);

    }

    if(strcmp(argv[1], "copy") == 0){
        //INICJALIZACJA ZMIENNYCH
        char * nazwa_pliku1;
        char * nazwa_pliku2;
        char * tryb;
        nazwa_pliku1 = argv[2];
        nazwa_pliku2 = argv[3];
        tryb = argv[6];
        int rozmiar_rekordu = atoi(argv[5]);
        int rozmiar_tablicy = atoi(argv[4]);
        char * buffor1 = (char*) calloc(rozmiar_rekordu, sizeof(char));
        int i;
        if(strcmp(tryb, "lib") == 0){
            //OTWIERANIE PLIKOW
            FILE * file1 = fopen(nazwa_pliku1, "r");
            FILE * file2 = fopen(nazwa_pliku2, "wa+");

            //KOPIOWANIE
            for(i = 0; i < rozmiar_tablicy; i++){
                fread(buffor1, 1, rozmiar_rekordu, file1);
                fwrite(buffor1, 1, rozmiar_rekordu, file2);
            }

            //ZAMYKANIE PLIKOW
            fclose(file1);
            fclose(file2);
        }
        else if(strcmp(tryb, "sys") == 0){
            //OTWIERANIE PLIKOW
            int file1 = open(nazwa_pliku1, O_RDWR, S_IRUSR | S_IWUSR);
            int file2 = open(nazwa_pliku2, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

            //KOPIOWANIE
            for(i = 0; i < rozmiar_tablicy; i++){
                read(file1, buffor1, rozmiar_rekordu);
                write(file2, buffor1, rozmiar_rekordu);
            }

            //ZAMYKANIE PLIKOW
            close(file1);
            close(file2);
        }

	end_clock(tryb);
    }

    if(strcmp(argv[1], "read") == 0){
        //INICJALIZACJA ZMIENNYCH
        char * nazwa_pliku;
        nazwa_pliku = argv[2];
        int rozmiar_rekordu = atoi(argv[4]);
        int rozmiar_tablicy = atoi(argv[3]);

        //OTWIERANIE PLIKU
        FILE * file = fopen(nazwa_pliku, "r");

        //INICJALIZACJA ZMIENNYCH
        char * buffor1 = (char*) calloc(rozmiar_rekordu, sizeof(char));
        int i;

        //ZCZYTYWANIE ZMIENNYCH
        for(i = 0; i < rozmiar_tablicy; i++){
            fread(buffor1, 1, rozmiar_rekordu, file);
            printf(buffor1);
        }

        //ZAMYKANIE PLIKU
        fclose(file);
    }

    return 0;
}
