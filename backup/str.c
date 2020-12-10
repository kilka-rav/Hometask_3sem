#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char* s1 = malloc(12312);
    char* s2 = "VAVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV";
    char* s3 = strcat(s1, s2);
    s3 = strcat(s3, s2);
    printf("%s\n", s3);
    return 0;
}

