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

int main() {
    if ((courses = malloc(sizeof(Course) * 3)) == NULL) {
        perror("malloc for course list\n");
        exit(1);
    }
    strcpy(courses[0].code, "CSC108");
    strcpy(courses[1].code, "CSC148");
    strcpy(courses[2].code, "CSC209");

   struct sockaddr_in *self = init_server_addr(PORT);
   int listen_fd = set_up_server_socket(self, 5);

   while (1) {
     int fd = accept_connection(listen_fd);
     if (fd < 0) {
       continue;
     }
     // for holding arguments to individual commands passed to sub-procedure
     // printf("Welcome to the Help Centre Queuing System\nPlease type a command:\n>");

     char buf[INPUT_BUFFER_SIZE] = {'\0'};
     int inbuf = 0;
     int room = sizeof(buf);
     char *after = buf;
     int nbytes;

     while ((nbytes = read(fd, after, room)) > 0) {
       printf("%s", buf);
     }

   }
   return 0;
 }

//     while (fgets(input, INPUT_BUFFER_SIZE, input_stream) != NULL) {
//         // only echo the line in batch mode since in interactive mode the user
//         // has just typed the line
//         if (batch_mode) {
//             printf("%s", input);
//         }
//         // tokenize arguments
//         // Notice that this tokenizing is not sophisticated enough to
//         // handle quoted arguments with spaces so names can not have spaces.
//         char *next_token = strtok(input, DELIM);
//         cmd_argc = 0;
//         while (next_token != NULL) {
//             if (cmd_argc >= INPUT_ARG_MAX_NUM) {
//                 error("Too many arguments.");
//                 cmd_argc = 0;
//                 break;
//             }
//             cmd_argv[cmd_argc] = next_token;
//             cmd_argc++;
//             next_token = strtok(NULL, DELIM);
//         }
//         if (cmd_argc > 0 && process_args(cmd_argc, cmd_argv) == -1) {
//             break; // can only reach if quit command was entered
//         }
//         printf(">");
//     }
//
//     if (batch_mode) {
//         fclose(input_stream);
//     }
//
//     return 0;
// }
//
// /*
//  * Read and process commands
//  * Return:  -1 for quit command
//  *          0 otherwise
//  */
// int process_args(int cmd_argc, char **cmd_argv) {
//
//     int result;
//
//     if (cmd_argc <= 0) {
//         return 0;
//     } else if (strcmp(cmd_argv[0], "add_student") == 0 && cmd_argc == 3) {
//         result = add_student(&stu_list, cmd_argv[1], cmd_argv[2], courses,
//                         num_courses);
//         if (result == 1) {
//             error("This student is already in the queue.");
//         } else if (result == 2) {
//             error("Invalid Course -- student not added.");
//         }
//     }  else if (strcmp(cmd_argv[0], "print_full_queue") == 0 && cmd_argc == 1) {
//         printf("%s", print_full_queue(stu_list));
//
//     } else if (strcmp(cmd_argv[0], "print_currently_serving") == 0 && cmd_argc == 1) {
//         printf("%s", print_currently_serving(ta_list));
//
//     } else if (strcmp(cmd_argv[0], "give_up") == 0 && cmd_argc == 2) {
//         if (give_up_waiting(&stu_list, cmd_argv[1]) == 1) {
//             error("There was no student by that name waiting in the queue.");
//         }
//     } else if (strcmp(cmd_argv[0], "add_ta") == 0 && cmd_argc == 2) {
//         add_ta(&ta_list, cmd_argv[1]);
//
//     } else if (strcmp(cmd_argv[0], "remove_ta") == 0 && cmd_argc == 2) {
//         if (remove_ta(&ta_list, cmd_argv[1]) == 1) {
//            error("Invalid TA name.");
//         }
//     } else if (strcmp(cmd_argv[0], "next") == 0 && cmd_argc == 2) {
//         if (next_overall(cmd_argv[1], &ta_list, &stu_list) == 1) {;
//            error("Invalid TA name.");
//         }
//     } else {
//         error("Incorrect syntax.");
//     }
//     return 0;
// }

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

    fprintf(stderr, "Waiting for a new connection...\n");
    int client_socket = accept(listenfd, (struct sockaddr *)&peer, &peer_len);
    if (client_socket < 0) {
        perror("accept");
        return -1;
    } else {
        fprintf(stderr,
            "New connection accepted from %s:%d\n",
            inet_ntoa(peer.sin_addr),
            ntohs(peer.sin_port));
        return client_socket;
    }
}
