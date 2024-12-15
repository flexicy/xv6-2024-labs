#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"

int main(int argc, char *argv[]){
    // 创建两个管道
    int pp2c[2];
    int pp2p[2];
    
    if (pipe(pp2c) < 0) {
        fprintf(2, "Pipe creation failed for pp2c\n");
        exit(1);
    }
    
    if (pipe(pp2p) < 0) {
        fprintf(2, "Pipe creation failed for pp2p\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "Fork failed\n");
        exit(1);
    }

    if (pid != 0) {
        // 父进程
        close(pp2c[0]); // 关闭读端
        close(pp2p[1]); // 关闭写端

        // 向子进程发送数据
        if (write(pp2c[1], ".", 1) != 1) {
            fprintf(2, "Parent write to child failed\n");
            exit(1);
        }
        close(pp2c[1]); // 关闭写端

        // 从子进程读取数据
        char buf;
        if (read(pp2p[0], &buf, 1) != 1) {
            fprintf(2, "Parent read from child failed\n");
            exit(1);
        }
        printf("%d: received pong\n", getpid());

        close(pp2p[0]); // 关闭读端
        wait(0);
    }
    else {
        // 子进程
        close(pp2c[1]); // 关闭写端
        close(pp2p[0]); // 关闭读端

        // 从父进程读取数据
        char buf;
        if (read(pp2c[0], &buf, 1) != 1) {
            fprintf(2, "Child read from parent failed\n");
            exit(1);
        }
        printf("%d: received ping\n", getpid());

        // 向父进程发送数据
        if (write(pp2p[1], &buf, 1) != 1) {
            fprintf(2, "Child write to parent failed\n");
            exit(1);
        }
        close(pp2p[1]); // 关闭写端
        close(pp2c[0]); // 关闭读端
        exit(0);
    }

    exit(0);
}
