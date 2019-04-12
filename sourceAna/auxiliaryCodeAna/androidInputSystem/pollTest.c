#include<stdio.h>
#include <signal.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define _GNU_SOURCE         /* See feature_test_macros(7) */

/*
描述:
    监听一个文件，是否有数据可以读取
用法:
    ./poolTest <file_path>
*/

int main( int argc, char** argv  )
{

    if(argc != 2){
        printf("usage:\n    %s <file_path>", argv[1]);
        return -1;
    }

    char* file_path = argv[1];

    int fd = open(file_path, O_RDWR);
    if(fd < -1){
        printf("error:fd=%d\n", fd);
        return -1;
    }

    struct pollfd  pollFd ={
        .fd = fd,
        .events = POLLIN 
    };
    int ret = -1;
    char buf[20]={0};
    while(1){

        /* code */
        ret = poll(&pollFd, 1, -1);
        if(ret <0){
            printf("error:poll return %d\n", ret);
            continue;
        }
        if(pollFd.revents&POLLIN){
            printf("file chanage!!!\n");
            ret = read(pollFd.fd , buf, 20);
            printf("ret = %d, buf=%s\n", ret, buf);
        }
        usleep(100000);
    }
}

