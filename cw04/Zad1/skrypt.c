#include <stdio.h>
#include <time.h>

int main(int argc, char ** argv) {
	
	while(1){
		time_t czas;
		time(&czas);
		char * data = ctime(&czas);
		printf("%s", data);
	}

    	return 0;
}
