#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int Fork () {
  int ret = fork();
  if (ret < 0) {
    perror("fork");
    exit(1);
  }
  return ret;
}

int main(int argc, char **argv) {
    int i;
    int iterations;

    if (argc != 2) {
        fprintf(stderr, "Usage: forkloop <iterations>\n");
        exit(1);
    }

    iterations = strtol(argv[1], NULL, 10);
    int n;
    for (i = 0; i < iterations; i++) {
      n = Fork();
      printf("ppid = %d, pid = %d, i = %d\n", getppid(), getpid(), i);
      if (n != 0) {
        wait(NULL);
        exit(0);
      }
    }

    return 0;
}
