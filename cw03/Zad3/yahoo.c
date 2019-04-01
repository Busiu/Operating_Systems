#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char ** argv){
    
	int array_size = 100000000;
    char * hehe = (char*) calloc(array_size, sizeof(char));
	char * tmp = hehe;
	printf("Ale zem tablice duza zaalokowal :O\n");
	for(int i = 0; i < array_size; i++){
		*tmp = (char) i % 26 + 65;
		tmp++;
	}
	free(hehe);
	
    return 0;
}
