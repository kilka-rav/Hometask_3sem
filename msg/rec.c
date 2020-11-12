#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <unistd.h>
#include <fcntl.h>
#include <sys/msg.h>

#define MAX 256
//const char* mes_gen_path = "/home/sergey/messages/mes_gen";

typedef struct _Message {
    long mtype;
    pid_t pid;
} Message;

const long forward = 1;
const long back = 2;

int main() {
    char* path_to_file = (char*) malloc(MAX);
    getcwd(path_to_file, MAX);
    key_t key = ftok(path_to_file, 1);    
    int id = msgget(key, 0);
    Message msg;

    int size = msgrcv(id, &msg, sizeof(Message), forward, 0);
    printf("pid from message: %d\n", msg.pid);

    pid_t pid = getpid();
    msg.pid = pid;
    msg.mtype = back;
    printf("my pid: %d\n", pid);
    msgsnd(id, &msg, sizeof(Message), 0);
    free(path_to_file);
    return 0;
}
