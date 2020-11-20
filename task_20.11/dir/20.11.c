#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

int main(int argc, char** argv) {
    DIR* dir;
    struct dirent* lol;
    char* direc = argv[1];
    if ( dir = opendir(direc) ) {
        while( lol = readdir(dir) ) {
            printf("%s\n", lol->d_name);
        }
    }
    return 0;
}
