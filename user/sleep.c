#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        // 检查是否提供了正确的参数
        fprintf(2, "usage: sleep <time>\n");
        exit(1);
    }

    // 使用 atoi 将字符串转换为整数（秒）
    sleep(atoi(argv[1]));
    exit(0);
}
