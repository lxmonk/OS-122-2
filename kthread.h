#dfeine MAX_STACK_SIZE 4000
#define MAX_MUTEXES 64
#define MAX_CONDS 64

/********************************
	The API of the KLT package
 ********************************/

int kthread_create( void*(*start_func)(), void* stack, unit stack_size ); 
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