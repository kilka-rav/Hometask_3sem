#define _GNU_SOURCE
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>


#define ERROR(string) fprintf(stderr, "%s\n", string)
typedef struct sembuf Sembuf;

typedef struct _stack_t {
    void** mem;
    key_t key;
    int data[3]; //data[0] - size, data[1] - count, data[2] - sem
    //int size;
    //int count;
    //int sem;
} stack_t;

/* Attach (create if needed) shared memory stack to the process.
Returns stack_t* in case of success. Returns NULL on failure. */
stack_t* attach_stack(key_t key, int size);

/* Detaches existing stack from process. 
Operations on detached stack are not permitted since stack pointer becomes invalid. */
int detach_stack(stack_t* stack);

/* Marks stack to be destroed. Destruction are done after all detaches */ 
int mark_destruct(stack_t* stack);

/* Returns stack maximum size. */
int get_size(stack_t* stack);

/* Returns current stack size. */
int get_count(stack_t* stack);

/* Push val into stack. */
int push(stack_t* stack, void* val);

/* Pop val from stack into memory */
int pop(stack_t* stack, void** val);

Sembuf* init_sema(int num);
int increment_sem(int sem);
int decrement_sem(int sem);

//---------------------------------------------
/* Additional tasks */

/* Control timeout on push and pop operations in case stack is full or empty.
val == -1 Operations return immediatly, probably with errors.
val == 0  Operations wait infinitely.
val == 1  Operations wait timeout time.
*/
//int set_wait(int val, timespec* timeout);
