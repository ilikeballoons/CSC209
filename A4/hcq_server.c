#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>     /* inet_ntoa */
#include <netdb.h>         /* gethostname */
#include <sys/socket.h>

#include "hcq.h"
#define INPUT_BUFFER_SIZE 256
#define OUT_BUF_SIZE 1024
#define FULL_QUEUE_LENGTH 14

#define WAITING_NAME_STATE 100
#define WAITING_TYPE_STATE 101
#define WAITING_S_COURSE_STATE 102
#define WAITING_S_COMMAND_STATE 103
#define WAITING_T_COMMAND_STATE 104
#define T_HELPING_STATE 105
#define S_HELPING_STATE 106
#define IS_STUDENT 1000
#define IS_TA 2000


#ifndef PORT
  #define PORT 51302
#endif

// Use global variables so we can have exactly one TA list and one student list
Ta *ta_list = NULL;
Student *stu_list = NULL;

Course *courses;
int num_courses = 3;

struct sockaddr_in *init_server_addr(int port);
int set_up_server_socket(struct sockaddr_in *self, int num_queue);
int accept_connection(int listenfd);
int find_network_newline(const char *buf, int n);
int process_args(int cmd_argc, char **cmd_argv);
void handle_client(int fd);
void parse_command(char *command, int state, int fd);

int main() {
  setbuf(stdout, NULL);

    if ((courses = malloc(sizeof(Course) * 3)) == NULL) {
        perror("malloc for course list\n");
        exit(1);
    }
    strcpy(courses[0].code, "CSC108");
    strcpy(courses[1].code, "CSC148");
    strcpy(courses[2].code, "CSC209");

   struct sockaddr_in *self = init_server_addr(PORT);
   int listen_fd = set_up_server_socket(self, 5);
   while (1) { // listen for connections
     int fd = accept_connection(listen_fd);
     if (fd < 0) {
       continue;
     }
//     int pid = fork();
//     if (pid < 0) {
///      perror("fork");
//       exit(1);
    // } else if (pid == 0) { // child
       handle_client(fd);
  //   }

   }
   return 0;
 }
 // NOTE: need to fix double printing
 void handle_client(int fd) {
   int student_or_ta;
   int state = WAITING_NAME_STATE;
   char buf[INPUT_BUFFER_SIZE] = {'\0'};
   int inbuf = 0;
   int room = sizeof(buf);
   char *after = buf;
   int nbytes;
   char *name;

   write(fd, "Welcome to the Help Centre, what is your name?\r\n",
 strlen("Welcome to the Help Centre, what is your name?\r\n"));

   while ((nbytes = read(fd, after, room)) > 0) { // listen for input
     inbuf += nbytes; // how many bytes were just added?
     int where;

     while ((where = find_network_newline(buf, inbuf)) > 0) { // listen for complete commands
       buf[where-2] = '\0';
       char *command = Malloc(strlen(buf));
       strcpy(command, buf);
       int msg_length;
       char *msg;

       switch (state) {
         case WAITING_NAME_STATE :
          asprintf(&name, "%s", command);
          msg_length = asprintf(&msg, "Hello, %s\r\n", name);
          write(fd, msg, msg_length);
          msg_length = asprintf(&msg, "Are you a TA or a Student (enter T or S)?\r\n");
          write(fd, msg, msg_length);
          free(msg);
          state = WAITING_TYPE_STATE;
          break;

         case WAITING_TYPE_STATE :
          if (strcmp(command, "S") == 0) {
            student_or_ta = IS_STUDENT;
            msg_length = asprintf(&msg, "Valid courses: %s, %s, %s\r\nWhich course are you asking about?\r\n", courses[0].code, courses[1].code, courses[2].code);
            write(fd, msg, msg_length);
            free(msg);
            state = WAITING_S_COURSE_STATE;
          } else if (strcmp(command, "T") == 0) {
            student_or_ta = IS_TA;
            add_ta(&ta_list, name);
            msg_length = asprintf(&msg, "Valid commands for TA:\r\n\tstats\r\n\tnext\r\n\t(or use Ctrl-C to leave)\r\n");
            write(fd, msg, msg_length);
            free(msg);
            state = WAITING_T_COMMAND_STATE;
          } else {
            msg_length = asprintf(&msg, "Are you a TA or a Student (enter T or S)?\r\n");
            write(fd, msg, msg_length);
            free(msg);
          }
          break;

        case WAITING_S_COURSE_STATE :

          if(strcmp(command, courses[0].code) == 0
            || strcmp(command, courses[1].code) == 0
            || strcmp(command, courses[2].code) == 0) {
              add_student(&stu_list, name, command, courses, num_courses);
              state = WAITING_S_COMMAND_STATE;
              msg_length = asprintf(&msg, "You have been entered into the queue. While you wait, you can use the command stats to see which TAs are currently serving students.\r\n");
              write(fd, msg, msg_length);
              free(msg);
          } else {
            msg_length = asprintf(&msg, "Valid courses: %s, %s, %s\r\nWhich course are you asking about?\r\n", courses[0].code, courses[1].code, courses[2].code);
            write(fd, msg, msg_length);
            free(msg);
          }
          break;

        case WAITING_S_COMMAND_STATE :
          if (strcmp(command, "stats") == 0) {
            msg_length = asprintf(&msg, "%s", print_full_queue(stu_list));
            write(fd, msg, msg_length);
            free(msg);
          } else { // TODO: maybe remove this based on desired behavior
            msg_length = asprintf(&msg, "You have been entered into the queue. While you wait, you can use the command stats to see which TAs are currently serving students.\r\n");
            write(fd, msg, msg_length);
            free(msg);
          }
          break;

        case WAITING_T_COMMAND_STATE :
          if (strcmp(command, "stats") == 0) {
            msg_length = asprintf(&msg, "%s", print_currently_serving(ta_list));
            write(fd, msg, msg_length);
            free(msg);
          } else if (strcmp(command, "next") == 0) {
            next_overall(name, &ta_list, &stu_list);
          } else {
            msg_length = asprintf(&msg, "Valid commands for TA:\r\n\tstats\r\n\tnext\r\n\t(or use Ctrl-C to leave)\r\n");
            write(fd, msg, msg_length);
            free(msg);
          }
          break;
       }
       free(command);
       inbuf = inbuf - where;
       memset(buf, '\0', where - 1);
       memmove(buf, buf + where, inbuf); // move the stuff after the full ine to the beginning of the buffer
     }
     after = &(buf[inbuf]);
     room = INPUT_BUFFER_SIZE - inbuf;
   }
   switch (student_or_ta) {
     case IS_STUDENT :
      give_up_waiting(&stu_list, name);
      break;
     case IS_TA :
      remove_ta(&ta_list, name);
      break;
   }
   close(fd);
 }

/*
 * Initialize a server address associated with the given port.
 */
struct sockaddr_in *init_server_addr(int port) {
    struct sockaddr_in *addr = malloc(sizeof(struct sockaddr_in));

    // Allow sockets across machines.
    addr->sin_family = PF_INET;

    // The port the process will listen on.
    addr->sin_port = htons(port);

    // Clear this field; sin_zero is used for padding for the struct.
    memset(&(addr->sin_zero), 0, 8);

    // Listen on all network interfaces.
    addr->sin_addr.s_addr = INADDR_ANY;

    return addr;
}

/*
 * Create and set up a socket for a server to listen on.
 */
int set_up_server_socket(struct sockaddr_in *self, int num_queue) {
    int soc = socket(PF_INET, SOCK_STREAM, 0);
    if (soc < 0) {
        perror("socket");
        exit(1);
    }

    // Make sure we can reuse the port immediately after the
    // server terminates. Avoids the "address in use" error
    int on = 1;
    int status = setsockopt(soc, SOL_SOCKET, SO_REUSEADDR,
        (const char *) &on, sizeof(on));
    if (status < 0) {
        perror("setsockopt");
        exit(1);
    }

    // Associate the process with the address and a port
    if (bind(soc, (struct sockaddr *)self, sizeof(*self)) < 0) {
        // bind failed; could be because port is in use.
        perror("bind");
        exit(1);
    }

    // Set up a queue in the kernel to hold pending connections.
    if (listen(soc, num_queue) < 0) {
        // listen failed
        perror("listen");
        exit(1);
    }

    return soc;
}


/*
 * Wait for and accept a new connection.
 * Return -1 if the accept call failed.
 */
int accept_connection(int listenfd) {
    struct sockaddr_in peer;
    unsigned int peer_len = sizeof(peer);
    peer.sin_family = PF_INET;

    int client_socket = accept(listenfd, (struct sockaddr *)&peer, &peer_len);
    if (client_socket < 0) {
        perror("accept");
        return -1;
    } else {
        return client_socket;
    }
}

/*
 * Search the first n characters of buf for a network newline (\r\n).
 * Return one plus the index of the '\n' of the first network newline,
 * or -1 if no network newline is found.
 * Definitely do not use strchr or other string functions to search here. (Why not?)
 */
int find_network_newline(const char *buf, int n) {
  if (n > INPUT_BUFFER_SIZE) { return - 1; } // invalid search index
  for (int i = 0; i < n; i++) {
    if (buf[i] == '\r' && buf[i+1] == '\n') {
      return (i+1)+1;
    }
  }
    return -1;
}
