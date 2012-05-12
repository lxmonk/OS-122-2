#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

#include "kthread.h"


int
kthread_create(void*(*start_func)(), void* stack, uint stack_size) {
    return fork_kthread(start_func, stack);
}

int kthread_id() {
    return get_id();
}

void kthread_exit() {
    void proc_kthread_exit();
}

int kthread_join( int thread_id ) {
    return proc_kthread_join(thread_id);
}

int kthread_mutex_alloc() {
    int i;
    if (first_mutex) { //first mutex allocated, init the array to 0
        memset(mutex_p_array,0,4 * MAX_MUTEXES);
        first_mutex = 0;
        initlock(&array_lock,"arraylock");
    }
    //find the first empty place in the array
    acquire(&array_lock);
    for(i=0; i < MAX_MUTEXES;i++){
        if (mutex_p_array[i] == 0)
            break;
    }
    release(&array_lock);
    if (i == MAX_MUTEXES)
        return -1; //the mutex array is full
    mutex_p_array[i] = (void*)kalloc();
    if (mutex_p_array[i] == 0)
        return -1;
    initlock(&mutex_p_array[i]->lock,(char*)&i);
    memset(mutex_p_array[i]->waiting_kthreads,0,4 * NPROC);
    mutex_p_array[i]->first=0;
    mutex_p_array[i]->count=0;

    return i;
}

int kthread_mutex_dealloc( int mutex_id ) {
    acquire(&array_lock);
    kfree((char*)mutex_p_array[mutex_id]);
    mutex_p_array[mutex_id]=0;
    release(&array_lock);
    return 0;
}
int kthread_mutex_lock( int mutex_id ) {
    acquire(&(mutex_p_array[mutex_id]->lock));
    if (mutex_p_array[mutex_id]->count == NPROC) {
        release(&(mutex_p_array[mutex_id]->lock));
        return -1;
    }
    if (mutex_p_array[mutex_id]->count == 0) {
        //     mutex_p_array[mutex_id]->waiting_kthreads[mutex_p_array[mutex_id]->first] =get_current_kthread();
        mutex_p_array[mutex_id]->count++;
        release(&mutex_p_array[mutex_id]->lock);
        return 0;
    }

    return 0;
}
int kthread_mutex_unlock( int mutex_id );

int kthread_cond_alloc();
int kthread_cond_dealloc( int cond_id );
int kthread_cond_wait( int cond_id, int mutex_id );
int kthread_cond_signal( int cond_id );
