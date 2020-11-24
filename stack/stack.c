#include "stack.h"


int global_wait = 0; // 1 - wait, 0 - run


void message_error(int error) {
    if ( error < 0 ) {
        ERROR(strerror(errno));
        exit(1);
    }
}

stack_t* attach_stack(key_t key, int size) {
    stack_t* stack = (stack_t*) malloc(sizeof(stack_t));
    if ( !stack ) {
        return NULL;
    }
    int id = shmget(key, size * sizeof(void*) + 3 * sizeof(int), IPC_CREAT | IPC_EXCL | 0666);
    if ( (id < 0) && (errno != EEXIST) ) {
        ERROR("Error in allocation");
        message_error(id);
        exit(2);
    }
    int sema = semget(key, 1, IPC_CREAT | IPC_EXCL | 0777);
    //message_error(sema);
    void* mem = shmat(id, NULL, 0);
    init_sema(0);
    increment_sem(sema);
    stack->key = key;
    stack->data[0] = size;
    stack->data[1] = 0;
    stack->data[2] = sema;
    stack->mem = mem;
    printf("size: %d\n", stack->data[0]);
    return stack;
}

int detach_stack(stack_t* stack) {
    int reply = shmdt((void*)stack->mem);
    message_error(reply);
    return reply;
}

int push(stack_t* stack, void* val) {
    decrement_sem(stack->data[3]);
    if ( stack->data[0] == stack->data[1] ) {
        ERROR("STACK IS OVERLOAD");
        return -1;
    }
    stack->mem[stack->data[1]] = val;
    stack->data[1]++;
    increment_sem(stack->data[3]);
    return 0;
}

int pop(stack_t* stack, void** val) {
    decrement_sem(stack->data[3]);
    if ( stack->data[1] <= 0 ) {
        *val = NULL;
        ERROR("STACK IS EMPTY");
        return -1;
    }
    //add handler
    *val = stack->mem[stack->data[1] - 1];
    stack->data[1]--;
    increment_sem(stack->data[3]);
    return 0;
}

int mark_destruct(stack_t* stack) {
    int id = shmget(stack->key, stack->data[0] * sizeof(void*), 0);
    message_error(id);
    int check = semctl(stack->data[2], 1, IPC_RMID);
    message_error(id);
    int reply = shmctl(id, IPC_RMID, NULL);
    message_error(reply);
    return reply;
}




int get_size(stack_t* stack) {
    return stack->data[0];
}

int get_count(stack_t* stack) {
    return stack->data[1];
}

Sembuf* init_sema(int num) {
    static Sembuf semafor;
    if ( num == 0 ) {
        semafor.sem_num = num; //next SIGSEGV
        if ( global_wait == 1 ) {
            semafor.sem_flg = SEM_UNDO;
        }
        else if ( global_wait == 0 ) {
            semafor.sem_flg = IPC_NOWAIT | SEM_UNDO;
        }
        else {
            fprintf(stderr, "ERROR IN SEMA\n");
            exit(3);
        }
    }
    else {
        semafor.sem_num = num;
    }
    return &semafor;
}

int increment_sem(int sem) {
    if ( global_wait = 1 ) {
        return semtimedop(sem, init_sema(1), 1, NULL); 
    }
    else {
        return semop(sem, init_sema(1), 1);
    }
}

int decrement_sem(int sem) { 
    if ( global_wait == 1 ) {
        return semtimedop(sem, init_sema(-1), 1, NULL);
    }
    else {
        return semop(sem, init_sema(-1), 1);
    }
}


