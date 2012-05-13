#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"


int*
getConfigValues(void)
{
    int fd;
    int *val_array = malloc(sizeof(int)*6);
    char val[10];
    int n;

    // printf(stdout, "open test\n");
    fd = open("ass2_conf.txt", 0);
    if(fd < 0){
        printf(2, "open ass2_conf.txt  failed!\n");
        exit();
    }
    for(n=0; n < 6;n++) {
        memset(val,0,10);
        read(fd,val,10);
        val_array[n]=atoi(val);

        printf(2,"s=%s    d=%d\n",val_array[n],val);
    }
    close(fd);
    return val_array;
}


int main() {
    int *a;

    a=getConfigValues();
    printf(2,"%d %d %d %d %d %d",a[0],a[1],a[2],a[3],a[4],a[5]);

    return 0;
}
