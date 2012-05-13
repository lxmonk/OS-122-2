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


static int host_mutex, salad_mutex, pasta_mutex, steak_mutex;
static int host_cv, salad_cv, pasta_cv, steak_cv;

int
dealloc_ret(int allocated, int ids[]) {

switch(allocated) {		/* not 'break;;' on purpose! */
 case 8:
kfree(ids[allocated--]);
    case 7:
        kthread_cond_dealloc(ids[allocated--]);
    case 6:
        kthread_cond_dealloc(ids[allocated--]);
    case 5:
        kthread_cond_dealloc(ids[allocated--]);
    case 4:
        kthread_mutex_dealloc(ids[allocated--]);
    case 3:
        kthread_mutex_dealloc(ids[allocated--]);
    case 2:
        kthread_mutex_dealloc(ids[allocated--]);
    case 1:
        kthread_mutex_dealloc(ids[allocated--]);
    case 0: default:
        return -1;
    }
return -1;
}

int
sim_init(int stud_initial, int stud_joining, int seats_num,
         int salad_buf_size, int pasta_buf_size,
         int steak_buf_size) {

    int allocated = 0;
    int ids[8 + seats_num + 4];
    void* stacks[seats_num + 4];

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

/* create the host */
if ((stacks[0] = kalloc()) == 0)
    return dealloc_ret(allocated, ids);
ids[++allocated] = stacks[0];

if ((kthread_create(host_func, stacks[0], MAX_STACK_SIZE)) < 0)
    return dealloc_ret(allocated, ids);

/* create the waiters: */
if ((stacks[1] = kalloc()) == 0)
    return dealloc_ret(allocated, ids);
ids[++allocated] = stacks[1];

if ((kthread_create(host_func, stacks[0], MAX_STACK_SIZE)) < 0)
    return dealloc_ret(allocated, ids);

    return 0;
    }
