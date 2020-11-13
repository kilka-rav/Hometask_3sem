#include <stdio.h>
#define MY_VAR

int main(int argc, char** argv, char** envp) {
    if ( argc != 1) {
        fprintf(stderr, "Error in arguments");
        return 1;
    }
    for(int i = 0; envp[i]; ++i) {
        printf("%s\n", envp[i]);
    }
    return 0;
}

