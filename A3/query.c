#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>


#include "freq_list.h"
#include "worker.h"
/* fork with error checking */
int Fork () {
  int ret = fork();
  if (ret < 0) {
    perror("fork");
    exit(1);
  }
  return ret;
}

/* TODO: replace me with a proper docstring
*/
int main(int argc, char **argv) {
  char ch;
  char path[PATHLENGTH];
  char *startdir = ".";

  /* this models using getopt to process command-line flags and arguments */
  while ((ch = getopt(argc, argv, "d:")) != -1) {
    switch (ch) {
      case 'd':
      startdir = optarg;
      break;
      default:
      fprintf(stderr, "Usage: query [-d TARGET_DIRECTORY_NAME]\n");
      exit(1);
    }
  }

  // Open the directory provided by the user (or current working directory)
  DIR *dirp;
  if ((dirp = opendir(startdir)) == NULL) {
    perror("opendir");
    exit(1);
  }

  /* For each entry in the directory, eliminate . and .., and check
  * to make sure that the entry is a directory, then call run_worker
  * to process the index file contained in the directory.
  * Note that this implementation of the query engine iterates
  * sequentially through the directories, and will expect to read
  * a word from standard input for each index it checks.
  */
  struct dirent *dp;
  int num_children = 0;
  char *paths[MAXWORKERS];

  while ((dp = readdir(dirp)) != NULL) {
    if (strcmp(dp->d_name, ".") == 0 ||
    strcmp(dp->d_name, "..") == 0 ||
    strcmp(dp->d_name, ".svn") == 0 ||
    strcmp(dp->d_name, ".git") == 0) {
      continue;
    }

    strncpy(path, startdir, PATHLENGTH);
    strncat(path, "/", PATHLENGTH - strlen(path));
    strncat(path, dp->d_name, PATHLENGTH - strlen(path));
    path[PATHLENGTH - 1] = '\0';

    struct stat sbuf;
    if (stat(path, &sbuf) == -1) {
      // This should only fail if we got the path wrong
      // or we don't have permissions on this entry.
      perror("stat");
      exit(1);
    }

    if (S_ISDIR(sbuf.st_mode) && num_children <= MAXWORKERS) { // this block will execute once per subdirectory
      paths[num_children] = Malloc(PATHLENGTH);
      strcpy(paths[num_children], "./");
      strncat(paths[num_children], path, PATHLENGTH - strlen(path) - 2);

      num_children++;
    }
  }

  int words_fd[2 * num_children], freqs_fd[2 *num_children];
  for (int i = 0; i < num_children; i++) {
    pipe(&words_fd[2*i]);
    pipe(&freqs_fd[2*i]);
  }

  int pid;
  for (int i = 0; i < num_children; i++) { // make a child for each subdir
    pid = Fork();
    if (pid == 0){ // CHILD
      printf("%d\n", i);
      Close(words_fd[2*i+1]);
      Close(freqs_fd[2*i]);
      run_worker(paths[i], words_fd[2*i], freqs_fd[2*i+1]);
      Close(words_fd[2*i]);
      Close(freqs_fd[2*i+1]);
      exit(0);
    }
  }

  Close(words_fd[0]);
  Close(freqs_fd[1]);
  char query_word[MAXWORD];
  FreqRecord *master = Malloc(MAXRECORDS * sizeof(FreqRecord));
  FreqRecord *child_freqr = Malloc(sizeof(FreqRecord));
  while(fgets(query_word, MAXWORD, stdin) != 0) {
    for (int i = 0; i < MAXRECORDS; i++) {
      master[i] = *get_empty_freqrecord();
    }
    child_freqr = get_empty_freqrecord();
    int count = 0;
    size_t len = strlen(query_word);
    query_word[len] = '\0'; // ensure null termination

    // write that word to output pipe
    for (int i = 0; i < num_children; i++) {
      if((Write(words_fd[2*i+1], query_word, len)) != len) {
        fprintf(stderr, "Failed to write %s to words pipe\n", query_word);
        exit(1);
      }
      do {
        Read(freqs_fd[2*i], child_freqr, sizeof(FreqRecord));
        if(child_freqr->freq > 0) {
          insert(master, child_freqr, &count);
        }
      } while (child_freqr->freq > 0);
    }
    print_freq_records(master);
  }
  for (int i = 0; i < num_children; i++) {
    Close(words_fd[2*i+1]);
    Close(freqs_fd[2*i]);
  }
  // int status;
  // while ((pid = waitpid(-1, &status, 0)) != -1) {
  //   printf("Process %d terminated\n", pid);
  // }

  if (closedir(dirp) < 0){
    perror("closedir");
  }

  return 0;
}
