#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>

#define ERROR(s) fprintf(stderr, "%s\n", s)

#define BUFLEN 1024

typedef struct dirent Dirent;

const char s1[] = ".";
const char s2[] = "..";

int logg; //log-descriptor

void check_arg(int arg) {
    if ( arg != 3 ) {
        fprintf(stderr, "ERROR IN INPUT\n");
        exit(1);
    }
}

void check(int arg) {
    if ( arg < 0 ) {
        ERROR(strerror(errno));
        exit(2);
    }
}

void check_contains(char* origin, char* backup) {
    if (strstr(origin, backup) == NULL ) {
        return;
    }   
    else {
        ERROR("ERROR IN PATH");
        exit(5);
    }   
}



int check_dir(char* path, char* name) {
    char* result = malloc(strlen(path) + strlen(name));
    result[0] = '\0';
    strcat(result, path);
    strcat(result, "/");
    strcat(result, name);
    struct stat buffer;
    int han = lstat(result, &buffer);
    check(han);
    free(result);
    if ( (buffer.st_mode & S_IFMT) == S_IFREG ) {
        return 1;
    }
    else if ( (buffer.st_mode & S_IFMT) == S_IFDIR ) {
        return 2;
    }
    else if ( (buffer.st_mode & S_IFMT ) == S_IFLNK ) {
        return 3;
    }
    else {
        return -1;
    }
}

time_t clock_modification(char* path) {
    struct stat buf;
    stat(path, &buf);
    return buf.st_mtime;
}

void write_log(char* path) {
    time_t last_time = clock_modification(path);
    char* record = malloc(strlen(path) + 20);
    sprintf(record, "%s %ld\n", path, last_time);
    printf("record = %s\n", record);
    write(logg, record, strlen(record));
    free(record);
}
    

void copy_link(char* path, char* back, char* name) {
    char* arg_one = malloc(strlen(path) + strlen(name) + 1);
    char* arg_two = malloc(strlen(path) + strlen(name) + 1);
    sprintf(arg_one, "%s/%s", path, name);
    sprintf(arg_two, "%s/%s", back, name);
    char cmd[] = "cp";
    char* argv[6];
    argv[0] = cmd;
    argv[1] = "-r";
    argv[2] = "-L";
    argv[3] = arg_one;
    argv[4] = arg_two;
    argv[5] = NULL;
    int status = 0;
    int pid = fork();
    if ( pid == 0 ) {
        execvp(cmd, argv);
    }
    wait(&status);
    write_log(arg_one);
    free(arg_one);
    free(arg_two);
}

void copy_file(char* arg_one, char* arg_two, char* name) {
    int len = strlen(arg_one) + strlen(name);
    char* path_original = (char*) malloc(len);
    path_original[0] = '\0';
    strcat(path_original, arg_one);
    strcat(path_original, "/");
    strcat(path_original, name);
    write_log(path_original);
    len = strlen(arg_two) + strlen(name);
    char* path_back = (char*) malloc(len);
    path_back[0] = '\0';
    strcat(path_back, arg_two);
    strcat(path_back, "/");
    strcat(path_back, name);
    int fd_orig = open(path_original, O_RDONLY);
    check(fd_orig);
    int fd_backup = open(path_back, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    check(fd_backup);
    struct stat buffer;
    fstat(fd_orig, &buffer);
    size_t size = buffer.st_size;
    char* buf = (char*) malloc(size);
    int debug = read(fd_orig, buf, size);
    check(debug);
    debug = write(fd_backup, buf, size);
    free(buf);
    free(path_original);
    free(path_back);
}

void recursive_down(char* arg_one, char* arg_two, DIR* dir, Dirent* entry) {
    while( entry = readdir(dir) ) { 
        if ( (strcmp(entry->d_name, s1) != 0 ) & (strcmp(entry->d_name, s2) != 0 ) ) {
            int action = check_dir(arg_one, entry->d_name);
            if ( action == 1 ) {
                copy_file(arg_one, arg_two, entry->d_name);
            }
            else if ( action == 2 ) {
                char* result = (char*) malloc(strlen(arg_one) + strlen(entry->d_name));
                result[0] = '\0';
                strcat(result, arg_one);
                strcat(result, "/");
                strcat(result, entry->d_name);
                char* res_back = (char*) malloc(strlen(arg_two) + strlen(entry->d_name));
                res_back[0] = '\0';
                strcat(res_back, arg_two);
                strcat(res_back, "/");
                strcat(res_back, entry->d_name);
                Dirent* entry_local;
                int create = mkdir(res_back, S_IRWXU);
                check(create);
                DIR* dir_local = opendir(result);
                printf("dir_file: %s\ndir_back %s\n", result, res_back);
                recursive_down(result, res_back, dir_local, entry_local);
                free(result);
                free(res_back);
            }
            else if ( action == 3 ) {
                copy_link(arg_one, arg_two, entry->d_name);
            }
            else if ( action == -1 ) {
                ERROR("UNKNOWN TYPE FILE");
            }
        }
    }
    return;

}

int main(int argc, char** argv) {
    printf("__________________________\nRUN\n");
    check_arg(argc);
    check_contains(argv[1], argv[2]);
    int create = mkdir(argv[2], S_IRWXU);
    check(create);
    unlink("log.txt");
    logg = open("log.txt", O_CREAT | O_RDWR, 0777);
    check(logg);
    DIR* dir;
    Dirent* entry;
    dir = opendir(argv[1]);
    recursive_down(argv[1], argv[2], dir, entry);
    closedir(dir);
    close(logg);
    return 0;
}
