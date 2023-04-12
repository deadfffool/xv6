#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path,char *target)
{
    char buf[512],*p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", target);
        return;
    }

    if(fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", target);
        close(fd);
        return;
    }

    switch(st.type)
    {
        case T_DEVICE:
        case T_FILE:
        {
            printf("find: the first arg must be the path of dir!\n");
            exit(1);
        }
        case T_DIR:
        {
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
            {
                printf("find: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf+strlen(buf);
            *p++ = '/';

            //不要在.或者..目录中递归
            while((read(fd,&de,sizeof(de)))==sizeof(de))
            {
                if(de.inum ==0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                    continue;
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;  //这段是用来干嘛的
                if(stat(buf, &st) < 0){
                    printf("find: cannot stat %s\n", buf);
                    continue;
                }
                if(st.type==T_FILE && strcmp(de.name,target)==0)
                    printf("%s\n",buf);
                if(st.type==T_DIR)
                    find(buf,target);
                } 
            break;
        }
    }
    close(fd);
    return ;
}

int main(int argv,char *argc[])
{
    if(argv!=3)
    {
        fprintf(2,"find: need 3 args!\n");
        exit(1);
    }

    find(argc[1],argc[2]);
    exit(0);
}