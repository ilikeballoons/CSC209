#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include "freq_list.h"
#include "worker.h"

int main(int argc, char **argv) {
    Node *head = NULL;
    char **filenames = init_filenames();
    char arg;
    char *query = "";
    char *listfile = "index";
    char *namefile = "filenames";

    /* an example of using getop to process command-line flags and arguments */
    while ((arg = getopt(argc,argv,"i:n:q:")) > 0) {
        switch(arg) {
        case 'i':
            listfile = optarg;
            break;
        case 'n':
            namefile = optarg;
            break;
        case 'q':
            query = optarg;
            break;
        default:
            fprintf(stderr, "Usage: test_worker [-i FILE] [-n FILE] [-q QUERY]\n");
            exit(1);
        }
    }
    printf("Query: %s\n", query);
    read_list(listfile, namefile, &head, filenames);
    FreqRecord *frp = get_word(query, head, filenames);
    print_freq_records(frp);
    //display_list(head, filenames);

    return 0;
}
