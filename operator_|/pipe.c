#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

int count_pipe(char* s) {
    int count = 0;
    int len = strlen(s);
    for(int i = 0; i < len; i++) {
        if ( s[i] == '|' ) {
            count++;
        }
    }
    return count;
}

void closed(int* arr, int count) {
    int i = 0;
    while(i < count) { // There are while, so we need close every fork()
        close(arr[2 * i]);
        close(arr[2 * i + 1]);
        i++;
    }
}

void execute(char* command, int* arr, int count, int num) {
    int len = strlen(command);
    char **action = (char**)malloc(len);
    int i = 0;
    while( (action[i] = strtok_r(command, " ", &command)) ) {
        i += 1;
    }
    if (num != 0) {
        dup2(arr[2 * (num - 1)], 0);
    }
    if (num < (count - 1)) {
        dup2(arr[2 * num + 1], 1);
    }
    closed(arr, count);
    int check = execvp(action[0], action);
    if ( check < 0 ) {
        fprintf(stderr, "ERROR IN COMMAND\n");
        exit(6);
    }
}

void check(char* buf) {
        int len = strlen(buf);
        char **action = (char**)malloc(len);
        int i = 0;
        while( (action[i] = strtok_r(buf, " ", &buf)) ) {
            i += 1;
        }
        execvp(action[0], action);
        exit(2);
}

void dedebug(char* s) {
    char* str = "history ";
    if ( strcmp(str, s) == 0 ) {
        fprintf(stderr, "ERROR IN COMMAND\n");
        exit(5);
    }
}

int main(int argc, char** argv) {
    int fd;
    if ( argc != 2 ) {
        fprintf(stdout, "Error in number file\n");
        exit(1);
    }
    fd = open(argv[1], O_RDONLY);
    if ( fd < 0 ) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(2);
    }
    struct stat buffer;
    fstat(fd, &buffer);
    int size = buffer.st_size;
    char* buf = (char*)malloc(size);
    int debug_read = read(fd, buf, size - 1);
    if ( debug_read < 0 ) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(3);
    }
    int count = count_pipe(buf);
    if ( count == 0 ) { //обработка без |
      check(buf); 
    }
    count++;
    int pipe_arr[count * 2];
    for(int i = 0; i < count; i++) {
        pipe((int*)pipe_arr + i * 2);
    }
    char* buf2 = (char*)malloc(size);
    strcpy(buf2, buf);
    for(int i = 0; i < count; i++) {
        char* command = strtok_r(buf2, "|", &buf2);
        //dedebug(command);
        pid_t kid = fork();
        if ( kid == 0 ) {
            execute(command, (int*) pipe_arr, count, i);
        }
    }
    closed((int*) pipe_arr, count); // close last fork()
    for(int i = 0; i < count + 1; ++i) {
        wait(NULL);
    }
    free(buf);
    return 0;
}
