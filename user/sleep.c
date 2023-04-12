#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(int argc,char *argv[])
{
    if(argc!=2)
    {
        fprintf(2,"Usage: sleep + time");
        exit(1);
    }
    int sleeptime = atoi(argv[1]);
    if(sleeptime<=0)
    {
        fprintf(2,"Sleep time need to be positive!");
        exit(1);
    }
    sleep(sleeptime);
    exit(0);
    return 0;
}