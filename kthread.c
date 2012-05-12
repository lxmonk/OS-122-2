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
        initlock(&array_lock,"arraylock");
        acquire(&array_lock);
        if (first_mutex == 1){
            memset(mutex_used,0,sizeof(int) * MAX_MUTEXES);
            first_mutex = 0;
        }
        release(&array_lock);
    }
    //find the first empty place in the array
    acquire(&array_lock);
    for(i=0; i < MAX_MUTEXES;i++){
        if (mutex_used[i] == 0) {
            mutex_used[i] = 1;
            break;
        }
    }
    release(&array_lock);
    //the mutex array is full
    if (i == MAX_MUTEXES)
        return -1;
    //init the mutex
    initlock(&(mutex_array[i].lock),(char*)&i);
    memset(mutex_array[i].waiting_kthreads,0,sizeof(int) * NPROC);
    mutex_array[i].first=0;
    mutex_array[i].count=0;

    return i;
}

int kthread_mutex_dealloc( int mutex_id ) {
    acquire(&array_lock);
    mutex_used[mutex_id]=0;
    release(&array_lock);
    return 0;
}

int kthread_mutex_lock( int mutex_id ) {

    kthread_mutex_t *mutex;
    acquire(&(mutex_array[mutex_id].lock));
    mutex=&(mutex_array[mutex_id]);
    // no room for new kthread
    if (mutex->count == NPROC) {
        release(&(mutex->lock));
        return -1;
    }
    //only kthread in the mutex
    if (mutex->count == 0) {
        mutex->waiting_kthreads[mutex->first] = kthread_id();
        mutex->count++;
        release(&(mutex->lock));
        return 0;
    }
    //join the waiting kthread
    mutex->waiting_kthreads[mutex->first + mutex->count] = kthread_id();
    mutex->count++;
    kthread_block(mutex->waiting_kthreads[mutex->first + mutex->count]);
    release(&(mutex->lock));

    return 0;
}
int kthread_mutex_unlock( int mutex_id ){

    kthread_mutex_t *mutex;
    acquire(&(mutex_array[mutex_id].lock));
    mutex=&(mutex_array[mutex_id]);
    // no threads waiting or mutex is not mine
    if ((mutex->count == 0) ||
        (mutex->waiting_kthreads[mutex->first] != kthread_id())) {
            release(&(mutex->lock));
            return -1;
    }
    if (mutex->count > 1) {
        mutex->first = (mutex->first + 1) % NPROC;
        kthread_UNblock(mutex->first);
    }
    mutex->count--;
    release(&(mutex->lock));

    return 0;
}

int kthread_cond_alloc();
int kthread_cond_dealloc( int cond_id );
int kthread_cond_wait( int cond_id, int mutex_id );
int kthread_cond_signal( int cond_id );
