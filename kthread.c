#include "defs.h"
#include "spinlock.h"
#include "kthread.h"


int kthread_create( void*(*start_func)(), void* stack, uint stack_size ) {
    return fork_kthread(start_func, stack);

}

int kthread_id() {return -1;}
void kthread_exit() {}
int kthread_join( int thread_id ) {return -1;}
