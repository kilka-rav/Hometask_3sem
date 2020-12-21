#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <sys/wait.h>

#define ERROR(string) fprintf(stderr, "%s\n", string)

typedef struct dirent Dirent;

const char s1[] = ".";
const char s2[] = "..";

int logg;

void check_arguments(int arg) {
    if ( arg != 3 ) {
        ERROR("ERROR IN INPUT");
        exit(1);
    }
}

void check(int arg) {
    if ( arg < 0 ) {
        ERROR(strerror(errno));
        exit(3);
    }
}

void my_handler(int sig) {
    if ( sig == SIGINT ) {
        ERROR("ERROR IN SIGNAL");
        exit(2);
    }
}

void contains(char* origin, char* backup) {
    if ( (strstr(origin, backup) == NULL ) & (strstr(backup, origin) == NULL) ) { 
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
    else if ( (buffer.st_mode & S_IFMT) == S_IFLNK ) {
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

int compare(char* string) {
    FILE* f = fopen("log.txt", "r");
    char* buf = malloc(128);
    rewind(f);
    while( !(feof(f)) ) {
        fgets(buf, 128, f);
        if ( strncmp(string, buf, strlen(string) - 1) == 0 ) {
            free(buf);
            fclose(f);
            return 1;
        }
    }
    free(buf);
    fclose(f);
    return 0;
}

int check_change(char* path, char* name) {
    char* result = (char*) malloc(strlen(path) + strlen(name) + 30);
    sprintf(result, "%s/%s", path, name);
    clock_t local_time = clock_modification(result);
    sprintf(result, "%s %ld", result, local_time);
    if ( compare(result) == 0 ) {
        return 0;
    }
    return 1;
}

void write_log(char* path) {
    time_t last_time = clock_modification(path);
    char* record = malloc(strlen(path) + 20);
    sprintf(record, "%s %ld\n", path, last_time);
    write(logg, record, strlen(record));
    free(record);
}


void copy_file(char* arg_one, char* arg_two, char* name) {
    char* path_original = (char*) malloc(strlen(arg_one) + strlen(name) + 1);
    sprintf(path_original, "%s/%s", arg_one, name);
    write_log(path_original); 
    char* path_back = (char*) malloc(strlen(arg_two) + strlen(name) + 1);
    path_back[0] = '\0';
    strcat(path_back, arg_two);
    DIR* lmao = opendir(path_back);
    if ( lmao == NULL ) {
        mkdir(path_back, S_IRWXU);
    }
    closedir(lmao);
    strcat(path_back, "/");
    strcat(path_back, name);
    int fd_orig = open(path_original, O_RDONLY);
    check(fd_orig);
    int fd_backup = open(path_back, O_CREAT | O_WRONLY, 0666);
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
/*
void copy_link(char* path, char* back, char* name) {
    contains(path, name);
    copy_file(path, back, name);
}
*/



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




void recursive_down(char* path_origin, char* path_backup, DIR* dir, Dirent* entry) {
    while( entry = readdir(dir) ) {
        if ( (strcmp(entry->d_name, s1) != 0 ) & (strcmp(entry->d_name, s2) != 0 ) ) {
            int action = check_dir(path_origin, entry->d_name);
            if ( action == 1 ) {
                if ( check_change(path_origin, entry->d_name) == 0 ) {
                    copy_file(path_origin, path_backup, entry->d_name);
                }
            }
            else if ( action == 2 ) {
                char* result = (char*) malloc(strlen(path_origin) + strlen(entry->d_name));
                sprintf(result, "%s/%s", path_origin, entry->d_name);
                char* res_back = (char*) malloc(strlen(path_backup) + strlen(entry->d_name));
                sprintf(res_back, "%s/%s", path_backup, entry->d_name);
                DIR* lmao = opendir(res_back);
                if ( lmao == NULL ) {
                    mkdir(res_back, S_IRWXU);
                }
                closedir(lmao);
                Dirent* entry_local;
                DIR* dir_local = opendir(result);
                recursive_down(result, res_back, dir_local, entry_local);
                free(result);
                //free(res_back);
            }
            else if ( action == 3 ) {
                if ( check_change(path_origin, entry->d_name) == 0 ) {
                    copy_link(path_origin, path_backup, entry->d_name);
                }
            }
            else if ( action == -1 ) {
                ERROR("UNKNOWN TYPE FILE");
            }
        }
    }
    return;

}



void my_daemon(char* path_origin, char* path_backup) {
    DIR* dir;
    Dirent* entry;
    dir = opendir(path_origin);
    recursive_down(path_origin, path_backup, dir, entry);
}

int main(int argc, char** argv) {
    check_arguments(argc);
    contains(argv[1], argv[2]);
    float minut, sec;
    minut = 0;
    sec = 15;
    sec = sec + minut * 60;
    pid_t main_pid = getpid();
    pid_t pid = fork();
    kill(main_pid, SIGKILL);
    printf("MY pid %d\n", getpid());
    logg = open("log.txt", O_RDWR);
    check(logg);
    if ( pid == 0 ) {
        while(1) {
            my_daemon(argv[1], argv[2]);
            sleep(2);
        }
    }
    return 0;
}
