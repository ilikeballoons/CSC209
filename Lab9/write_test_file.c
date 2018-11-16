#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/* Write random integers (in binary) to a file with the name given by the command-line
 * argument.  This program creates a data file for use by the time_reads program.
 */

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: write_test_file filename\n");
        exit(1);
    }

    FILE *fp;
    if ((fp = fopen(argv[1], "w")) == NULL) {
        perror("fopen");
        exit(1);
    }

    int one_hundred_ints[100];
    for (int i = 0; i < 100; i++) {
      one_hundred_ints[i] = random() % 100;
    }

    size_t r1 = fwrite(one_hundred_ints, sizeof(int), 100, fp);
    if (r1 < 100) {
      perror("fwrite");
      exit(1);
    }

    int result;
    if ((result = fclose(fp)) != 0) {
      perror("fclose");
      exit(1);
    }
    return 0;
}
