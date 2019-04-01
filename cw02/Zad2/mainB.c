#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <dirent.h>
#include <malloc.h>
#include <memory.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include <ftw.h>

typedef int bool;
#define true 1
#define false 0

const int SecondsPerMinute = 60;
const int SecondsPerHour = 3600;
const int SecondsPerDay = 86400;
const int DaysOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

time_t dateArgument;
char * argument;

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

int displayInfo(char * fpath, struct stat * stat, int tflag, struct FTW * ftwbuf){
    //INICJALICACJA ZMIENNYCH
    time_t lastMod = stat->st_mtime;
    char * symbol;
    off_t fileSize = stat->st_size;
    mode_t permission = stat->st_mode;



    //SPRAWDZENIE WARUNKOW
    if(tflag != FTW_F){
        return 0;
    }

    if(dateArgument > lastMod){
        symbol = "mniejsze";
    }
    else if(dateArgument < lastMod){
        symbol = "wieksze";
    }
    else{
        symbol = "rowne";
    }

    if(strcmp(argument, symbol) == 0){
        printf("%s %jd ", fpath, fileSize);

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

    return 0;

}

int main(int argc, char ** argv) {
    //PRZYGOTOWANIE DATY
    if(argc < 9){
        printf("Prosze podac wszystkie potrzebne argumenty!!!\n");
        return 0;
    }

    //INICJALIZACJA DANYCH
    char * dir = argv[1];
    int flags = FTW_PHYS;
    dateArgument = convertTime(atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]) ,atoi(argv[8]));
    argument = argv[2];

    //WLASCIWA FUNKCJA
    nftw(dir, displayInfo, 5, flags);

    return 0;
}
