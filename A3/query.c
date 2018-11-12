#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

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
    int i = 1;
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

        // Only call run_worker if it is a directory
        // Otherwise ignore it.
        //make the master pipe
        // READ = 0
        // WRITE = 1
        int words_fd[2], freqs_fd[2];
        pipe(words_fd);
        pipe(freqs_fd);

        if (S_ISDIR(sbuf.st_mode)) {
          // fork
          int pid = Fork();
          if (pid < 0) {
            // parent
            char query_word[MAXWORD];
            Close(words_fd[0]);
            Close(freqs_fd[1]);
            while(fgets(query_word, MAXWORD, stdin) != 0) {
              // read a word from STDIN_FILENO
              size_t len = strlen(query_word);
              // write that word to output pipe
              query_word[len] = '\0'; // ensure null termination
              if((Write(words_fd[1], query_word, len)) != len) {
                fprintf(stderr, "Failed to write %s to words pipe\n", query_word);
                exit(1);
              }
              // read all freqrecords that get written from the input pipe to STDOUT_FILENO
              // do something with the freqrecords
              // close word write end
              Close(words_fd[1]);
              // if all children exit, close the freqrecords

            }
            close(words_fd[1]);
            // when all child write ends are closed, close master read end

          } else if (pid == 0) {
            // child
            Close(words_fd[1]);
            Close(freqs_fd[0]);
            char dir_path[PATHLENGTH];
            strcpy(dir_path, "./");
            strncat(dir_path, path, PATHLENGTH - strlen(path) - 2);
            printf("path %d:\t%s\n", i, dir_path);
            // reads words from input pipe
            // calls run_worker(childs_path, input_pipe, output_pipe);
            run_worker(dir_path, words_fd[0], freqs_fd[1]);
            // when master process write end is closed, closed write and read ends
            Close(words_fd[0]);
            Close(freqs_fd[1]);
            exit(0);
          }
          i++;
          // printf("directory %d:\t%s\n", i, dir_path);

        }
    }

    if (closedir(dirp) < 0)
        perror("closedir");

    return 0;
}
