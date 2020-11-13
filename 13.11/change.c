#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char** argv) {   
    char* str= argv[1];
    char **command = (char**)malloc(strlen(str));
	command[0] = strtok_r(str, " ", &str);
    execve(command[0], command, argv + 2);
    free(command);
}
