#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "uthread.h"

//#define SCHED_PB  change scheduling policies

#define T_A_DEBUG 0

#define ROUND_ROBIN 0
#define PRIORITY_BASED 1


int first_uthread = 1;		/* flag to make sure the ut_table is
                                   initialized only once.  */
uthread_table ut_table;

void
print_tinfo() {
    int i;
    uthread_t *cur;

    for (i = 0; i < MAX_UTHREADS; i++) {
        cur = ut_table.threads[i];
        if (cur != 0) {
            DEBUG_PRINT(4, "%d) tid=%d, ss_sp=%x, ss_esp=%x, priority=%d",
                        i, cur->tid, cur->ss_sp, cur->ss_esp,
                        cur->priority);
        }
    }
}

int uthread_create(void (*start_func)(), int priority){
    int ut_id;
    /* void* current_esp; */

    DEBUG_PRINT(4, "inside uthread_create", 999);
    if (first_uthread) {	/* initialize the uthreads table */
        DEBUG_PRINT(4, "inside 'first_uthread' loop", 999);
        ut_table.cur_threads = 0;
        ut_table.highest_p = 9;
        ut_table.running_tid = -1;
        memset(ut_table.threads, 0, sizeof(ut_table.threads));
        first_uthread = 0;
    }
    if (ut_table.cur_threads == MAX_UTHREADS) {
        return -1;
    }
    for (ut_id = 0; ut_id < MAX_UTHREADS; ut_id++) {
        /* if we've got to the end of the array, that means the last
           place is free, otherwise, we would have seen it's full with
           ut.cur_threads  */
        if (ut_table.threads[ut_id] == 0)
            break;		/* we'll put the new thread in ut_id' */
    }
    ut_table.threads[ut_id] = malloc(sizeof(uthread_t));
    if (ut_table.threads[ut_id] == 0) {
        /* not enough room on heap */
        return -1;
    }
    ut_table.threads[ut_id]->ss_sp = malloc(UTHREAD_STACK_SIZE);
    if (ut_table.threads[ut_id]->ss_sp == 0) { /* not enough room on heap */
        free(ut_table.threads[ut_id]);         /* need to free
                                                  previous malloc */
        return -1;
    }
    ut_table.threads[ut_id]->ss_esp = (ut_table.threads[ut_id]->ss_sp +
                                       (UTHREAD_STACK_SIZE - 4));
    ut_table.threads[ut_id]->ss_ebp = ut_table.threads[ut_id]->ss_esp;

    DEBUG_PRINT(4, "%d: ss_sp is %x," /* stack will be at %x" */, ut_id,
                ut_table.threads[ut_id]->ss_sp);
    /* mallocs were successful */

    ut_table.cur_threads++;
    if (ut_table.highest_p > priority) /* new top priority thread */
        ut_table.highest_p = priority;
    ut_table.threads[ut_id]->priority = priority;
    ut_table.threads[ut_id]->tid = ut_id;
    DEBUG_PRINT(4, "ut_id=%d, ut_table.threads[ut_id]=%x, "
                "ut_table.threads[ut_id]->tid=%d, "
                "&ut_table.threads[ut_id]->tid=%x",
                ut_id, ut_table.threads[ut_id],
                ut_table.threads[ut_id]->tid,
                &(ut_table.threads[ut_id]->tid));

    ut_table.threads[ut_id]->entryfunc = start_func;
    ut_table.threads[ut_id]->virgin = 1;
    /* if (T_A_DEBUG) { */
    /*     print_tinfo(); */
    /* } */
    /* STORE_ESP(current_esp); */
    /* create the initial stack for the new thread */
    /* LOAD_ESP(ut_table.threads[ut_id]->ss_esp); */

    /* PUSH(wrap_function); */

    /* PUSH(start_func); */
    /* PUSH(uthread_exit); */
    /* the 'return address' will be calling uthread_exit (after
       start_func is done). on later calls, this will be the next
       function to call. */

    /* PUSH(ut_table.threads[ut_id]->ss_sp); */
    /* update the ss_esp */
    /* ut_table.threads[ut_id]->ss_esp = ut_table.threads[ut_id]->ss_sp - 8; */
    /* LOAD_ESP(current_esp);	/\* restore the calling thread's stack *\/ */
    return ut_id;		/* these will be recycled */
}


uthread_t* next_thread(int start) {

    int sched;
#ifdef SCHED_PB
    sched = PRIORITY_BASED;
#else
    sched = ROUND_ROBIN;
#endif

    uthread_t *next;

    DEBUG_PRINT(4, "inside next", 900);
    for (;;start = (start + 1) % MAX_UTHREADS) {
        DEBUG_PRINT(4, "start=%d", start);
        next = ut_table.threads[start];
        DEBUG_PRINT(4, "next=%x, next->tid=%d", next, next->tid);
        if (next != 0) {
            if (sched == ROUND_ROBIN ||
                next->priority == ut_table.highest_p) {
                DEBUG_PRINT(4, "returning thread with tid=%d", next->tid);
                return next;
            }
        }
    }
}

uthread_t uthread_self() {
    DEBUG_PRINT(3, "returning ut_table.running_tid=%d", ut_table.running_tid);
    DEBUG_PRINT(3,"ut_table.threads[ut_table.running_tid]->tid=%d",ut_table.threads[ut_table.running_tid]->tid);
    DEBUG_PRINT(3,"ut_table.threads[1]->tid=%d",ut_table.threads[1]->tid);
    return *(ut_table.threads[ut_table.running_tid]);
}
static uthread_t *s_self;
static uthread_t *s_next;

void uthread_yield() {

    /* save_esp */
    DEBUG_PRINT(3, "inside uthread_yield", 178);
    s_self = ut_table.threads[uthread_self().tid];
    DEBUG_PRINT(3, "inside yield s_self->tid=%d", s_self->tid);
    /* DEBUG_PRINT(4, "self.tid=%d", self.tid); */
    DEBUG_PRINT(3, "BEFORE STORE_ESP/EBP: s_self->ss_esp=%x, "
                "s_self->ss_ebp=%x", s_self->ss_esp, s_self->ss_ebp);
    STORE_ESP(s_self->ss_esp);
    STORE_EBP(s_self->ss_ebp);

    DEBUG_PRINT(3, "AFTER STORE_ESP/EBP: s_self->ss_esp=%x, "
                "s_self->ss_ebp=%x", s_self->ss_esp, s_self->ss_ebp);
    /* correct uthread_t (self, next), old_epb, return address, (no
       function Args) */
    /* s_self->ss_esp += (8 + (2 * (sizeof(uthread_t)))); */
    s_next = next_thread(((s_self->tid) + 1) % MAX_UTHREADS);
    DEBUG_PRINT(3, "next->tid=%d", s_next->tid);
    ut_table.running_tid = s_next->tid;
    LOAD_ESP(s_next->ss_esp);
    LOAD_EBP(s_next->ss_ebp);
    DEBUG_PRINT(3, "s_next->ss_esp=%x, s_next->ss_ebp=%x",
                s_next->ss_esp, s_next->ss_ebp);
    if (s_next->virgin) {
        /* DEBUG_PRINT(3, "virgin thread\n", 111); */
        s_next->virgin = 0;
        PUSH(s_next->entryfunc);
        CALL(wrap_function);
    }
    /* return to the chosen uthread */
    return;
}

int get_highest_p() {
    int cur_max = 9;
    int cur_thread;
    for (cur_thread = 0; cur_thread < MAX_UTHREADS; cur_thread++) {
        if ((ut_table.threads[cur_thread] != 0 &&
             ut_table.threads[cur_thread]->priority < cur_max)) {
                cur_max = ut_table.threads[cur_thread]->priority;
        }
    }
    return cur_max;
}

static int s_self_tid;

void uthread_exit() {
    /* very similar to yield, but removing self from the threads table */

    /* s_self = uthread_self(); */
    s_self = ut_table.threads[uthread_self().tid];
    s_self_tid = s_self->tid;
    /* cleanup this thread */
    free(s_self->ss_sp);
    free(ut_table.threads[s_self_tid]);
    ut_table.threads[s_self->tid] = 0;
    ut_table.cur_threads--;
    ut_table.highest_p = get_highest_p(); /* now without this thread - who is highest? */

    if (ut_table.cur_threads == 0) { /* no more threads to run */
        exit();
    }

    /* ELSE, pass control to the next thread */

    s_next = next_thread((s_self_tid + 1) % MAX_UTHREADS);
    ut_table.running_tid = s_next->tid;

    LOAD_ESP(s_next->ss_esp);
    LOAD_EBP(s_next->ss_ebp);
    if (s_next->virgin) {
        s_next->virgin = 0;
        PUSH(s_next->entryfunc);
        CALL(wrap_function);
    }
    /* return to the chosen uthread */
    return;
}

int uthread_start_all() {
    DEBUG_PRINT(4, "inside.", 999);

    if (first_uthread)
        return -1;		/* uthread_create wasn't called at all */
    if (ut_table.cur_threads == 0)
        return -1;		/* no threads to run */
    if (ut_table.running_tid != -1)
        return -1;		/* uthread_start_all was already
                                   called before. */
    DEBUG_PRINT(4, "didn't fail - let's do it", 101);
    /* let's do it. */
    s_next = next_thread(0);	/* start with the 1st thread */
    /* STORE_ESP(esp); */
    /* DEBUG_PRINT(4, "got next thread. tid=%d ESP=%x ss_esp=%x", */
    /*             next->tid, esp, next->ss_esp); */
    ut_table.running_tid = s_next->tid;
    DEBUG_PRINT(4, "running thread updated", 78);
    LOAD_ESP(s_next->ss_esp);
    LOAD_EBP(s_next->ss_ebp);

    s_next->virgin = 0;
    PUSH(s_next->entryfunc);
    CALL(wrap_function);
    /* STORE_ESP(esp); */
    /* DEBUG_PRINT(4, "ESP loaded. ESP=%x ss_esp=%x", esp, next->ss_esp); */
    /* pass control to the chosen uthread */
    return 0;                   /* this should never be reached :) */
}

int uthread_setpr(int priority) {
    int old_priority;
    uthread_t *self;

    self = ut_table.threads[ut_table.running_tid];
    old_priority = self->priority;
    self->priority = priority;
    return old_priority;
}

int uthread_getpr() {
    return ut_table.threads[ut_table.running_tid]->priority;
}

void wrap_function(void (*entry)()) {
    DEBUG_PRINT(4, "wrap_function called", 999);
    entry();
    uthread_exit();
    /* implicitly return to the address stored on the stack.
       This makes it possible to use wrap_function for future calls,
       by pushing the next function to run as a function arg, and the
       caller as the return address.                                 */
}
