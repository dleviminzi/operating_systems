#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

void handler(int sig) {
    if (sig == SIGSEGV) {
        fprintf(stderr, "Segmentation fault occured\n");
        exit(4);
    }
}

void fdRedirect(int newFD, int targetFD) {
    close(targetFD);
    dup2(newFD, targetFD);
}

int main(int argc, char *argv[]) {
    /* flags for segmentation fault and catch */
    static int segF;
    static int catch;

    /* getopt parameters */
    int opt = 0;
    int option_index = 0;

    /* initializing temporary buffer, and fds for input/output copying */
    char buff;
    int ifd = 0;
    int ofd = 0;

    static struct option long_options[] = {
        {"input", required_argument, 0, 'i'},
        {"output", required_argument, 0, 'o'},
        {"segfault", no_argument, &segF, 1},
        {"catch", no_argument, &catch, 1},
        {0, 0, 0, 0}
    };

    while((opt = getopt_long(argc, argv, ":i:o:", long_options, &option_index)) != -1) {
        switch(opt) {
            case 'i':
                if ((ifd = open(optarg, O_RDONLY)) > 0) fdRedirect(ifd, 0);
                else {
                    fprintf(stderr, "Input file could not be opened.");
                    exit(2);
                }
                break;
            case 'o':
                if ((ofd = open(optarg, O_CREAT | O_RDWR | O_TRUNC, 0666)) > 0) fdRedirect(ofd, 1);
                else {
                    fprintf(stderr, "Output file could not be opened or created.");
                    exit(3);
                }
                break;
            case '?':
                fprintf(stderr, "Unknown option. Options are: input, output, segfault, catch.\n");
                exit(1);
            case ':':
                fprintf(stderr, "Argument must be provided for %c.\n", optopt);
                exit(1);
        }
    }

    if (catch) signal(SIGSEGV, handler);
    
    if (segF) {
        char *ptr = NULL;
        *ptr = 5;
    } else while (read(0, &buff, sizeof(char)) != 0) write(1, &buff, sizeof(char));

    close(1);

    return 0;
}