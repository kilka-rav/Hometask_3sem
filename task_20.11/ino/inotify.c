#include <stdio.h>
#include <sys/inotify.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#define MAX 4096

void check(int err) {
    if ( err < 0 ) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }
}

void check_number(int num) {
    if ( num != 2 ) {
        fprintf(stderr, "ERROR IN INPUT\n");
        exit(2);
    }
}

int main(int argc, char** argv) {
    check_number(argc);
    int mask = IN_CREATE | IN_DELETE | IN_MOVE_SELF;
    char buf[MAX];
    struct inotify_event* event;
    int in = inotify_init();
    check(in);
    int watch = inotify_add_watch(in, argv[1], mask);
    check(watch);
    printf("ENTER IN INFINITY LOOP:\n");
    while(1) {
        int book = read(in, buf, MAX);
        check(book);
        event = (struct inotify_event*) buf;
        if ( event->mask & IN_CREATE ) {
            printf("event: create\n");
        }
        else if ( event->mask & IN_DELETE ) {
            printf("event: delete\n");
        }
        else if ( event->mask & IN_MOVE_SELF ) {
            printf("event: move\n");
        }
        else {
            printf("UNKNOWN\n");
        }
    }
    close(in);
    return 0;
}


