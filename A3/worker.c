#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include "freq_list.h"
#include "worker.h"

void *Malloc (int size) {
  void *ret = malloc(size);
  if (ret == NULL) {
    perror("malloc failed");
    exit(1);
  }
  return ret;
}


/* Returns an array of FreqRecord structs for the word word.
*/
FreqRecord *get_word(char *word, Node *head, char **file_names) {
  // figure out how many file names there are
  int number_of_files;
  for(number_of_files = 0; file_names[number_of_files] != NULL; number_of_files++) {}
  // declare an array of structs the same length as the number of file file_names
  FreqRecord *frequencies = Malloc(number_of_files * sizeof(FreqRecord));
  // iterate through the linked list
  Node *cur = head;

  while(cur) {
    int i = 0;
    if (strcmp(cur->word, word) == 0) {
      for (; i < number_of_files; i++) {
        // if you find the word, copy the frequency at that position to the FreqRecord
        frequencies[i].freq = cur->freq[i];
        strncpy(frequencies[i].filename, file_names[i], PATHLENGTH);
      }
    }
    if(cur->next) { //advance to the next item in the linkedlist
      cur = cur->next;
    } else { // no more items, add the empty frequency to show no more data
      frequencies[i].freq = 0;
      strcpy(frequencies[i].filename, "\0");
      cur = NULL;
    }
  }
  return frequencies;
}

/* Print to standard output the frequency records for a word.
* Use this for your own testing and also for query.c
*/
void print_freq_records(FreqRecord *frp) {
    int i = 0;

    while (strlen(frp[i].filename) > 0) {
        printf("%d    %s\n", frp[i].freq, frp[i].filename);
        i++;
    }
}

/* Complete this function for Task 2 including writing a better comment.
*/
void run_worker(char *dirname, int in, int out) {
    return;
}
