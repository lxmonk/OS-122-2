/* void printf(int, char*, ...); */
/* int sim_init(int parameters[]); */
#include "types.h"
#include "user.h"


int main(int argc, char* argv[]) {

    int params[] = {1,0,4,4,4,4};

    kltsim(params);
    printf(2, "%d:DONE! uptime=%d\n", uptime(), uptime());
    sleep(10);
    printf(2, "%d:DONE! uptime=%d\n", uptime(), uptime());
    exit();
    return 0;
}
