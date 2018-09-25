#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    	int ppid = strtod(argv[1], NULL);

	if(argc > 2) {
		printf("USAGE: most_processes [ppid]\n");
		return 1;
	}

	if(argc == 2) {
		char *ptr;
		int ppid = strtol(argv[1], &ptr, 10); //capture the ppid
		printf("peepeeid %d", ppid);
		return 0;
	}

}
