#include "uthread.h"
#include "types.h"
#include "user.h"

static int k_stat = 0;

void
thread_do() {
    int i, tid;

    printf(2, "inside thread\n");
    tid = uthread_self().tid;
    for (i=0; i<k_stat; i++) {
        printf(2, "thread %d iteration %d\n", tid, i);
        uthread_yield();
    }
    uthread_exit();
}

int
main(int argc, char** argv) {
    int n;
    int c;

    n = atoi(argv[1]);
    k_stat = atoi(argv[2]);
    printf(2, "n=%d k=%d\n", n, k_stat);

    for (c=0; c < n; c++) {
        uthread_create(thread_do, 0);
        printf(2, "creating thread %d\n", c);
    }
    printf(2, "calling uthread_start_all\n");
    uthread_start_all();
    printf(2, "this will not be printed\n");
    return 0;
}
