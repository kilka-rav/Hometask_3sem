#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>

#define ERROR(string) fprintf(stderr, "%s\n", string)

void check_arguments(int arg) {
    if ( arg != 3 ) {
        ERROR("ERROR IN INPUT");
        exit(1);
    }
}

void my_handler(int sig) {
    if ( sig == SIGINT ) {
        ERROR("ERROR IN SIGNAL");
        exit(2);
    }
}

void daemon(char* path_origin, char* path_backup) {


}

int main(int argc, char** argv) {
    check_arguments(argc);
    float minut, sec;
    minut = 0;
    sec = 15;
    sec = sec + minut * 60;
    pid_t main_pid = getpid();
    pid_t pid = fork();
    signal(SIGINT, my_handler);
    kill(main_pid, SIGINT);
    if ( pid == 0 ) {
        while(1) {
            daemon(argv[1], argv[2]);
            sleep(sec);

        }
    }
    return 0;
}

