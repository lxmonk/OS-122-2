#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

#include "kthread.h"

#define SALAD 0
#define PASTA 1
#define STEAK 2

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))


static int host_mutex, salad_mutex, pasta_mutex, steak_mutex;
static int host_cv, salad_cv, pasta_cv, steak_cv;
static int chair_num, waiting_students;

int dealloc_ret(int allocated, int ids[]) {

    while(allocated > 12)    /* loop over all created students */
        kfree((void*)ids[allocated--]);

    switch(allocated) {	/* no 'break;;' on purpose! */
    case 12:			/* steak waiter stack */
        kfree((void*)ids[allocated--]);
    case 11:			/* pasta waiter stack */
        kfree((void*)ids[allocated--]);
    case 10:			/* salad waiter stack */
        kfree((void*)ids[allocated--]);
    case 9:			/* host stack */
        kfree((void*)ids[allocated--]);
    case 8:			/* steak_cv */
        kthread_cond_dealloc(ids[allocated--]);
    case 7:			/* pasta_cv */
        kthread_cond_dealloc(ids[allocated--]);
    case 6:			/* salad_cv */
        kthread_cond_dealloc(ids[allocated--]);
    case 5:			/* host_cv */
        kthread_cond_dealloc(ids[allocated--]);
    case 4:			/* steak_mutex */
        kthread_mutex_dealloc(ids[allocated--]);
    case 3:			/* pasta_mutex */
        kthread_mutex_dealloc(ids[allocated--]);
    case 2:			/* salad_mutex */
        kthread_mutex_dealloc(ids[allocated--]);
    case 1:			/* host_mutex */
        kthread_mutex_dealloc(ids[allocated--]);
    case 0: default:
        return -1;
    }
    return -1;
}

void* host_func() {

    return (void*)0;
}
void* salad_func() {return (void*)0;}
void* pasta_func() {return (void*)0;}
void* steak_func() {return (void*)0;}
void* eating_sudent_func() {return (void*)0;}
void print_stats() {}


int sim_init(int stud_initial, int stud_joining, int seats_num,
             int salad_buf_size, int pasta_buf_size,
             int steak_buf_size) {

    int allocated;
    int ids[8 + seats_num + 4];	/* 8 for mutexes and cvs 4
                                           for waiters and host. */
    void* stacks[seats_num + 4];
    int i, stud_to_be_seated, host_tid;

    allocated = 0;

    /* update static vars  */
    chair_num = seats_num;
    waiting_students = max((stud_initial - seats_num), 0) + stud_joining;
    if ((host_mutex = kthread_mutex_alloc()) < 0)
        return dealloc_ret(allocated, ids); /* could also be -1 */
    ids[++allocated] = host_mutex;

    if ((salad_mutex = kthread_mutex_alloc()) < 0)
        return dealloc_ret(allocated, ids);
    ids[++allocated] = salad_mutex;

    if ((pasta_mutex = kthread_mutex_alloc()) < 0)
        return dealloc_ret(allocated, ids);
    ids[++allocated] = pasta_mutex;

    if ((steak_mutex = kthread_mutex_alloc()) < 0)
        return dealloc_ret(allocated, ids);
    ids[++allocated] = steak_mutex;

    if ((host_cv = kthread_cond_alloc()) < 0)
        return dealloc_ret(allocated, ids);
    ids[++allocated] = host_cv;

    if ((salad_cv  = kthread_cond_alloc()) < 0)
        return dealloc_ret(allocated, ids);
    ids[++allocated] = salad_cv;

    if ((pasta_cv = kthread_cond_alloc()) < 0)
        return dealloc_ret(allocated, ids);
    ids[++allocated] = pasta_cv;

    if ((steak_cv = kthread_cond_alloc()) < 0)
        return dealloc_ret(allocated, ids);
    ids[++allocated] = steak_cv;

    i = 0;
    /* create the host */
    if ((stacks[i] = kalloc()) == 0)
        return dealloc_ret(allocated, ids);
    ids[++allocated] = (int)stacks[i];

    if ((host_tid = kthread_create(host_func, stacks[i++],
                                   MAX_STACK_SIZE)) < 0)
        return dealloc_ret(allocated, ids);

    /* create the waiters: */
    /* SALAD */
    if ((stacks[i] = kalloc()) == 0)
        return dealloc_ret(allocated, ids);
    ids[++allocated] = (int)stacks[i];

    if ((kthread_create(salad_func, stacks[i++], MAX_STACK_SIZE)) < 0)
        return dealloc_ret(allocated, ids);

    /* PASTA */
    if ((stacks[i] = kalloc()) == 0)
        return dealloc_ret(allocated, ids);
    ids[++allocated] = (int)stacks[i];

    if ((kthread_create(pasta_func, stacks[i++], MAX_STACK_SIZE)) < 0)
        return dealloc_ret(allocated, ids);

    /* STEAK */
    if ((stacks[i] = kalloc()) == 0)
        return dealloc_ret(allocated, ids);
    ids[++allocated] = (int)stacks[i];

    if ((kthread_create(steak_func, stacks[i++], MAX_STACK_SIZE)) < 0)
        return dealloc_ret(allocated, ids);

    /* the '+ 4' is for the waiters and the host */
    stud_to_be_seated = (min((seats_num), (stud_initial))) + 4;
    /* create the sitting students */
    while (i < stud_to_be_seated) {
        if ((stacks[i] = kalloc()) == 0)
            return dealloc_ret(allocated, ids);
        ids[++allocated] = (int)stacks[i];

        if ((kthread_create(eating_sudent_func, stacks[i++],
                            MAX_STACK_SIZE)) < 0)
            return dealloc_ret(allocated, ids);
    }

    if (kthread_join(host_tid) < 0)
        return dealloc_ret(allocated, ids);

    print_stats();
    return 0;
}
