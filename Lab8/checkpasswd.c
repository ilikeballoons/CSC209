#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 256
#define MAX_PASSWORD 10
#define READ 0
#define WRITE 1

#define SUCCESS "Password verified\n"
#define INVALID "Invalid password\n"
#define NO_USER "No such user\n"

#define VALIDATE_EXEC "./validate"
#define VALIDATE "validate"

int main(void) {
  char user_id[MAXLINE];
  char password[MAXLINE];

  if(fgets(user_id, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }
  
  if(fgets(password, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }

  int pfd[2], err;
  if ((err = pipe(pfd)) == -1) {
    perror("pipe");
    exit(1);
  }

  if((err = write(pfd[WRITE], user_id, MAX_PASSWORD)) < 0) {
    perror("write");
    exit(1);
  }

  if((err = write(pfd[WRITE], password, MAX_PASSWORD)) < 0) {
    perror("write");
    exit(1);
  }

  int pid = fork();
  if (pid < 0) {
    perror("fork");
    exit(1);
  }

  if (pid > 0) { //parent
    close(pfd[READ]);
    write(pfd[WRITE], user_id, 32);
    write(pfd[WRITE], password, MAX_PASSWORD);
    close(pfd[WRITE]);
    int status;
    if(wait(&status) == -1) {
      perror("wait");
      exit(1);
    }
    if (WIFEXITED(status)) {
      int result = WEXITSTATUS(status);
      if(result == 0) {
        printf(SUCCESS);
      } else if(result == 2) {
        printf(INVALID);
      } else if(result == 3) {
        printf(NO_USER);
      }
    }
} else if (pid == 0) {
    close(pfd[WRITE]);
    dup2(pfd[READ], STDIN_FILENO);
    execl(VALIDATE_EXEC, VALIDATE, NULL);
    close(pfd[READ]);
  }
  return 0;
}
