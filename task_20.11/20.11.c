#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

int main(int argc, char** argv) {
    DIR* dir;
    struct dirent* out;
    char* direc = argv[1];
    if ( dir = opendir(direc) ) {
        while( out = readdir(dir) ) {
            printf("%s\n", out->d_name);
        }
    }
    return 0;
}
