#include <stdio.h>
#include <dirent.h>
#include <malloc.h>
#include <memory.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>

typedef int bool;
#define true 1
#define false 0

const int SecondsPerMinute = 60;
const int SecondsPerHour = 3600;
const int SecondsPerDay = 86400;
const int DaysOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

bool IsLeapYear(short year)
{
    if (year % 4 != 0) return false;
    if (year % 100 != 0) return true;
    return (year % 400) == 0;
}

time_t convertTime(int year, int month, int day, int hour, int minute, int second)
{
    time_t secs = 0;
    for (int y = 1970; y < year; y++)
        secs += (IsLeapYear(y)? 366: 365) * SecondsPerDay;
    for (int m = 1; m < month; m++) {
        secs += DaysOfMonth[m - 1] * SecondsPerDay;
        if (m == 2 && IsLeapYear(year)) secs += SecondsPerDay;
    }
    secs += (day - 1) * SecondsPerDay;
    secs += (hour - 1) * SecondsPerHour;
    secs += minute * SecondsPerMinute;
    secs += second;
    return secs;
}




void seekingThroughDirectory(char * path, char * argument, int * date){
    //INICJALIZOWANIE ZMIENNYCH
    struct stat stat;
    char * fullPath = calloc(1000, sizeof(char));
    off_t fileSize;
    time_t lastMod;
    time_t dateArgument = convertTime(date[0], date[1], date[2], date[3], date[4], date[5]);
    mode_t permission;
    char * symbol;

    //OTWIERANIE KATALOGU
    DIR * dir = opendir(path);
    if(dir == NULL){
        return;
    }

    struct dirent* dirent =  readdir(dir);

    while(dirent != NULL){
        //SCIEZKA PLIKU
        strcpy(fullPath, path);
        strcat(fullPath, "/");
        strcat(fullPath, dirent->d_name);

        //INICJALIZACJA STAT
        lstat(fullPath, &stat);

        //POBIERANIE DANYCH O PLIKU
        fileSize = stat.st_size;
        lastMod = stat.st_mtime;
        permission = stat.st_mode;

	//printf("%jd %jd\n", dateArgument, lastMod);

        //SPRAWDZENIE WARUNKOW ARGUMENTOW
        if(dateArgument > lastMod){
            symbol = "mniejsze";
        }
        else if(dateArgument < lastMod){
            symbol = "wieksze";
        }
        else{
            symbol = "rowne";
        }

        //SPRAWDZANIE, CZY KATALOG KROPKOWY
        if(strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0){
            dirent = readdir(dir);
            continue;
        }

        //SPRAWDZANIE, CZY KATALOG
        if(S_ISDIR(permission)){
            seekingThroughDirectory(fullPath, argument, date);
        }

        //WYPISYWANIE DANYCH
        if(strcmp(argument, symbol) == 0 && S_ISREG(permission)){
            printf("%s %jd ", fullPath, fileSize);

            if (permission & S_IRUSR) printf("r");
            else printf("-");
            if (permission & S_IWUSR) printf("w");
            else printf("-");
            if (permission & S_IXUSR) printf("x");
            else printf("-");

            if (permission & S_IRGRP) printf("r");
            else printf("-");
            if (permission & S_IWGRP) printf("w");
            else printf("-");
            if (permission & S_IXGRP) printf("x");
            else printf("-");

            if (permission & S_IROTH) printf("r");
            else printf("-");
            if (permission & S_IWOTH) printf("w");
            else printf("-");
            if (permission & S_IXOTH) printf("x ");
            else printf("- ");

            printf("%s", ctime(&lastMod));
        }

        dirent = readdir(dir);
    }

    //ZAMYKANIE KATALOGU
    closedir(dir);
}

int main(int argc, char ** argv) {
    //PRZYGOTOWANIE DATY
    if(argc < 9){
        printf("Prosze podac wszystkie potrzebne argumenty!!!\n");
        return 0;
    }
    int date[6];
    for(int i = 0; i < 6; i++){
        date[i] = atoi(argv[i + 3]);
    }

    //WLASCIWA FUNKCJA
    seekingThroughDirectory(argv[1], argv[2], date);

    return 0;
}
