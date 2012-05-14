#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

#include "kthread.h"

#define MAX_STUDS 2000
#define LONG_EAT 100000
#define SHORT_EAT 1000

#define SALAD 0
#define PASTA 1
#define STEAK 2

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))


static int host_mutex;
static int host_cv, salad_cv, pasta_cv, steak_cv;
static int chair_num, seating_students, waiting_students;
static int salad_buf_size, pasta_buf_size, steak_buf_size;
static int allocated, i, salad_waiter, pasta_waiter, steak_waiter;
static void* stacks[MAX_STUDS];
/* 8 for mutexes and cvs 4 for waiters and host. */
static int alloced_elts[8 + 4 + MAX_STUDS];
static volatile int sim_on;
static int dishes[3], cvs[3];
static int foods[3];
int dealloc_ret(int allocated, int alloced_elts[]) {

    while(allocated > 12)    /* loop over all created students */
        kfree((void*)alloced_elts[allocated--]);

    switch(allocated) {	/* no 'break;;' on purpose! */
    case 12:			/* steak waiter stack */
        kfree((void*)alloced_elts[allocated--]);
    case 11:			/* pasta waiter stack */
        kfree((void*)alloced_elts[allocated--]);
    case 10:			/* salad waiter stack */
        kfree((void*)alloced_elts[allocated--]);
    case 9:			/* host stack */
        kfree((void*)alloced_elts[allocated--]);
    case 8:			/* steak_cv */
        kthread_cond_dealloc(alloced_elts[allocated--]);
    case 7:			/* pasta_cv */
        kthread_cond_dealloc(alloced_elts[allocated--]);
    case 6:			/* salad_cv */
        kthread_cond_dealloc(alloced_elts[allocated--]);
    case 5:			/* host_cv */
        kthread_cond_dealloc(alloced_elts[allocated--]);
    case 4:			/* steak_mutex */
        kthread_mutex_dealloc(alloced_elts[allocated--]);
    case 3:			/* pasta_mutex */
        kthread_mutex_dealloc(alloced_elts[allocated--]);
    case 2:			/* salad_mutex */
        kthread_mutex_dealloc(alloced_elts[allocated--]);
    case 1:			/* host_mutex */
        kthread_mutex_dealloc(alloced_elts[allocated--]);
    case 0: default:
        return -1;
    }
    return -1;
}

void* eating_sudent_func();

void sit_student() {
    /* host_mutex is already taken */
    void* stack;

    if ((stack = kalloc()) == 0)
        return;
    alloced_elts[++allocated] = (int)stacks[i];

    if ((kthread_create(eating_sudent_func, stacks[i++],
                        MAX_STACK_SIZE)) < 0) {
        i--;
        return kfree((void*)alloced_elts[allocated--]);
    }
    waiting_students--;
    seating_students++;
}
void* host_func() {
    while(waiting_students > 0) {
        kthread_mutex_lock(host_mutex);
        if (seating_students < chair_num)
            sit_student();      /* might fail with no side effects */
        kthread_mutex_unlock(host_mutex);
    }

    while (1) {
        kthread_mutex_lock(host_mutex);
        if (seating_students == 0)
            break;
        kthread_mutex_unlock(host_mutex);
    }
    sim_on = 0;
    kthread_join(salad_waiter);
    kthread_join(pasta_waiter);
    kthread_join(steak_waiter);
    dealloc_ret(allocated, alloced_elts); /* free resources. */
    kthread_exit();
    return (void*)0;		/* never reached.. */
}
void* salad_func() {
    while (sim_on != 0){
        kthread_mutex_lock(dishes[SALAD]);
        if (foods[SALAD] < salad_buf_size) {
            foods[SALAD]++;
        }
        kthread_mutex_unlock(dishes[SALAD]);
    }
    kthread_exit();
    return (void*)0;		/* never reached.. */
}
void* pasta_func() {
    while (sim_on != 0){
        kthread_mutex_lock(dishes[PASTA]);
        if (foods[PASTA]< pasta_buf_size) {
            foods[PASTA]++;
        }
        kthread_mutex_unlock(dishes[PASTA]);
    }
    kthread_exit();
    return (void*)0;		/* never reached */
}

void* steak_func() {
    while (sim_on != 0){
        kthread_mutex_lock(dishes[STEAK]);
        if (foods[STEAK] < steak_buf_size) {
            foods[STEAK]++;
        }
        kthread_mutex_unlock(dishes[PASTA]);
    }
    kthread_exit();
    return (void*)0;		/* never reached */
}

void eat_proc(int iter_num) {
    int i, j, k, t;

    for(i = 1; i <= iter_num; i++) {
        for (j=1; j <= 1000; j++) {
            t=1;
            for (k=1; k <= 20; k++)
                t *= k;
        }
    }
}

void update_stats() {}


void* eating_sudent_func() {
    int stud_id;
    int dish;

    stud_id = kthread_id();
    dish = stud_id % 3;
    kthread_mutex_lock(dishes[dish]);
    /* get 1st dish */
 d1:
    if (foods[dish] > 0)
        foods[dish]--;
    else {
        kthread_cond_wait(cvs[dish], dishes[dish]);
        goto d1;
    }
    kthread_mutex_unlock(dishes[dish]);

    /* get 2nd dish */
    dish = (stud_id + 1) % 3;
    kthread_mutex_lock(dishes[dish]);
 d2:
    if (foods[dish] > 0)
        foods[dish]--;
    else {
        kthread_cond_wait(cvs[dish], dishes[dish]);
        goto d2;
    }
    kthread_mutex_unlock(dishes[dish]);

    eat_proc(LONG_EAT);

    /* get 3rd dish */
    dish = (stud_id + 2) % 3;
    kthread_mutex_lock(dishes[dish]);
 d3:
    if (foods[dish] > 0)
        foods[dish]--;
    else {
        kthread_cond_wait(cvs[dish], dishes[dish]);
        goto d3;
    }
    kthread_mutex_unlock(dishes[dish]);
    eat_proc(SHORT_EAT);

    kthread_mutex_lock(host_mutex);
    seating_students--;		/* get up and leave */
    kthread_mutex_unlock(host_mutex);

    update_stats();
    kthread_exit();
    return (void*)0;
}

void print_stats() {}


int sim_init(int stud_initial, int stud_joining, int seats_num,
             int _salad_buf_size, int _pasta_buf_size,
             int _steak_buf_size) {

    void* stacks[seats_num + 4];
    int stud_to_be_seated, host_tid;
    int salad_mutex, pasta_mutex, steak_mutex;


    /* update static vars  */
    chair_num = seats_num;
    seating_students = 0;
    waiting_students = max((stud_initial - seats_num), 0) + stud_joining;
    allocated = 0;
    salad_buf_size = _salad_buf_size;
    pasta_buf_size = _pasta_buf_size;
    steak_buf_size = _steak_buf_size;
    foods[SALAD] = salad_buf_size;
    foods[PASTA] = pasta_buf_size;
    foods[STEAK] = steak_buf_size;


    if ((host_mutex = kthread_mutex_alloc()) < 0)
        return dealloc_ret(allocated, alloced_elts); /* could also be -1 */
    alloced_elts[++allocated] = host_mutex;

    if ((salad_mutex = kthread_mutex_alloc()) < 0)
        return dealloc_ret(allocated, alloced_elts);
    alloced_elts[++allocated] = salad_mutex;
    dishes[SALAD] = salad_mutex;

    if ((pasta_mutex = kthread_mutex_alloc()) < 0)
        return dealloc_ret(allocated, alloced_elts);
    alloced_elts[++allocated] = pasta_mutex;
    dishes[PASTA] = pasta_mutex;

    if ((steak_mutex = kthread_mutex_alloc()) < 0)
        return dealloc_ret(allocated, alloced_elts);
    alloced_elts[++allocated] = steak_mutex;
    dishes[STEAK] = steak_mutex;

    if ((host_cv = kthread_cond_alloc()) < 0)
        return dealloc_ret(allocated, alloced_elts);
    alloced_elts[++allocated] = host_cv;

    if ((cvs[SALAD]  = kthread_cond_alloc()) < 0)
        return dealloc_ret(allocated, alloced_elts);
    alloced_elts[++allocated] = salad_cv;

    if ((cvs[PASTA] = kthread_cond_alloc()) < 0)
        return dealloc_ret(allocated, alloced_elts);
    alloced_elts[++allocated] = pasta_cv;

    if ((cvs[STEAK] = kthread_cond_alloc()) < 0)
        return dealloc_ret(allocated, alloced_elts);
    alloced_elts[++allocated] = steak_cv;

    i = 0;
    /* create the host */
    if ((stacks[i] = kalloc()) == 0)
        return dealloc_ret(allocated, alloced_elts);
    alloced_elts[++allocated] = (int)stacks[i];

    if ((host_tid = kthread_create(host_func, stacks[i++],
                                   MAX_STACK_SIZE)) < 0)
        return dealloc_ret(allocated, alloced_elts);

    /* create the waiters: */
    /* SALAD */
    if ((stacks[i] = kalloc()) == 0)
        return dealloc_ret(allocated, alloced_elts);
    alloced_elts[++allocated] = (int)stacks[i];

    if ((salad_waiter = kthread_create(salad_func, stacks[i++],
                                       MAX_STACK_SIZE)) < 0)
        return dealloc_ret(allocated, alloced_elts);

    /* PASTA */
    if ((stacks[i] = kalloc()) == 0)
        return dealloc_ret(allocated, alloced_elts);
    alloced_elts[++allocated] = (int)stacks[i];

    if ((pasta_waiter = kthread_create(pasta_func, stacks[i++],
                                       MAX_STACK_SIZE)) < 0)
        return dealloc_ret(allocated, alloced_elts);

    /* STEAK */
    if ((stacks[i] = kalloc()) == 0)
        return dealloc_ret(allocated, alloced_elts);
    alloced_elts[++allocated] = (int)stacks[i];

    if ((steak_waiter = kthread_create(steak_func, stacks[i++],
                                       MAX_STACK_SIZE)) < 0)
        return dealloc_ret(allocated, alloced_elts);

    /* the '+ 4' is for the waiters and the host */
    stud_to_be_seated = (min((seats_num), (stud_initial))) + 4;
    /* create the sitting students */
    while (i < stud_to_be_seated) {
        if ((stacks[i] = kalloc()) == 0)
            return dealloc_ret(allocated, alloced_elts);
        alloced_elts[++allocated] = (int)stacks[i];

        if ((kthread_create(eating_sudent_func, stacks[i++],
                            MAX_STACK_SIZE)) < 0)
            return dealloc_ret(allocated, alloced_elts);
        seating_students++;
    }

    sim_on = 1;

    if (kthread_join(host_tid) < 0)
        return dealloc_ret(allocated, alloced_elts);

    print_stats();
    kthread_exit();
    return 0;			/* never reached. */
}
