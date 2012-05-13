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
    char *buf;
    char val[10];
    int n,i;

    // printf(stdout, "open test\n");
    fd = open("ass2_conf.txt", 0);
    if(fd < 0){
        printf(stdout, "open ass2_conf.txt  failed!\n");
        exit();
    }
    i=0;
    while((n = read(fd,buf,1)) > 0){
        val[i]=*buf;
        i++;
        if (*buf != '\n')
            break;
    }



    close(fd);
}
