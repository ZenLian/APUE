#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <dirent.h>

#include <unistd.h>
#include <sys/stat.h>

/*
 * TODO:
 * [x] parse options
 * [ ] map user id to user name
 * [ ] list time
 * [ ] symbol link -> dest file
 * [ ] sort by name
 * [ ] handle [-r] option to list recursively
 */

// 存储文件信息结构体
struct fileinfo {
    char *name;
    char type;
    char access[10];
    nlink_t nlink;
    uid_t uid;
    gid_t gid;
    dev_t rdev;
    off_t size;
    blksize_t blksize;
    blkcnt_t  blkcnt;
};

// 与选项相关的全局变量
int flag_long = 0;  // -l
int flag_all  = 0;  // -a
int flag_almost = 0;// -A

// 分析选项
void parse_opt(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "laA")) != -1) {
        switch (opt) {
            case 'l':
                flag_long = 1;
                break;
            case 'a':
                flag_all = 1;
                break;
            case 'A':
                flag_almost = 1;
                break;
            default:
                break;
        }
    }
    return;
}

void mod2type(mode_t mode, char *type)
{
    if (S_ISREG(mode))
        *(type) = '-';
    else if (S_ISDIR(mode))
        *(type) = 'd';
    else if (S_ISCHR(mode))
        *(type) = 'c';
    else if (S_ISBLK(mode))
        *(type) = 'b';
    else if (S_ISFIFO(mode))
        *(type) = 'p';
    else if (S_ISLNK(mode))
        *(type) = 'l';
    else if (S_ISSOCK(mode))
        *(type) = 's';
    else /* unknown file type */
        return;
}
void mod2acs(mode_t mode, char *access)
{
    /* user access permission */
    access[0] = (S_IRUSR & mode) ? 'r' : '-';
    access[1] = (S_IWUSR & mode) ? 'w' : '-';
    access[2] = (S_IXUSR & mode) ? 'x' : '-';

    /* group access permission */
    access[3] = (S_IRGRP & mode) ? 'r' : '-';
    access[4] = (S_IWGRP & mode) ? 'w' : '-';
    access[5] = (S_IXGRP & mode) ? 'x' : '-';

    /* other access permission */
    access[6] = (S_IROTH & mode) ? 'r' : '-';
    access[7] = (S_IWOTH & mode) ? 'w' : '-';
    access[8] = (S_IXOTH & mode) ? 'x' : '-';

    access[9] = '\0';
}

// 打印
void display(struct fileinfo *fileinfo, int n, int flag_long)
{
    int i;
    if (!flag_long) {
        for (i = 0; i < n; i++)
            fprintf(stdout, "%s\n", fileinfo[i].name);
    }
    else { /* if (flag_long)*/
        for (i = 0; i < n; i++) {
            printf("%c", fileinfo[i].type);
            printf("%s.", fileinfo[i].access);
            printf(" ");
            printf("%lu", fileinfo[i].nlink);
            printf(" ");
            /* TODO: map id to name */
            printf("%u", fileinfo[i].uid);
            printf(" ");
            printf("%u", fileinfo[i].gid);
            printf(" ");
            printf("%lu", fileinfo[i].size);
            printf(" ");
            printf("%s", fileinfo[i].name);
            printf("\n");
        }
    }
}

void do_ls(char *pathname)
{
    DIR *dp;
    struct dirent *direntp;
    struct fileinfo fileinfo[1000];
    struct stat statbuf;
    char fullname[100];
    int n = 0;

    // 读取目录
    // 对于非目录,直接列出该文件

    // 对于目录，列出目录下文件
    dp = opendir(pathname);
    while ((direntp = readdir(dp)) != NULL) {
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

        fileinfo[n].name = direntp->d_name;

        // 处理-l
        if (flag_long) {
            // 构建完整路径名
            memset(fullname, 0, sizeof(fullname));
            strcpy(fullname, pathname);
            if (fullname[strlen(fullname)-1] != '/')
                strcat(fullname, "/");
            strcat(fullname, direntp->d_name);

            // stat获取文件完整信息
            memset(&statbuf, 0, sizeof(statbuf));
            stat(fullname, &statbuf);
            mod2type(statbuf.st_mode, &(fileinfo[n].type));
            mod2acs(statbuf.st_mode, fileinfo[n].access);
            fileinfo[n].nlink = statbuf.st_nlink;
            fileinfo[n].uid = statbuf.st_uid;
            fileinfo[n].gid = statbuf.st_gid;
            fileinfo[n].size = statbuf.st_size;
            /* other info... */
        }

        n++;
    }

    display(fileinfo, n, flag_long);
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

    if (optind >= argc)
        do_ls(".");
    else
        while (optind < argc)
            do_ls(argv[optind++]);

    return 0;
}
