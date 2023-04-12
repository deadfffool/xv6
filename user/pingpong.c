#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int p1[2],p2[2];
char message;
int main(int argc,char *argv[])
{
    if (argc != 2)
    {
        fprintf(2,"pingpong need an arg!\n");
        exit(1);
    }
    message = argv[1][0];
    if(pipe(p1)<0||pipe(p2)<0) fprintf(2,"pipe error!");
    if(fork()==0)
    {
        close(p1[1]);
        close(p2[0]);
        read(p1[0],&message,1);
        write(p2[1],&message,1);
        fprintf(1,"child %d:recevied ping %c\n",getpid(),message);
        close(p1[0]);
        close(p2[1]);
    }
    else
    {
        close(p1[0]);
        close(p2[1]);
        write(p1[1],&message,1);
        read(p2[0],&message,1);
        wait(0);
        fprintf(1,"father %d:recevied pong %c\n",getpid(),message);
        close(p1[1]);
        close(p2[0]);
    }
    exit(0);
}