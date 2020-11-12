#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <unistd.h>
#include <fcntl.h>
#include <sys/msg.h>

#define MAX 256

typedef struct _Message {
    long type;
    pid_t pid;
} Message;

const long forward = 1;
const long back = 2;

Message init(pid_t pid) {
    Message ms;
    ms.pid = pid;
    ms.type = forward;
    return ms;
}

int main() {
    char* path_to_file = (char*) malloc(MAX);
    getcwd(path_to_file, MAX);
    key_t key = ftok(path_to_file, 1);    
    int id = msgget(key, IPC_CREAT | 0666);
    pid_t pid = getpid();
    Message msg = init(pid);;
    printf("my pid: %d\n", pid);
    msgsnd(id, &msg, sizeof(Message), 0);
    int size = msgrcv(id, &msg, sizeof(Message), back, 0);
    printf("pid from message: %d\n", msg.pid);
    free(path_to_file);
    return 0;
}
