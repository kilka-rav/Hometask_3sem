#include "stack.h"

const char path[] = "/home/ravil/3sem/operator/hometask_2/Hometask_3sem/stack";

int timeout_flag = 0;
struct timespec timeout_time = {0, 0};

Stack_t* attach_stack(key_t key, int size) {
    Stack_t* stack = (Stack_t*)calloc(1, sizeof(Stack_t));
    if ( stack == NULL ) {
        ERROR("ERROR IN ATTACH");
        return stack;
    }

    void* memory = NULL; 
    int shmem_id = shmget(key, size, IPC_EXCL | 0666);
    int sem_id;
    if ( shmem_id > 0 ) {
        memory = shmat(shmem_id, NULL, 0);
        if ( memory == NULL ) {
            ERROR("ERROR");
        }
        sem_id = semget(key, 2, IPC_EXCL | 0666);
        if ( sem_id <= 0 ) {
            ERROR("ERROR IN SEMGET");
            free(stack);
            return NULL;
        }
    }
    else {
        shmem_id = shmget(key, size, IPC_CREAT | IPC_EXCL | 0666);
        check(shmem_id);
        memory = shmat(shmem_id, NULL, 0);
        sem_id = semget(key, 2, IPC_CREAT | IPC_EXCL | 0666);
        check(sem_id);
        sem_change(sem_id, 1, 1, 0);
    }

    stack->size = size;
    stack->memory = memory;
    stack->shmem_id = shmem_id;
    stack->sem_id = sem_id;
    return stack;
}

int detach_stack(Stack_t* stack) {
    if ( stack == NULL ) {
        ERROR("NOT STACK");
        return -1;
    }
    else {
        int shm_del = shmdt(stack->memory);
        check(shm_del);
        return 0;
    }
}

int mark_destruct(Stack_t* stack) {
    if ( stack == NULL ) {
        ERROR("NOT_STACK");
        return -1;
    }
    else {
        int shm_del = shmctl(stack->shmem_id, IPC_RMID, NULL);
        check(shm_del);
        shm_del = semctl(stack->sem_id, 0, IPC_RMID);
        check(shm_del);
        return 0;
    }
}

int get_size(Stack_t* stack) {
    if ( stack == NULL ) {
        return -1;
    }
    else {
        return stack->size;
    }
}

int get_count(Stack_t* stack) {
    if ( stack == NULL ) {
        return -1;
    }
    else {
        int val = semctl(stack->sem_id, 0, GETVAL);
        return val;
    }
}

int push(Stack_t* stack, void* val) {
    if ( stack == NULL ) {
        ERROR("NOT STACK");
        return -1;
    }
    if ( get_count(stack) >= get_size(stack) ) {
        ERROR("STACK IS OVERLOAD");
        return -1;
    }
    int waiting = sem_change(stack->sem_id, 1, -1, 1);
    if ( waiting == 0 ) {
        stack->memory[get_count(stack)] == val;
        sem_change(stack->sem_id, 0, 1, 0);
        sem_change(stack->sem_id, 1, 1, 1);
        return 0;
    }
    return -1;
}

int pop(Stack_t* stack, void** val) {
    if ( stack == NULL ) {
        ERROR("NOT STACK");
        return -1;
    }

    int waiting = sem_change(stack->sem_id, 1, -1, 1);
    if ( waiting == 0 ) {
        if ( get_count(stack) == 0 ) {
            ERROR("STACK IS EMPTY");
            sem_change(stack->sem_id, 1, 1, 1);
            return -1;
        }
        *val = stack->memory[get_count(stack) - 1];
        sem_change(stack->sem_id, 0, -1, 0);
        sem_change(stack->sem_id, 1, 1, 1);
        return 0; 
    }
    return -1;    
}

int set_wait(int val, struct timespec* timeout) {
    timeout_flag = val;
    if ( timeout != NULL ) {
        timeout_time.tv_nsec = timeout->tv_nsec;
        timeout_time.tv_sec = timeout->tv_sec;
    }
}

int sem_change(int sem_id, int sem_num, int val, int undo) {
    int res;
    struct sembuf sems;
    sems.sem_num = sem_num;
    sems.sem_op = val;

    if ( sem_num == 0 ) {
        sems.sem_flg = 0;
        return semop(sem_id, &sems, 1);
    }
    else if ( sem_num == 1 ) {
        if (timeout_flag == -1) {
            if (undo == 1) {
                sems.sem_flg = IPC_NOWAIT | SEM_UNDO;
            }
            else { 
                sems.sem_flg = IPC_NOWAIT;
            }
            res = semop(sem_id, &sems, 1);
            return res; 
        }
        else if (timeout_flag == 0) {
            if ( undo == 1 ) {
                sems.sem_flg = SEM_UNDO;
            }
            else {
                sems.sem_flg = 0;
            }
            res = semop(sem_id, &sems, 1);
            return res; 
        }
        else if ( timeout_flag == 1 ) {
            if ( undo == 1 ) {
                sems.sem_flg = SEM_UNDO;
            }
            else {
                sems.sem_flg = 0;
            }
            res = semtimedop(sem_id, &sems, 1, &timeout_time);
            return res; 
        } 
    }
    return -1;
}

void unit_test2(key_t key) {
    const int size = 1000000;
    const int push_count = 10;
    const int fork_count = 11;
    pid_t main_pid = getpid();
    pid_t pid;
    setgid(main_pid);
    for (int i = 0; i < 2; i++)
    {
        pid = fork();
    }

    Stack_t* stack = attach_stack(key, size);
    for (int j = 0; j < push_count; j++) {
        void* a = NULL + j + pid * 5;
        int check = push(stack, a);
        printf("count == %d size = %d\n", get_count(stack), get_size(stack));
    }
    for (int j = 0; j < push_count; j++) {
        void* a;
        int check = pop(stack, &a);
    }
    if (getpid() == main_pid) {
        void* a;
        sleep(1);
        setgid(main_pid + 1);
        killpg(main_pid, SIGKILL);
        push(stack, NULL);
        push(stack, NULL);
        pop(stack, &a);
        pop(stack, &a);
        printf("TEST 3 =  %d\n", get_count(stack));
        detach_stack(stack);
        mark_destruct(stack);
    }
    else
    {
        printf("end of the child with pid %d\n", getpid());
        detach_stack(stack);
    }
}
void unit_test1(key_t key) {
    printf("UNIT_TES1\nRUN\n_________________________________\n");
    Stack_t* s = attach_stack(key, 5);
    for(size_t i = 0; i < 5; i++) {
        printf("count %d\n", get_count(s));
        printf("size %d\n", get_size(s));
        push(s, (void*)(i+1));
    }
    for(size_t j = 0; j < 6; j++){
        void* val = NULL;
        pop(s, &val);
        printf("%p\n", val);
    }
    int a = detach_stack(s);
    mark_destruct(s);
    printf("%d\n_______________________________________________\nEND\n", a);
}

int main(int argc, char** argv) {
    key_t key = atoi(argv[1]);
    unit_test1(key);
    unit_test2(key + 2);
    }
