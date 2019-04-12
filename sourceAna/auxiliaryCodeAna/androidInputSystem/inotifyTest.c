#include<stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
/*
描述:
    用于监测一个文件夹下文件的创建、删除事件
用法： 
    ./inotifyTest <file_path>
*/

/* 每次读取 struct inotify_event数据个数 */
#define READ_INOTIFY_EVENT_NUMS         10
#define INOTIFY_EVENT_SIZE  (sizeof(struct inotify_event))
#define READ_BUF_SIZE       (READ_INOTIFY_EVENT_NUMS*INOTIFY_EVENT_SIZE)



static int para_check( int argc, char** argv)
{

    if(argc != 2){
        return -1;
    }
    return 0;
}

static char* get_file_path(char** argv)
{

    return argv[1];

}

/* 处理上报的事件 */
static void handle_events(char* event_buf, int read_len)
{
    struct inotify_event* event;
    char* ptr = event_buf;
    /* 计算出事件尾部 */
    char* event_end = event_buf+read_len;
    for(; ptr < event_end ;){
        
        event = (struct inotify_event*)ptr;
        ptr+=sizeof(struct inotify_event)+event->len;
        
        if(event->mask &IN_CREATE){
            
            printf("file create!!!,[file name]%s\n", event->name);

        }else if(event->mask&IN_DELETE){

            printf("file delete!!!,[file name]%s\n", event->name);

        }else{

            printf("event can't be deal!!,[file name]%s\n", event->name);

        }

    }

}

/* 监听 */
static void lisener(int inotify_fd)
{

    
    int read_len = -1;
    char event_buf[READ_BUF_SIZE];
        //__attribute__ ((aligned(__alignof__(struct inotify_event))));

    for(;;){
        read_len = read(inotify_fd, event_buf, READ_BUF_SIZE);
        
        if(read_len < 16){
            continue;
        }
        /* 处理数据 */
        handle_events(event_buf, read_len);
       
    }
}


int main(int argc, char**argv)
{
    if(0 != para_check(argc, argv)){

        printf("Usage:\n    %s <path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* inotify 队列句柄 */
    int inotify_fd = -1;
    /* 记录一个被监视的路径 */
    int path_fd1 = -1;

    char* file_path = NULL;

    inotify_fd = inotify_init();
    if(inotify_fd < -1 ){

        printf(" error:[inotify_fd]%d\n", inotify_fd);
        exit(EXIT_FAILURE);
    }

    file_path = get_file_path(argv);
    printf("[file_path]%s\n", file_path);
    /* 添加监听事件 */
    path_fd1 = inotify_add_watch(inotify_fd, file_path, IN_CREATE|IN_DELETE);
    if(path_fd1 < 0){
        printf("error:[path_fd1]%d\n", path_fd1);
        exit(EXIT_FAILURE);
    }

    /* 监测 */
    lisener(inotify_fd);

    exit(EXIT_SUCCESS);    
}