#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

int fd_end;


int check(char* s) {
    int len = strlen(s);
    int del_5, del_3;
    del_3 = 0;
    del_5 = 0;
    for(int i = 0; i < len; i++){
        if ( ( s[i] != '-' ) && ( s[i] < 48 || s[i] > 57 ) ) {
            return 0;
        }
        del_3 += ((s[i] - '0') % 3);
    }
    if ( s[len - 1] == '5' || s[len - 1] == '0' )
        del_5 = 1;
    if ( del_3 % 3 == 0 )
        del_3 = 1;
    else
        del_3 = 0;
    if ( del_5 == 1 && del_3 == 1 )
        return 3;
    else if ( del_5 == 1 && del_3 == 0 )
        return 2;
    else if ( del_3 == 1 && del_5 == 0 )
        return 1;
    else
        return 0;
}
void bizz_buzz(char* str) {
    if ( check(str) == 0 )
        write(fd_end, str, strlen(str));
    else if ( check(str) == 1 )
        write(fd_end, &"bizz", 4);
    else if ( check(str) == 2 )
        write(fd_end, &"buzz", 4);
    else if ( check(str) == 3 )
        write(fd_end, &"bizzbuzz", 8);
}

char* concat(char* s, char* buf) {
    size_t len = strlen(buf);
    char* res = malloc(len+2);
    memcpy(res, buf, len);
    memcpy(res + len, s, 2);
    return res;
}

int main(int argc, char** argv) {
    if ( argc != 3 ) {
        fprintf(stderr, "wrong number of FILES\n");
        return 1;
    }
    int fd_start = open(argv[1], O_RDWR);
    if ( fd_start < 0 ) {
        fprintf(stdout, "%s\n", strerror(errno));
        return 2;
    }
    fd_end = open(argv[2], O_CREAT | O_RDWR, 0666);
    if ( fd_end < 0 ) {
        fprintf(stdout, "%s\n", strerror(errno));
        return 3;
    }
    struct stat buffer;
    fstat(fd_start, &buffer);
    size_t sz = buffer.st_size;
    size_t count = 0;
    char* min_buf = malloc(1);
    char* buf = malloc(0);
    while(count < sz) {
        read(fd_start, min_buf , 1);
        char c = *min_buf;
        if ( (c != ' ') && (c != '\0') && (c != '\n') &&  (c != '\v') && (c != '\t') ) {
            buf = concat(min_buf, buf);
        }
        else {
            if ( strlen(buf) == 0 ) {
                write(fd_end, &c, 1);
            }
            else {
                bizz_buzz(buf);
                write(fd_end, &c, 1);
                free(buf);
                buf = malloc(0);
            }
        }
        count++;
    }
    free(min_buf);
    free(buf);
    return 0;
}

