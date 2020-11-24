#include "stack.h"

int main(int argc, char** argv) {
    fork();
    fork();
    key_t key = atoi(argv[1]);
    stack_t* s = attach_stack(key, 5);
    for(size_t i = 0; i < 5; i++) {
        printf("count %d\n", get_count(s));
        printf("size %d\n", get_size(s));
        push(s, (void*)(i+1));
        printf("i = %ld\n", i);
        printf("i = %ld push\n", (size_t)s->mem[i]);
    }
    for(size_t j = 0; j < 6; j++){
        void* val = NULL; 
        pop(s, &val);
        printf("%p\n", val);
    }
    int a = detach_stack(s);
    mark_destruct(s);
    printf("%d\n", a);
}
