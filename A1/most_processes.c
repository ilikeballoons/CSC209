#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_SIZE 1024
#define MAX_UID_SIZE 31

void read_lines() {
  int greatest_pid_num = 1;
  char greatest_uid[MAX_UID_SIZE] = "NO UID";

  int current_pid_num = 1;
  char current_uid[MAX_UID_SIZE];

  char line_uid[MAX_UID_SIZE];
  char input_line[MAX_LINE_SIZE];

  while(fgets(input_line, MAX_LINE_SIZE, stdin) != NULL) {
    sscanf(input_line, "%s", line_uid);
    //read uid == current_uid
    if(strcmp(line_uid, current_uid) == 0) {
      current_pid_num++;
      if (greatest_pid_num < current_pid_num) {
        greatest_pid_num = current_pid_num;
        strcpy(greatest_uid, current_uid);
      }
    } else {
      //read uid != current_uid
      strcpy(current_uid, line_uid);
      current_pid_num = 1;
    }
  }
  if(strcmp(greatest_uid, "NO UID") != 0) {
    printf("%s %d\n", greatest_uid, greatest_pid_num);
  }
}

void read_lines_ppid(int user_ppid) {
  int greatest_pid_num = 1;
  char greatest_uid[MAX_UID_SIZE] = "PPID NOT FOUND";
  int current_pid_num = 1;
  char current_uid[MAX_UID_SIZE];
  int line_ppid = -1;
  char line_uid[MAX_UID_SIZE];

  char input_line[MAX_LINE_SIZE];

  while(fgets(input_line, MAX_LINE_SIZE, stdin) != NULL) {
    sscanf(input_line, "%s %*d %d", line_uid, &line_ppid);
    //ppid matches param
    if(line_ppid == user_ppid) {
      //read uid == current_uid
      if(strcmp(line_uid, current_uid) == 0) {
        current_pid_num++;
        if(greatest_pid_num < current_pid_num) {
          greatest_pid_num = current_pid_num;
          strcpy(greatest_uid, current_uid);
        }
      } else {
        //read uid != current uid
        strcpy(current_uid, line_uid);
        current_pid_num = 1;
      }
    }
  }
  if(strcmp(greatest_uid, "PPID NOT FOUND") != 0) {
    printf("%s %d\n", greatest_uid, greatest_pid_num);
  }
}

int main(int argc, char **argv) {
  if(argc > 2) {
    printf("USAGE: most_processes [ppid]\n");
    return 1;
  }

  if(argc == 2) { //command line options included
    char *ptr;
    int ppid = strtol(argv[1], &ptr, 10); //capture the ppid
    read_lines_ppid(ppid);
    return 0;
  }

  if(argc == 1) { //no command line options
    read_lines();
    return 0;
  }
}
