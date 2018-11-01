#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <dirent.h>

#include <unistd.h>
#include <sys/stat.h>

extern int optind;

// 存储文件信息结构体
struct fileinfo {
    char *name;
    mode_t mode;

};

// 与选项相关的全局变量
int flag_long = 0;  // -l
int flag_all  = 0;  // -a
int flag_almost = 0;// -A

// 分析选项
void parse_opt()
{
    return;
}

// 打印
void display(struct fileinfo *, int, int);

void do_ls(char *pathname)
{
    DIR *dp;
    struct dirent *direntp;
    struct fileinfo fileinfo[1000];

    // 读取目录
    // 对于非目录,直接列出该文件

    // 对于目录，列出目录下文件
    int n = 0;
    dp = opendir(pathname);
    while (direntp = readdir(dp)) {
        // 处理-a,-A
        if (flag_all) {
            ;// do nothing
        }
        else if (flag_almost) {
            if (!strcmp(direntp->d_name, ".") || !strcmp(direntp->d_name, ".."))
                continue;
        }
        else {
            if(direntp->d_name[0] == '.')
                continue;
        }

        fileinfo[n++].name = direntp->d_name;

        // 处理-l
        if (flag_long) {

        }
    }

    /*
    for (int i = 0; i < n; i++) {
        fprintf(stdout, "%s\n", namebuf[i]);
    }
    */
}

int main(int argc, char *argv[])
{
// 分析选项
    parse_opt(argc, argv);
//
    if (optind >= argc)
        do_ls(".");
    else
        while (optind < argc)
            do_ls(argv[optind++]);

    return 0;
}
