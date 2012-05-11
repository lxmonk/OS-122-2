#include "uthread.h"
#include "types.h"
#include "user.h"

#define T_A_DEBUG 1

static int k_stat = 0;

void
thread_do() {
    int i, tid;

    tid = uthread_self().tid;
    DEBUG_PRINT("inside thread %d", tid);
    for (i=0; i<k_stat; i++) {
        printf(2, "thread %d iteration %d\n", uthread_self().tid, i);
        uthread_yield();
    }
    uthread_exit();
}

int
main(int argc, char** argv) {
    int n;
    int c;
    int ret, esp;

    n = atoi(argv[1]);
    k_stat = atoi(argv[2]);

    DEBUG_PRINT("n=%d k=%d", n, k_stat);

    for (c=0; c < n; c++) {
        ret = uthread_create(thread_do, 0);
        DEBUG_PRINT("creating thread %d. ret=%d", c, ret);
    }
    STORE_ESP(esp);
    DEBUG_PRINT("Before 'uthread_start_all' ESP=%x", esp);
    DEBUG_PRINT("calling uthread_start_all", 999);
    uthread_start_all();
    STORE_ESP(esp);
    DEBUG_PRINT("After 'uthread_start_all' ESP=%x", esp);
    printf(2, "this will not be printed\n");
    return 0;
}
