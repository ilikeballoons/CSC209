/* The purpose of this program is to practice writing signal handling
 * functions and observing the behaviour of signals.
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

/* Message to print in the signal handling function. */
#define MESSAGE "%ld reads were done in %ld seconds.\n"

/* Global variables to store number of read operations and seconds elapsed.
 */
long num_reads, seconds;


/* The first command-line argument is the number of seconds to set a timer to run.
 * The second argument is the name of a binary file containing 100 ints.
 * Assume both of these arguments are correct.
 */

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: time_reads s filename\n");
        exit(1);
    }
    seconds = strtol(argv[1], NULL, 10);

    FILE *fp;
    if ((fp = fopen(argv[2], "r")) == NULL) {
      perror("fopen");
      exit(1);
    }

    /* In an infinite loop, read an int from a random location in the file,
     * and print it to stderr.
     */
    for (;;) {
      int seek_loc = random() % 100;
      int num, err_flag;
      if ((err_flag = fseek(fp, seek_loc * sizeof(int), SEEK_SET)) != 0) {
        perror("fseek");
        exit(1);
      }
      if((err_flag = fread(&num, sizeof(int), 1, fp)) != 1) {
        perror("fread");
        exit(1);
      }
      fprintf(stderr, "%d\n", num);
      sleep(1);
    }
    return 1; // something is wrong if we ever get here!
}
