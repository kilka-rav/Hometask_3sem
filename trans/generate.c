#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int ASSERT;
pid_t pid;

void rewrite() {
    FILE* you = fopen("global.txt", "w");
    fprintf(you, "%d", 1); 
    fclose(you);
    exit(32);
}

void checking() {
    FILE* my = fopen("global.txt", "r");
    fscanf(my, "%d", &ASSERT);
    fclose(my);
    if ( ASSERT > 1 ) {
//        rewrite();
        printf("ERROR IN PRIEMNIK\n");
        exit(5);
    }
    else {
        FILE* you = fopen("global.txt", "w");
        fprintf(you, "%d", 2);
        fclose(you);
    }
}

typedef struct _Time {
    clock_t begin;
    clock_t end;
} Time;

void check_number_arguments(int argc) {
      if ( argc != 3 ) {
        fprintf(stderr, "Wrong numbers of files\n");
        exit(1);
    }
}
pid_t get_my_pid(char* s) {
    pid_t ret = atoi(s);
    if ( ret == 0 ) {
        fprintf(stderr, "ERROR IN PID\n");
        exit(2);
    }
}

void check_open(int fd) {
    if ( fd < 0 ) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(3);
    }
}


size_t get_size(int fd) {
    struct stat buffer;
    fstat(fd, &buffer);
    return buffer.st_size;
}

void send_byte(int* arr, sigset_t* signals) {
    for(int i = 0; i < 8; ++i) {
        if ( arr[i] == 0 ) {
            kill(pid, SIGUSR1);
        }
        else {
            kill(pid, SIGUSR2);
        }
        sigwaitinfo(signals, NULL);
    }
}

void masked(char c, int* arr) {
    unsigned num = (unsigned) c;
    for(int i = 0; i < 8; ++i) {
        arr[i] = (1 & num);
        num = num >> 1;
    }
}

void send_file(char* buf, size_t sz, sigset_t* signals, int fd) {
    size_t count = 0;
    while(count < sz) {
        int arr[8];
        read(fd, buf, 1);
        masked(*buf, arr);
        send_byte(arr, signals);
        count++;
    }
    kill(pid, SIGCHLD);
}

void delete() {
    remove("global.txt");
}

void print_time(Time Count, size_t sz) {
    double time = (Count.end - Count.begin) * 0.000001;
    printf("Time = %lf sec\n", time);
    printf("Size = %ld\n", sz);
    printf("Speed = %.2lf bytes/sec\n", (sz / time));
}

void print_end(int sig) {
    if ( sig == SIGINT ) {
        kill(pid, SIGCHLD);
        printf("\nYOU STOPPED TRANSMISSION\n");
        exit(2);
    }
}
int main(int argc, char* argv[]) {
    check_number_arguments(argc);
    pid = get_my_pid(argv[2]);
    char* name = argv[1];
    printf("%s\npid = %d\n", name, pid);
    checking();
    int fd = open(name, O_RDONLY);
    check_open(fd);
    size_t sz = get_size(fd);
    char* buf = (char* ) malloc(sz);
    printf("size bytes %ld\n", sz);
    Time Count;
    signal(SIGINT, print_end);
    sigset_t* signals = (sigset_t*) malloc(sizeof(sigset_t));
    sigemptyset(signals);
    sigaddset(signals, SIGUSR1);
    sigaddset(signals, SIGUSR2);
    int zorro = sigprocmask(SIG_BLOCK, signals, NULL);
    if ( zorro < 0 ) {
        fprintf(stderr, "ERROR IN SIGPROCMASK\n");
        return 1;
    }
    Count.begin = clock();
    send_file(buf, sz, signals, fd);
    Count.end = clock();
    print_time(Count, sz);
    free(buf);
    close(fd);
    //rewrite();
    //delete();
    return 0;
}
