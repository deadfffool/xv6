#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#define N 35;

void solve(int *pl)
{
    close(pl[1]);
    int t,pr[2],prime;
    //防止无效递归
    if(read(pl[0],&prime,sizeof prime)==0)
    {
        close(pl[0]);
        return;
    }
    pipe(pr);
    if(fork() == 0)
    {
        close(pl[0]);
        solve(pr);
    }
    else
    {
        close(pr[0]);
        for(t=-1;read(pl[0],&t,sizeof t);)
            if(t%prime!=0)
                write(pr[1],&t,sizeof t);
        close(pr[1]);
        close(pl[0]);
        wait(0);
        fprintf(1,"prime : %d\n",prime);
        exit(0);
    }
    exit(0);
}

int main(int argc,char *argv[])
{
    if(argc!=1)
    {
        fprintf(2,"prime does not need arg!\n");
        exit(1);
    }

    int p[2];
    pipe(p);
    if(fork() == 0)
        solve(p);
    else
    {
        close(p[0]);
        for(int seq=3;seq<=35;seq+=2)
            write(p[1],&seq,sizeof seq);
        close(p[1]);
        wait(0);
        fprintf(1,"prime : 2\n");
        exit(0);
    }        
    exit(0);
}