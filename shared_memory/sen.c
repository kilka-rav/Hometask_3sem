#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/shm.h>
#include <errno.h>

const char* mem_gen_path = "/home/ravil/3sem/operator/hometask_2/Hometask_3sem/shared_memory/rec.c";
const char* size_gen_path = "/home/ravil/3sem/operator/hometask_2/Hometask_3sem/shared_memory/sen.c";

void check(int argc) {
    if ( argc != 2 ) { 
        fprintf(stderr, "ERROR IN ARGS\n");
        exit(1);
    }   
}

int check_open(char* name) {
    int fd = open(name, O_RDONLY, 0666);
    if ( fd < 0 ) { 
        fprintf(stderr, "%s\n", strerror(errno));
        exit(2);
    }   
    return fd; 
}

int get_size(int fd) {
    struct stat buffer;
    fstat(fd, &buffer);
    return buffer.st_size;
}

int main (int argc, char** argv) {
    check(argc);
    int fd = check_open(argv[1]);
    int sz = get_size(fd);
    char* buf = (char*) malloc(sz);
    int book = read(fd, buf, sz);
    key_t key = ftok(mem_gen_path, 1);    
    int id = shmget(key, sz, IPC_CREAT | 0666);
    void* memory = shmat(id, NULL, 0);
    memcpy(memory, buf, sz);
    key_t key2 = ftok(size_gen_path, 1); 
    int id2 = shmget(key2, sizeof(int), IPC_CREAT | 0666);
    void* sh_sz = shmat(id2, NULL, 0);
    memcpy(sh_sz, &sz, sizeof(int));
    shmdt(memory);
    shmdt(sh_sz);
    free(buf);
    close(fd);
}
