#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "uthread.h"

int first_uthread = 1;
uthread_table ut_table;


int uthread_create(void (*start_func)(), int priority){
    int ut_id;
    void* current_esp;

    if (first_uthread) {	/* initialize the uthreads table */
        ut_table.cur_threads = 0;
        ut_table.highest_p = 9;
        memset(ut_table.threads, 0, sizeof(ut_table.threads));
        first_uthread = 0;
    }
    if (ut_table.cur_threads == MAX_UTHREADS) {
        return -1;
    }
    for (ut_id = 0; ut_id < MAX_UTHREADS; ut_id++) {
        /* if we've got to the end of the array, that the last place
           is free, otherwise, we would have seen it's full with
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
    /* mallocs were successful */

    ut_table.cur_threads++;
    if (ut_table.highest_p > priority) /* new top priority thread */
        ut_table.highest_p = priority;
    ut_table.threads[ut_id]->priority = priority;
    ut_table.threads[ut_id]->ss_size = 0;
    ut_table.threads[ut_id]->tid = ut_id;
    STORE_ESP(current_esp);
    /* create the initial stack for the new thread */
    LOAD_ESP(ut_table.threads[ut_id]->ss_sp);
    PUSH(start_func);
    PUSH(uthread_exit);		/* the 'return address' will be
                                   calling uthread_exit (after
                                   start_func is done). on later
                                   calls, this will be the next
                                   function to call. */
    PUSH(ut_table.threads[ut_id]->ss_sp);
    /* update the ss_size */
    ut_table.threads[ut_id]->ss_size = 12;



    return ut_id;		/* these will be recycled */
}


void uthread_yield() {

}
void uthread_exit(){}
int uthread_start_all() {return 0;}
int uthread_setpr(int priority) {return 0;}
int uthread_getpr() {return -1;}
uthread_t uthread_self();


void wrap_function(void (*entry)()) {
    entry();
    /* implicitly return to the address stored on the stack.
       This makes it possible to use wrap_function for future calls,
       by pushing the next function to run as a function arg, and the
       caller as the return address.                                 */

}
