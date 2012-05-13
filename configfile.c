#include "types.h"
#include "user.h"

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
