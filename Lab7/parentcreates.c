#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    int i;
    int iterations;

    if (argc != 2) {
        fprintf(stderr, "Usage: forkloop <iterations>\n");
        exit(1);
    }

    iterations = strtol(argv[1], NULL, 10);
    for (i = 0; i < iterations; i++) {
      int n = fork();
      if (n < 0) {
        perror("fork");
        exit(1);
      }
      printf("ppid = %d, pid = %d, i = %d\n", getppid(), getpid(), i);
      if (n == 0) {
        exit(0);
      } else {
        wait(NULL);
      }
    }

    return 0;
}
