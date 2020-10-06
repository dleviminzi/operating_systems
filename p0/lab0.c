#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

/* handler function for caught segmentation fault */
void handler(int sig) {
    if (sig == SIGSEGV) {
        fprintf(stderr, "Segmentation fault: %s\n", strerror(errno));
        exit(4);
    }
}

/* redirects newFD to targetFD */
void fdRedirect(int newFD, int targetFD) {
    close(targetFD);
    dup2(newFD, targetFD);
}

int main(int argc, char *argv[]) {
    static int segF;        /* flag for segmentation fault          */
    static int catch;       /* flag for catching segmentation fault */
    extern int errno;       /* error number holder for strerror(3)  */
    int opt = 0;            /* option variable for getopt           */
    int option_index = 0;   /* index for getopt                     */
    char buff;              /* buffer for copying stdin to stdout   */
    int ifd = 0;            /* temp input file descriptor holder    */
    int ofd = 0;            /* temp output file descriptor holder   */

    static struct option long_options[] = {
        {"input", required_argument, NULL, 'i'},
        {"output", required_argument, NULL, 'o'},
        {"segfault", no_argument, &segF, 1},
        {"catch", no_argument, &catch, 1},
        {0, 0, 0, 0}
    };

    while((opt = getopt_long(argc, argv, ":i:o:", long_options, &option_index)) != -1) {
        switch(opt) {
            case 'i':
                if ((ifd = open(optarg, O_RDONLY)) > 0) fdRedirect(ifd, 0);
                else {
                    fprintf(stderr, "Error on input: %s \nProper use: --input=filename", strerror(errno));
                    exit(2);
                }
                break;
            case 'o':
                if ((ofd = open(optarg, O_CREAT | O_RDWR | O_TRUNC, 0666)) > 0) fdRedirect(ofd, 1);
                else {
                    fprintf(stderr, "Error on output: %s \nProper use: --output=filename", strerror(errno));
                    exit(3);
                }
                break;
            case '?':
                fprintf(stderr, "Unknown option. Permitted options: input, output, segfault, catch \n");
                exit(1);
            case ':':
                fprintf(stderr, "Invalid argument provided for option.");
                exit(1);
        }
    }

    /* checking for non-option arguments */
    if (argv[optind]) {
        fprintf(stderr, "Non-option arguments are not permitted: %s\n", argv[optind]);
        exit(1);
    }

    if (catch) signal(SIGSEGV, handler);
    
    if (segF) {
        char *ptr = NULL;
        *ptr = 5;
    } else while (read(0, &buff, sizeof(char)) != 0) write(1, &buff, sizeof(char));

    close(0);
    close(1);

    return 0;
}