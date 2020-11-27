#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#define _GNU_SOURCE
#include <signal.h>
#include <poll.h>

#define BUFLEN 256

typedef struct pollfd Pollfd;

void check_input(int arg) {
    if ( arg < 2 ) {
        fprintf(stderr, "ERROR IN INPUT FILES\n");
        exit(1);
    }
}

void check(int fd) {
    if ( fd < 0 ) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(2);
    }
}

void print(Pollfd polfd, char* file) {
    char* buf = (char*) malloc(BUFLEN);
    //if ( polfd.revents & POLLIN ) {
    int book = read(polfd.fd, buf, BUFLEN);
    check(book);
    fprintf(stdout, "IN FIFO-file %s\n%s", file, buf);
    free(buf);
} 


int main(int argc, char** argv){
    check_input(argc);
    int count = 0;
    int* fds = (int*) malloc(argc - 1);
    Pollfd* polfds = (Pollfd*) malloc((argc - 1) * sizeof(Pollfd));
    while(count < argc - 1) {
        //unlink(argv[count]);
        fds[count] = open(argv[count + 1], O_NONBLOCK);
        check(fds[count]);
        //init(polfds[count], fds[count]);
        polfds[count].events = POLLIN;
        polfds[count].fd = fds[count];
        polfds[count].revents = 0;
        count++;
    }
    /*
    Pollfd* polfds = (Pollfd*) malloc(count * sizeof(Pollfd));
    for(int i = 0; i < argc; ++i) {
        init(polfds[i], fds[i]);
    }
    */
    int rep;
    while( rep = poll(polfds, count, -1) != -1) {
        for(int i = 0; i < count; ++i) {
        if (polfds[i].revents & POLLIN) {
            print(polfds[i], argv[i + 1]);
        }
        }
    }
    free(fds);
    return 0;
}

