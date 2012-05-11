
/********************************
        Majors which inline assembly
 ********************************/

// Saves the value of esp to var
#define STORE_ESP(var)          asm("movl %%esp, %0;" : "=r" ( var ))

// Loads the contents of var into esp
#define LOAD_ESP(var)   asm("movl %0, %%esp;" : : "r" ( var ))

// Calls the function func
#define CALL(addr)		asm("call *%0;" : : "r" ( addr ))

// Pushes the contents of var to the stack
#define PUSH(var)		asm("movl %0, %%edi; push %%edi;" : : "r" ( var ))


#define MAX_UTHREADS 64
#define UTHREAD_STACK_SIZE 4000

// Represents a ULT.
// Feel free to extend this definition as needed.
typedef struct
{
    int tid;		// A unique thread ID within the process
    void *ss_sp;	// Stack base or pointer
    void *ss_esp;	// Stack size
    int priority;	// The priority of the thread 0...9 (0 is highest)
} uthread_t;

typedef struct
{
    int cur_threads;
    int highest_p;
    uthread_t *threads[MAX_UTHREADS];
    int running_tid;
} uthread_table;

/********************************
        The API of the ULT package
 ********************************/

int uthread_create(void (*start_func)(), int priority);
void uthread_yield();
void uthread_exit();
int uthread_start_all();
int uthread_setpr(int priority);
int uthread_getpr();
uthread_t uthread_self();

// A function that wraps the entry functipn of a thread.
// This is just a suggestion, feel free to modify it as needed.
void wrap_function(void (*entry)());
