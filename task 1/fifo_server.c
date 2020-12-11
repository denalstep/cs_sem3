#include "fifo.h"

int main()
{
        setbuf(stdout, NULL);
        printf("Server is working. Press Ctrl + C to exit.\n");
        
        char client_fifo[CLIENT_FIFO_NAME_LEN];
        
        int server_fd;
        int client_fd;
        
        struct request req;
        
        umask(0);
        if (mkfifo(SERVER_FIFO, 0666) == -1 && errno != EEXIST) {
                perror("Error: cant't create fifo");
                return errno;
        }

        if ((server_fd = open(SERVER_FIFO, O_RDWR, NULL)) == -1) {
                perror("Error: can't open fifo");
                return errno;
        }
        
        signal(SIGPIPE, SIG_IGN);
        
        while (1) {
                if (read(server_fd, &req, sizeof(struct request)) != sizeof(struct request)) {
                        perror("Error: can't read request");
                        continue;
                }
                printf("Request has came, client pid: %d; file: %s\n", req.pid, req.file_name);
                
                snprintf(client_fifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) req.pid);
                if ((client_fd = open(client_fifo, O_WRONLY | O_NONBLOCK)) == -1) {
                        perror("Error: can't open client fifo");
                        continue;
                }
                fcntl(client_fd, F_SETFL, fcntl(client_fd, F_GETFL) & ~O_NONBLOCK);
                
                FILE* file = fopen(req.file_name, "rb");
                if (file == NULL) {
                    perror("Error: can't open file");
                    continue;
                }
                
                char buf[PIPE_BUF] = "";
                int size = 0;
                
                while ((size = fread(buf + 1, sizeof(char), PIPE_BUF - 1, file)) == PIPE_BUF - 1) {
                        buf[0] = 0;
                        if (write(client_fd, buf, PIPE_BUF) == -1) {
                                perror("Error: can't send data to client");
                                if (close(client_fd) == -1) {
                                        perror("Error: can't close client fd");
                                        return errno;
                                }
                                break;
                        }
                }
                
                buf[0] = 1;
                if (write(client_fd, buf, size + 1) == -1) {
                        perror("Error: can't send last piece of data to client");
                }
                if (close(client_fd) == -1) {
                        perror("Error: cant't close client fifo");
                }
        }
}
