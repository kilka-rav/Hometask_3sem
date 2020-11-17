#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/shm.h>
#include <errno.h>

const char* mem_gen_path = "/home/ravil/3sem/operator/hometask_2/shared_memory/rec.c";
const char* size_gen_path = "/home/ravil/3sem/operator/hometask_2/shared_memory/sen.c";

void check(int argc) {
    if ( argc != 2 ) {
        fprintf(stderr, "ERROR IN ARGS\n");
        exit(1);
    }
}

int check_open(char* name) {
    int fd = open(name, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if ( fd < 0 ) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(2);
    }
    return fd;
}

int main (int argc, char** argv) {
    check(argc);
    int fd = check_open(argv[1]);
    key_t key = ftok(mem_gen_path, 1);    
    key_t key2 = ftok(size_gen_path, 1);    
    int id2 = shmget(key2, sizeof(int), IPC_CREAT | 0666);
    void* sh_sz = shmat(id2, NULL, 0);
    int size;
    memcpy(&size, sh_sz, sizeof(int));
    int id = shmget(key, size, IPC_CREAT | 0666);
    void* memory = shmat(id, NULL, 0); 
    char* buf = (char*) malloc(size);
    memcpy(buf, memory, size);
    write(fd, buf, size);
    shmdt(sh_sz);
    shmdt(memory);
    shmctl(id, IPC_RMID, NULL);
    shmctl(id2, IPC_RMID, NULL);
    free(buf);
    close(fd);
}
