#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

//递归程序
void sieve(int pleft[2]){
    int p;
    read(pleft[0],&p,sizeof(p)); // 从左邻居的管道读取一个整数
    if(p ==-1){
        exit(0);
    }
    printf("prime %d\n",p); //输出数据
    int pright[2];
    pipe(pright);

    //创建子进程
    if(fork() == 0){
        //关闭多与的管道
        close(pright[1]);
        close(pleft[0]);
        sieve(pright);
    }
    else{
        //父进程关闭 就是现在这个进程关闭不需要的管道端口
        close(pright[0]);//不要读取了
        int buf;
        while(read(pleft[0],&buf,sizeof(buf)) && buf !=-1){
            if(buf %p != 0){//不是能除开的就是质数
            write(pright[1],&buf,sizeof(buf));
            }
        }
        buf =-1;//这个时候退出循环表示已经受到了左边的-1 给右边传
        write(pright[1],&buf,sizeof(buf));
        wait(0);//等待右侧进程结束
        exit(0);//退出这个进程
    }
}
int main(int argc,char **argv){
    int input_pipe[2];
    pipe(input_pipe);
    if(fork()==0){
        close(input_pipe[1]);//不用写入
        sieve(input_pipe);//进入筛选
        exit(0);
    }
    else{
        close(input_pipe[0]);
        int i;
        for(i =2;i<35;i++){
            write(input_pipe[1],&i,sizeof(i));
        }
        i =-1;//结束最后传入-1
        write(input_pipe[1],&i,sizeof(i));
    }
    wait(0);
    exit(0);
}