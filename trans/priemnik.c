#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdint.h>
#include <sys/stat.h>

int check_numbers(int args) {
    if ( args != 2 ) {
        fprintf(stderr, "ERROR IN NUMBER OF FILES\n");
        exit(1);
    }
}

int create_file(char* file) {
    int fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if ( fd < 0 ) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(2);
    }
}

void create_sem() {
    int sema = open("global.txt", O_CREAT | O_WRONLY, 0666);
    char* a = "1";
    write(sema, a, 1);
    close(sema);
}

void add1(int* buf, int count) {
    buf[count] = 1;
}

void add0(int* buf, int count) {
    buf[count] = 0;
}

void delete() {
    remove("global.txt");
}

void write_byte_one(int fd, int* buf) {
    int reply = 0;
    int pow = 1;
    for(int i = 0; i < 8; ++i) {
        reply += buf[i] * pow;
        pow *= 2;
    }
    char rep = (char)reply;
    write(fd, &rep, 1);
}

int main(int argc, char* argv[]) {
    check_numbers(argc);
    int note = create_file(argv[1]);
    pid_t pid = getpid();
    create_sem();
    printf("My pid = %d\nWait...\n", pid);
    int* byte = (int*) malloc(8 * sizeof(int));
    sigset_t* signals = (sigset_t*) malloc(sizeof(sigset_t));
    sigemptyset(signals);
    sigaddset(signals, SIGUSR1);
    sigaddset(signals, SIGUSR2);
    sigaddset(signals, SIGCHLD);
    sigprocmask(SIG_BLOCK, signals, NULL);
    int signal;
    int size = 0;
    siginfo_t* info = (siginfo_t*) malloc(sizeof(siginfo_t));
    while(1) {
        signal = sigwaitinfo(signals, info);
        if ( signal == -1 ) {
            fprintf(stderr, "ERROR IN SIGNAL\n");
            return 1;
        }
        if ( signal == SIGUSR1 ) {
            add0(byte, size);
            size++;
        }
        else if ( signal == SIGUSR2 ) {
            add1(byte, size);
            size++;
        }
        else if ( signal == SIGCHLD ) {
            kill(info->si_pid, SIGUSR1);
            break;
        }
        if ( size == 8 ) {
            write_byte_one(note, byte);
            size = 0;
        }
        kill(info->si_pid, SIGUSR1);
    }
    printf("THE END\n");
    free(info);
    free(byte);
    close(note);
    free(signals);
    delete();
    return 0;
}
