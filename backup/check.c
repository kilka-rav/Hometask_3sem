#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char** argv) {
    //int fd = open(argv[1], O_RDONLY); 
    struct stat buffer;
    int han = lstat(argv[1], &buffer);
    if ( (buffer.st_mode & S_IFMT) == S_IFREG ) { 
        printf("FILE\n");
    }   
    else if ( buffer.st_mode & S_IFDIR) {
        printf("DIRECTORY\n");
    }   
    else {
         printf("UNKMOWN\n");
    }   
    return 0;
}
 
