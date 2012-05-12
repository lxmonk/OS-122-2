#define MAX_STACK_SIZE 4000
#define MAX_MUTEXES 64
#define MAX_CONDS 64


typedef struct {
    struct spinlock lock;
    void *waiting_kthreads[NPROC];
    int first;
    int count;
} kthread_mutex_t;

static struct spinlock array_lock;
static kthread_mutex_t *mutex_p_array[MAX_MUTEXES];
static volatile int first_mutex = 1;



int kthread_id();
void kthread_exit();
int kthread_join( int thread_id );

int kthread_mutex_alloc();
int kthread_mutex_dealloc( int mutex_id );
int kthread_mutex_lock( int mutex_id );
int kthread_mutex_unlock( int mutex_id );

int kthread_cond_alloc();
int kthread_cond_dealloc( int cond_id );
int kthread_cond_wait( int cond_id, int mutex_id );
int kthread_cond_signal( int cond_id );
