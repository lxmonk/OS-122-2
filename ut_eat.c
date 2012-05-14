#include "types.h"
#include "user.h"
#include "uthread.h"



#define STUDENTS_INITIAL 0
#define STUDENTS_JOINING 1
#define NUM_OF_SEATS 2
#define SALAD_BUFFER_SIZE 3
#define PASTA_BUFFER_SIZE 4
#define STEAK_BUFFER_SIZE 5
#define SALAD 0
#define PASTA 1
#define STEAK 2

static int init[6]; //the init config values
static int food[3]; //the salad,pasta and steak buffers
static int empty_seats;
static int waiting_studens;


// gets the values from the config file
void getConfigValues(int val_array[6])
{
    int fd;
    char val[100];
    char *buf;
    int n,i,j;

    // printf(stdout, "open test\n");
    fd = open("ass2_conf.txt", 0);
    if(fd < 0){
        printf(2, "open ass2_conf.txt  failed!\n");
        exit();
    }
    n = read(fd,val,100);
    //    printf(2,"%s\n",val);
    i=0;
    buf=val;
    val_array[0]=atoi(buf);
    j=1;
    while (i < n){
        if (val[i] == '\n') {
            val_array[j]=atoi(buf+1);
            j++;
        }
        i++;
        buf++;
        if (j == 6)
            break;
    }

    close(fd);
}

// simulates a long eating
void long_eating_process() {
    int i,j,t,k;

    for(i=1;i < 100000;i++) {
         for(j=1; j <  1000;j++) {
             t = 1;
             for(k=1;k < 20;k++)
                 t *= k;
         }
    }
}

//simulates a short eating
void short_eating_process() {
    int i,j,t,k;

     for(i=1;i < 1000;i++) {
         for(j=1; j <  1000;j++) {
             t = 1;
             for(k=1;k < 20;k++)
                 t *= k;
         }
     }
}

//the waiter in-charge of the salad buffer
void salad_waiter_func()
{
    while (empty_seats  < init[STUDENTS_INITIAL]) {
        if (food[SALAD] < init[SALAD_BUFFER_SIZE])
            food[SALAD]++;
        uthread_yield();
    }
    uthread_exit();
}

//the waiter in-charge of the pasta buffer
void pasta_waiter_func()
{
    while (empty_seats  < init[STUDENTS_INITIAL]) {
        if (food[PASTA] < init[PASTA_BUFFER_SIZE])
            food[PASTA]++;
        uthread_yield();
    }
    uthread_exit();
}

//the waiter in-charge of the steak buffer
void steak_waiter_func()
{
    while (empty_seats  < init[STUDENTS_INITIAL]) {
        if (food[STEAK] < init[STEAK_BUFFER_SIZE])
            food[STEAK]++;
        uthread_yield();
    }
    uthread_exit();
}

//the eating student function
void student_func() {
    int id=uthread_self().tid;
    while (food[id % 3] == 0)
        uthread_yield();
    food[id % 3]--;
    long_eating_process();
    uthread_setpr(8); // update priority after eating
    while (food[(id+1) % 3] == 0)
        uthread_yield();
    food[(id+1) % 3]--;
    long_eating_process();
    uthread_setpr(7); // update priority after eating
    while (food[(id+2) % 3] == 0)
        uthread_yield();
    food[(id+2) % 3]--;
    short_eating_process();

    empty_seats++;
    uthread_exit();
}

//the host, adds waiting students to the table
void host_func() {
    while (empty_seats  < init[STUDENTS_INITIAL]) {
        if ((empty_seats > 0) && (waiting_studens > 0)) {
            empty_seats--;
            waiting_studens--;
            uthread_create(student_func,9);
        }
    }
    uthread_exit();
}
int main() {
    int i;

    getConfigValues(init);
    food[SALAD]=init[SALAD_BUFFER_SIZE];
    food[PASTA]=init[PASTA_BUFFER_SIZE];
    food[STEAK]=init[STEAK_BUFFER_SIZE];
    empty_seats = 0;
    waiting_studens = init[STUDENTS_JOINING];

    for(i = 0; i < init[STUDENTS_INITIAL];i++) {
        uthread_create(student_func,9);
    }
    uthread_create(host_func,5);
    uthread_create(salad_waiter_func,5);
    uthread_create(pasta_waiter_func,5);
    uthread_create(steak_waiter_func,5);

    uthread_start_all();
    return 0;
}
