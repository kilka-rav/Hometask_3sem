#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

void check_arguments(int arg) {
    if ( arg != 2 ) {
        fprintf(stderr, "ERROR IN INPUT\n");
        exit(1);
    }
}

void check(int arg) {
    if ( arg < 0 ) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(2);
    }
}

void print_lock(struct flock* f, char* name) {
    printf("NAME %s\n", name);
    printf("TYPE %d\n", f->l_type);
    printf("WHENCE %d\n", f->l_whence);
    printf("START %ld\n", f->l_start);
    printf("LEN %ld\n", f->l_len);
    printf("PID %d\n", f->l_pid);
}

int main(int argc, char** argv) {
    check_arguments(argc);
    struct flock f = {0};
    int fd = open(argv[1], O_RDONLY);
    check(fd);
    int ret;
    f.l_type = F_RDLCK;
    f.l_whence = SEEK_CUR;
    while( ret = fcntl(fd, F_GETLK, &f) >= 0 ) {
        //int ret = fcntl(fd, F_GETLK, &f);
        check(ret);
        print_lock(&f, argv[1]);
    }
    close(fd);
    return 0;
}
