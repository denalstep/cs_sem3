#include "fifo.h"

int main()
{
        signal(SIGINT, sigint_handler);
        setbuf(stdout, NULL);
        printf("Server is working. Press Ctrl + C to exit.\n");
        
        umask(0);
        if (mkfifo(SERVER_FIFO, 0666) == -1 && errno != EEXIST) {
                perror("can't create server fifo");
                return errno;
        }
        
        int server_fd = -1;
        if ((server_fd = open(SERVER_FIFO, O_RDONLY)) == -1) {
                perror("can't open server fifo");
                return errno;
        }
        
        struct request req;
        while(keep_running) {
                char client_fifo[CLIENT_FIFO_NAME_LEN];
                if (read(server_fd, &req, sizeof(struct request)) != sizeof(struct request)) {
                        continue;
                }
                printf("Request has been received, client pid: %d, path to file: %s\n", req.pid, req.file_name);
                
                snprintf(client_fifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) req.pid);
                int client_fd = -1;
                if ((client_fd = open(client_fifo, O_WRONLY)) == -1) {
                        perror("can't open client fifo");
                        continue;
                }
                
                int file = -1;
                if ((file = open(req.file_name, O_RDONLY, 0666)) == -1) {
                        perror("can't open file");
                        continue;
                }
                
                void* buf = NULL;
                if ((buf = malloc(PAGE_SIZE)) == NULL) {
                        fprintf(stderr, "OOM\n");
                        return -1;
                }
                
                for(;;) {
                        int rd = -1;
                        if ((rd = read(file, buf, PAGE_SIZE)) < 0) {
                                perror("can't read from file");
                                return errno;
                        }
                        if (rd == 0) {
                                break;
                        }
                        if (write(client_fd, buf, PAGE_SIZE) < 0) {
                                perror("can't write data to client");
                                return errno;
                        }
                        
                }
                if (close(file) == -1) {
                        perror("can't close file");
                        return errno;
                }
                if (close(client_fd) == -1) {
                        perror("can't close client fifo");
                        return errno;
                }
        }
}
