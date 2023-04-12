/*tips:
Linux的xargs命令用于从标准输入读取参数并将其作为命令行参数传递给指定命令。它可以帮助用户处理大量的输入参数，尤其是当这些参数包含空格或换行符时。
xargs命令的基本语法如下：xargs [options] command
其中，command是要执行的命令，options是一些选项。xargs命令会读取标准输入中的参数，将其转换为命令行参数并执行command命令。默认情况下，xargs命令使用空格作为分隔符来分隔参数，如果要使用其他分隔符，可以使用-d选项指定分隔符。
例如，如果要在当前目录下查找所有扩展名为.txt的文件，可以使用find命令找到所有文件，并将它们的名称传递给xargs命令，再将xargs的输出传递给grep命令进行过滤：find . -name "*.txt" | xargs grep "pattern"
这个命令会将所有扩展名为.txt的文件的名称传递给grep命令，然后grep命令会在这些文件中搜索包含"pattern"的行。
*/
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

int main(int argc, char *argv[])
{
        char buf[64];
        char *arg[MAXARG];
        int i;
        if(argc < 2){
           fprintf(2, "Usage: xargs command\n");
           exit(1);
        }
        for(int i = 1;i < argc; i++)
                arg[i - 1] = argv[i];
        arg[argc] = 0;
        while(1)
        {
                i = 0;
                while(1)
                {
                        int len;
                        len = read(0, &buf[i], 1);
                        if(len <= 0 || buf[i] == '\n')break;
                        i++;
                }
                if(i == 0)
                {
                        break;
                }
                buf[i] = 0;
                arg[argc - 1] = buf;//这里就相当于在echo line后面加上1或2
                if(fork() == 0)
                {
                        exec(arg[0],arg);
                        exit(1);
                }
                else    wait(0);
        }
        exit(0);
}
