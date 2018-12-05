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
#include "hcq_server.h"

/* Use global variables so we can have exactly one TA list, one student list,
 and one client list */
Ta *ta_list = NULL;
Student *stu_list = NULL;
Client *client_list = NULL;
fd_set active_fd_set, read_fd_set;

Course *courses;
int num_courses = 3;

int main() {
  setbuf(stdout, NULL); //TODO: delete this

  courses = Malloc(sizeof(Course) * 3);
  strcpy(courses[0].code, "CSC108");
  strcpy(courses[1].code, "CSC148");
  strcpy(courses[2].code, "CSC209");

  struct sockaddr_in *self = init_server_addr(PORT);
  int listen_fd = set_up_server_socket(self, 5);
  // Initialize the set of active sockets.
  FD_ZERO(&active_fd_set);
  FD_SET(listen_fd, &active_fd_set);

  while (1) { // listen for connections
    read_fd_set = active_fd_set;
    if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
      perror("select");
      exit(EXIT_FAILURE);
    }

    for (int fd = 0; fd < FD_SETSIZE; ++fd) {
      if (FD_ISSET(fd, &read_fd_set)) {
        if (fd == listen_fd) {
          // new connection request
          int new_fd = accept_connection(fd);
          if (fd < 0) {
            continue;
          }
          add_client(new_fd);
          FD_SET(new_fd, &active_fd_set);
        } else {
          Client *client = find_client(fd);
          if(client->state == DISCONNECT_STATE) {
            remove_client(fd);
          } else {
            handle_client(client);
          }
        }
      }
    }
  }
  return 0;
}

Client *add_client (int fd) {
  Client *new_client = Malloc(sizeof(Client));
  new_client->socket = fd;
  new_client->state = WAITING_NAME_STATE;
  new_client->type = -1;
  new_client->next = NULL;
  for (int i = 0; i < INPUT_BUFFER_SIZE; i++) {
    new_client->buf[i] = '\0';
  }
  new_client->after = new_client->buf;
  new_client->inbuf = 0;
  new_client->room = sizeof(new_client->buf);

  if (client_list == NULL) {
    client_list = new_client;
  } else {
    Client *last = client_list;
    while (last->next != NULL) {
      last = last->next;
    }
    last->next = new_client;
  }
  write(new_client->socket, "Welcome to the Help Centre, what is your name?\r\n",
  strlen("Welcome to the Help Centre, what is your name?\r\n"));

  return new_client;
}

int remove_client (int fd) {
  Client *client;
  if ((client = find_client(fd)) == NULL) { // client not found
    return -1;
  }
  if (client->type != -1) { // if type has been set
    switch (client->type) { // remove on disconnect
      case STUDENT :
        give_up_waiting(&stu_list, client->name);
        break;
      case TA :
        remove_ta(&ta_list, client->name);
        break;
    }
  }
  if (client->socket == client_list->socket) {
    // first in the linked list
    client_list = client_list->next;
  } else {
    Client *last = client_list;
    while (last->next->socket != client->socket) {
      // middle/end of queue
      last = last->next;
    }
    last = client->next;
  }
  free(client->name);
  free(client);
  close(fd);
  FD_CLR(fd, &active_fd_set);
  return 0;
}

Client *find_client (int fd) {
  if (client_list == NULL) {
    return NULL;
  }
  Client *last = client_list;
  while (last != NULL) {
    if (last->socket == fd) {
      return last;
    }
    last = last->next;
  }
  return NULL;
}

Client *find_client_student(Student *student) {
  if (client_list == NULL) {
    return NULL;
  }
  Client *last = client_list;
  while (last != NULL) {
    if (strcmp(last->name, student->name) == 0) {
      return last;
    }
    last = last->next;
  }
  return NULL;
}

void handle_client(Client *client) {
  int nbytes;
  if (client->inbuf < INPUT_BUFFER_SIZE) {
    nbytes = read(client->socket, client->after, client->room); // listen for input
  } else {
    remove_client(client->socket);
    return;
  }
  client->inbuf += nbytes; // how many bytes were just added?

  int where = find_network_newline(client->buf, client->inbuf);// listen for complete commands
  if (where != -1) {
    client->buf[where-2] = '\0';
    char *command = Malloc(strlen(client->buf));
    strcpy(command, client->buf);
    int msg_length;
    char *msg;

    switch (client->state) {
      case WAITING_NAME_STATE :
      asprintf(&client->name, "%s", command);
      msg_length = asprintf(&msg, "Are you a TA or a Student (enter T or S)?\r\n");
      write(client->socket, msg, msg_length);
      free(msg);
      client->state = WAITING_TYPE_STATE;
      break;

      case WAITING_TYPE_STATE :
      if (strcmp(command, "S") == 0) {
        client->type = STUDENT;
        msg_length = asprintf(&msg, "Valid courses: %s, %s, %s\r\nWhich course are you asking about?\r\n", courses[0].code, courses[1].code, courses[2].code);
        write(client->socket, msg, msg_length);
        free(msg);
        client->state = WAITING_S_COURSE_STATE;
      } else if (strcmp(command, "T") == 0) {
        client->type = TA;
        add_ta(&ta_list, client->name);
        msg_length = asprintf(&msg, "Valid commands for TA:\r\n\tstats\r\n\tnext\r\n\t(or use Ctrl-C to leave)\r\n");
        write(client->socket, msg, msg_length);
        free(msg);
        client->state = WAITING_T_COMMAND_STATE;
      } else {
        msg_length = asprintf(&msg, "Invalid role (enter T or S)\r\n");
        write(client->socket, msg, msg_length);
        free(msg);
      }
      break;

      case WAITING_S_COURSE_STATE :
      if(strcmp(command, courses[0].code) == 0
      || strcmp(command, courses[1].code) == 0
      || strcmp(command, courses[2].code) == 0) {
        add_student(&stu_list, client->name, command, courses, num_courses);
        client->state = WAITING_S_COMMAND_STATE;
        msg_length = asprintf(&msg, "You have been entered into the queue. While you wait, you can use the command stats to see which TAs are currently serving students.\r\n");
        write(client->socket, msg, msg_length);
        free(msg);
      } else {
        msg_length = asprintf(&msg, "This is not a valid course. Good-bye.\r\n");
        write(client->socket, msg, msg_length);
        free(msg);
        client->state = DISCONNECT_STATE; // TODO: doesn't end NC
      }
      break;

      case WAITING_S_COMMAND_STATE :
      if (strcmp(command, "stats") == 0) {
        msg_length = asprintf(&msg, "%s", print_full_queue(stu_list));
        write(client->socket, msg, msg_length);
        free(msg);
      } else {
        msg_length = asprintf(&msg, "Incorrect syntax\r\n");
        write(client->socket, msg, msg_length);
        free(msg);
      }
      break;

      case WAITING_T_COMMAND_STATE :
      if (strcmp(command, "stats") == 0) {
        msg_length = asprintf(&msg, "%s", print_currently_serving(ta_list));
        write(client->socket, msg, msg_length);
        free(msg);
      } else if (strcmp(command, "next") == 0) {
        if(stu_list != NULL) {
          Client *to_serve = find_client_student(stu_list);
          msg_length = asprintf(&msg, "Your turn to see the TA.\r\nWe are disconnecting you from the server now. Press Ctrl-C to close nc\r\n");
          write(to_serve->socket, msg, msg_length);
          free(msg);
          to_serve->state = DISCONNECT_STATE;
        }
        next_overall(client->name, &ta_list, &stu_list);
      } else {
        msg_length = asprintf(&msg, "Incorrect syntax\r\n");
        write(client->socket, msg, msg_length);
        free(msg);
      }
      break;
    }
    free(command);
    client->inbuf = client->inbuf - where;
    memset(client->buf, '\0', where - 1);
    memmove(client->buf, client->buf + where, client->inbuf); // move the stuff after the full ine to the beginning of the buffer

  }
  client->after = &(client->buf[client->inbuf]);
  client->room = INPUT_BUFFER_SIZE - client->inbuf;
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
    exit(EXIT_FAILURE);
  }

  // Make sure we can reuse the port immediately after the
  // server terminates. Avoids the "address in use" error
  int on = 1;
  int status = setsockopt(soc, SOL_SOCKET, SO_REUSEADDR,
    (const char *) &on, sizeof(on));
    if (status < 0) {
      perror("setsockopt");
      exit(EXIT_FAILURE);
    }

    // Associate the process with the address and a port
    if (bind(soc, (struct sockaddr *)self, sizeof(*self)) < 0) {
      // bind failed; could be because port is in use.
      perror("bind");
      exit(EXIT_FAILURE);
    }

    // Set up a queue in the kernel to hold pending connections.
    if (listen(soc, num_queue) < 0) {
      // listen failed
      perror("listen");
      exit(EXIT_FAILURE);
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
