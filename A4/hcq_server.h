#ifndef HCQSERVER_H
#define HCQSERVER_H
#endif

#ifndef PORT
#define PORT 51302
#endif

/* State Constants */
#define WAITING_NAME_STATE 100
#define WAITING_TYPE_STATE 101
#define WAITING_S_COURSE_STATE 102
#define WAITING_S_COMMAND_STATE 103
#define WAITING_T_COMMAND_STATE 104
#define T_HELPING_STATE 105
#define DISCONNECT_STATE 199

/* Client Type Constants */
#define TYPE_NOT_SET 'N'
#define STUDENT 'S'
#define TA 'T'

/* String Constants */
#define WELCOME_MSG "Welcome to the Help Centre, what is your name?\r\n"
#define TYPE_MSG "Are you a TA or a Student (enter T or S)?\r\n"
#define VALID_COURSES_MSG "Valid courses: CSC108, CSC148, CSC209\r\nWhich course are you asking about?\r\n"
#define SYNTAX_ERR_MSG "Incorrect syntax\r\n"
#define TA_COMMANDS_MSG "Valid commands for TA:\r\n\tstats\r\n\tnext\r\n\t(or use Ctrl-C to leave)\r\n"
#define TA_INVALID_ROLE_MSG "Invalid role (enter T or S)\r\n"
#define STU_QUEUE_MSG "You have been entered into the queue. While you wait, you can use the command stats to see which TAs are currently serving students.\r\n"
#define STU_INVALID_COURSE_MSG "This is not a valid course. Good-bye.\r\n"
#define STU_ACCEPT_MSG "Your turn to see the TA.\r\nWe are disconnecting you from the server now. Press Ctrl-C to close nc\r\n"

/* Miscellaneous Constants */
#define INPUT_BUFFER_SIZE 30
#define EXIT_FAILURE 1
/* Clients are stored in  order of the time of their addition.
  Newest clients are kept at the tail of the list. */
struct client {
  int state, socket, inbuf, room;
  char *name, *after, type;
  char buf[INPUT_BUFFER_SIZE + 2];
  struct client *next;
};

typedef struct client Client;

struct sockaddr_in *init_server_addr(int port);
int set_up_server_socket(struct sockaddr_in *self, int num_queue);
int accept_connection(int listenfd);
int find_network_newline(const char *buf, int n);
Client *add_client (int fd);
Client *find_client(int fd);
Client *find_client_student(Student *student);
void remove_client(int fd);
void handle_client(Client *client);
