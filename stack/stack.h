#ifndef _STACK_H_
#define _STACK_H_

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <assert.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define ERROR(string) fprintf(stderr, "%s\nLINE = %d\n", string, __LINE__)

typedef struct _Stack_t {
    int size;
    void** memory;
    int shmem_id;
    int sem_id;
} Stack_t;

void check(int arg) {
    if ( arg < 0 ) {
        ERROR(strerror(errno));
        exit(1);
    }
}
/* Attach (create if needed) shared memory stack to the process.
Returns Stack_t* in case of success. Returns NULL on failure. */
Stack_t* attach_stack(key_t key, int size);

/* Detaches existing stack from process. 
Operations on detached stack are not permitted since stack pointer becomes invalid. */
int detach_stack(Stack_t* stack);

/* Marks stack to be destroed. Destruction are done after all detaches */ 
int mark_destruct(Stack_t* stack);

/* Returns stack maximum size. */
int get_size(Stack_t* stack);

/* Returns current stack size. */
int get_count(Stack_t* stack);

/* Push val into stack. */
int push(Stack_t* stack, void* val);

/* Pop val from stack into memory */
int pop(Stack_t* stack, void** val);

/* Change semaphore */
int sem_change(int sem_id, int sem_num, int val, int undo);

//---------------------------------------------
/* Additional tasks */

/* Control timeout on push and pop operations in case stack is full or empty.
val == -1 Operations return immediatly, probably with errors.
val == 0  Operations wait infinitely.
val == 1  Operations wait timeout time.
*/
int set_wait(int val, struct timespec* timeout);

#endif
