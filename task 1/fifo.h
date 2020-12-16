#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>
#include <limits.h>

//#define SERVER_FIFO "/Users/fragett1/workspace/hw/p1/sv"
//#define CLIENT_FIFO_TEMPLATE "/Users/fragett1/workspace/hw/p1/cl.%ld"

#define PAGE_SIZE 4096
#define SERVER_FIFO "/tmp/fifo_server"
#define CLIENT_FIFO_TEMPLATE "/tmp/fifo_client.%ld"
#define CLIENT_FIFO_NAME_LEN (sizeof(CLIENT_FIFO_TEMPLATE) + 20)

struct request {
    pid_t pid;
    char file_name[256];
};

static volatile int keep_running = 1;
void sigint_handler(int sig)
{
        if (sig == SIGINT) {
                unlink(SERVER_FIFO);
                char msg[8] = "Exit\n";
                write(1, msg, sizeof(msg));
                keep_running = 0;
        }
}
