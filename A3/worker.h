#ifndef WORKER_H
#define WORKER_H

#define PATHLENGTH 128
#define MAXRECORDS 100

#define MAXWORKERS 10

// This data structure is used by the workers to prepare the output
// to be sent to the master process.

typedef struct {
    int freq;
    char filename[PATHLENGTH];
} FreqRecord;

FreqRecord *get_word(char *word, Node *head, char **file_names);
void print_freq_records(FreqRecord *frp);
void run_worker(char *dirname, int in, int out);

FreqRecord *get_empty_freqrecord ();
FILE *Fopen(char *file_name, char *mode);
int Close(int fd);
int Read (int fd, void *buf, size_t nbytes);
int Write(int fd, const void *buf, size_t nbytes);
void *Malloc (size_t size);



#endif /* WORKER_H */
