/* #include "defs.h" */
/* #include "param.h" */
/* #include "memlayout.h" */
/* #include "mmu.h" */
/* #include "x86.h" */
/* #include "proc.h" */
/* #include "spinlock.h" */

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"


#define MAX_STACK_SIZE 4000
#define MAX_MUTEXES 64
#define MAX_CONDS 64


typedef struct {
    struct spinlock lock;
    int waiting_kthreads[NPROC];
    int first;
    int count;
} kthread_mutex_t;

/* int sys_kthread_create(void*(*start_func)(), void* stack, */
/*                    uint stack_size); */
/* int sys_kthread_id(); */
/* void sys_kthread_exit(); */
/* int sys_kthread_join( int thread_id ); */
/* void sys_kthread_yield(); */

/* int sys_kthread_mutex_alloc(); */
/* int sys_kthread_mutex_dealloc( int mutex_id ); */
/* int sys_kthread_mutex_lock( int mutex_id ); */
/* int sys_kthread_mutex_unlock( int mutex_id ); */


typedef struct {
    int waiting_kthreads[NPROC];
    int first;
    int count;
} kthread_cond_t;


/* int sys_kthread_cond_alloc(); */
/* int sys_kthread_cond_dealloc( int cond_id ); */
/* int sys_kthread_cond_wait( int cond_id, int mutex_id ); */
/* int sys_kthread_cond_signal( int cond_id ); */


/* void* sys_kthread_get_ustack(); */
