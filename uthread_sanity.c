#include "uthread.h"
#include "types.h"
#include "user.h"

#define T_A_DEBUG 0

static int k_stat = 0;

void
thread_do() {
    int s_i, s_tid;
    s_tid = uthread_self().tid;
    for (s_i=0; s_i<k_stat; s_i++) {
        DEBUG_PRINT(3, "inside thread %d", s_tid);
        printf(2, "thread %d iteration %d\n", uthread_self().tid, s_i);
        printf(2, "priority %d\n", uthread_self().priority);
        uthread_yield();
    }
    DEBUG_PRINT(3, "thread tid=%d exiting. s_i=%d", uthread_self().tid,
                s_i);
    uthread_exit();
}

void
show_esp() {
    int esp;
    STORE_ESP(esp);
    DEBUG_PRINT(4, "ESP=%x", esp);
}
int
main(int argc, char** argv) {
    int n;
    int c;
    int ret, esp;

    n = atoi(argv[1]);
    k_stat = atoi(argv[2]);

    DEBUG_PRINT(4, "n=%d k=%d", n, k_stat);

    for (c=0; c < n; c++) {
        ret = uthread_create(thread_do, (c % 2));
        DEBUG_PRINT(4, "creating thread %d. ret=%d", c, ret);
    }
    STORE_ESP(esp);
    DEBUG_PRINT(4, "Before 'uthread_start_all' ESP=%x", esp);
    show_esp();
    DEBUG_PRINT(4, "calling uthread_start_all", 999);
    uthread_start_all();
    STORE_ESP(esp);
    DEBUG_PRINT(4, "After 'uthread_start_all' ESP=%x", esp);
    printf(2, "this will not be printed\n");
    return 0;
}
