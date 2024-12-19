// xargs.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define BUF_SIZE 2048
#define ARGS_SIZE 128

// 带参数列表，执行某个程序
void run(char *program, char **args) {
    int pid = fork();
    if(pid < 0){
        fprintf(2, "xargs: fork failed\n");
        exit(1);
    }
    if(pid == 0) { // child exec
        exec(program, args);
        fprintf(2, "xargs: exec %s failed\n", program);
        exit(1);
    }
    return; // parent return
}

int main(int argc, char *argv[]){
    if(argc < 2){
        fprintf(2, "Usage: xargs program [initial-args...]\n");
        exit(1);
    }

    char buf[BUF_SIZE]; // 读入时使用的内存池
    char *p = buf, *last_p = buf; // 当前参数的结束、开始指针
    char *argsbuf[ARGS_SIZE]; // 全部参数列表，字符串指针数组，包含 argv 传进来的参数和 stdin 读入的参数
    char **args = argsbuf; // 指向 argsbuf 中第一个从 stdin 读入的参数

    // 将 argv 提供的参数加入到最终的参数列表中
    for(int i=1;i<argc;i++) {
        if (args - argsbuf >= ARGS_SIZE - 1) { // 留出一个位置给 NULL
            fprintf(2, "xargs: too many arguments\n");
            exit(1);
        }
        *args = argv[i];
        args++;
    }
    char **pa = args; // 开始读入参数

    int read_ret;
    while((read_ret = read(0, p, 1)) == 1) {//逻辑读入标注输入 存储在p 每次去得一个字符
        if(*p == ' ' || *p == '\n') {//空格和换行符
            char current_char = *p; // 保存当前字符
            *p = '\0'; // 将空格或换行符替换为 '\0'

            if (args - argsbuf >= ARGS_SIZE - 1) { // 留出一个位置给 NULL
                fprintf(2, "xargs: too many arguments\n");
                exit(1);
            }

            *(pa++) = last_p;
            last_p = p + 1;

            if(current_char == '\n') {
                *pa = 0; // 参数列表末尾用 null 标识列表结束
                run(argv[1], argsbuf); // 执行最后一行指令
                pa = args; // 重置读入参数指针，准备读入下一行
            }
        }
        p++;
        if (p - buf >= BUF_SIZE) {
            fprintf(2, "xargs: input line too long\n");
            exit(1);
        }
    }

    if(read_ret < 0){
        fprintf(2, "xargs: read error\n");
        exit(1);
    }

    if(pa != args) { // 如果最后一行不是空行
        *p = '\0';
        if (args - argsbuf >= ARGS_SIZE - 1) {
            fprintf(2, "xargs: too many arguments\n");
            exit(1);
        }
        *(pa++) = last_p;
        *pa = 0; // 参数列表末尾用 null 标识列表结束
        run(argv[1], argsbuf); // 执行最后一行指令
    }

    while(wait(0) != -1) {}; // 循环等待所有子进程完成，每一次 wait(0) 等待一个
    exit(0);
}
