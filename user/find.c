// find.c
#include "kernel/types.h"    // 引入基本数据类型定义
#include "kernel/stat.h"     // 引入文件状态结构体定义
#include "user/user.h"       // 引入用户空间的系统调用和函数声明
#include "kernel/fs.h"       // 引入文件系统相关结构体和常量定义
void find(char *path,char *target){//定义一个递归查找函数，接受两个参数：起始路径和目标文件名
    char buf[512],*p;
    int fd;
    struct dirent de;//目录项结构体，用于存储从目录中读取的每一个条目。
    struct stat st;//文件状态结构体，用于存储文件或目录的元数据

    if((fd = open(path,0)) <0){
          fprintf(2, "find: cannot open %s\n", path);
            return;//使用 open 函数以只读模式 (0 表示只读) 打开指定的路径 path。
    }
    //获取文件状态
    if(fstat(fd, &st) < 0){//打开对应文件描述符文件并且存储在st
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
    }


    switch(st.type){
    case T_FILE:
    // 文件处理逻辑 用字符串首地质看最后文件和寻找的文件是否是同一个名称
    if(strcmp(path +strlen(path)-strlen(target),target)==0)
    {
        printf("%s\n", path);  // 如果匹配，打印文件的完整路径
    }
    break;
    case T_DIR://dirsiz表示最大目录文件大小
    if(strlen(path) +1 +DIRSIZ +1 >sizeof(buf)){
        printf("find: path too long\n");  // 如果超出，输出错误信息
            break;                             // 结束当前case
    }
    // 目录处理逻辑
    strcpy(buf,path);//回溯
    p = buf +strlen(buf);
    *p++ = '/';                 // 在buf末尾添加一个斜杠，准备拼接子目录或文件名

            // 循环读取目录中的每一个目录项
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0)continue;  // 如果目录项的inode号为0，表示无效，跳过

            memmove(p, de.name, DIRSIZ);  // 将目录项的名称复制到buf的当前指针位置
            p[DIRSIZ] = 0;                // 在复制的名称后添加字符串终止符
            //p[DIRSIZ] 等同于 *(p + DIRSIZ)

            // 获取新路径的文件状态信息
            if(stat(buf, &st) < 0){//系统调用 获得文件目录状态
                printf("find: cannot stat %s\n", buf);  // 如果获取失败，输出错误信息
                continue;                                 // 跳过当前目录项，继续下一个
        }
         // 跳过当前目录和父目录，避免无限递归
            if(strcmp(buf + strlen(buf) - 2, "/.") != 0 && strcmp(buf + strlen(buf) - 3, "/..") != 0) {
                find(buf, target);  // 递归调用find函数，继续查找
            }
    }
    break;  // 结束当前case
    }
    close(fd);  // 关闭文件描述符，释放资源
}

// 程序的主入口函数
int main(int argc, char *argv[])
{
    // 检查命令行参数的数量是否少于3个
    if(argc < 3){
        exit(0);  // 如果参数不足，程序退出
    }

    char target[512];        // 定义一个字符数组用于存储目标文件名
    target[0] = '/';         // 在目标文件名前添加一个斜杠
    strcpy(target + 1, argv[2]);  // 将命令行的第三个参数（目标文件名）复制到target中，紧跟斜杠之后

    find(argv[1], target);   // 调用find函数，传入起始路径和目标文件名
    exit(0);                 // 程序正常退出
}
