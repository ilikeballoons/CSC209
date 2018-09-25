#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_SIZE 1024
#define MAX_UID_SIZE 31

void read_lines() {
  int greatest_pid_num = 1;
  char greatest_uid[MAX_UID_SIZE];

  char current_uid[MAX_UID_SIZE];
  int current_pid_num = 1;

  char line_uid[MAX_UID_SIZE];
  char input_line[MAX_LINE_SIZE];

  //read first line and set the appropriate variables
  fgets(input_line, MAX_LINE_SIZE, stdin);
  sscanf(input_line, "%s", greatest_uid);

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
  printf("%s %d\n", greatest_uid, greatest_pid_num);
}

int main(int argc, char **argv) {
  if(argc > 2) {
    printf("USAGE: most_processes [ppid]\n");
    return 1;
  }

  if(argc == 2) {
    char *ptr;
    int ppid = strtol(argv[1], &ptr, 10); //capture the ppid
    read_lines();
    return 0;
  }

}
