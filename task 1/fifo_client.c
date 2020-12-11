#include "fifo.h"

char client_fifo[CLIENT_FIFO_NAME_LEN];

int main (int argc, char* argv[])
{
        setbuf(stdout, NULL);
        printf("Client %d has started.\n", getpid());
        
        int server_fd;
        int client_fd;
        struct request req;
        
        if (argc != 2) {
                perror("Error: invalid input.\n");
                exit(1);
        }
        
        umask(0);
        snprintf(client_fifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) getpid());
        if (mkfifo(client_fifo, 0666) == -1  && errno != EEXIST) {
                perror("Error: can't create client fifo");
                return errno;
        }
        
        req.pid = getpid();
        strcpy(req.file_name, argv[1]);
        
        server_fd = open(SERVER_FIFO, O_WRONLY | O_NONBLOCK, NULL);
        fcntl(server_fd, F_SETFL, fcntl(server_fd, F_GETFL) & ~O_NONBLOCK);
        
        client_fd = open(client_fifo, O_RDONLY | O_NONBLOCK, NULL);
        fcntl(client_fd, F_SETFL, fcntl(client_fd, F_GETFL) & ~O_NONBLOCK);
        
        write(server_fd, &req, sizeof(struct request));
        printf("Request has been sent.\n");
        
        char buf[PIPE_BUF] = "";
        int flag = 0;
        int size = PIPE_BUF;
        
        printf("Received file:\n");
        while (size == PIPE_BUF && !buf[0]) {
                sleep(1);
                ioctl(client_fd, FIONREAD, &flag);
                if (!flag) {
                        perror("Error: no data from server");
                        exit(1);
                }
                size = read(client_fd, buf, PIPE_BUF);
                if (size == -1) {
                        perror("Error: can't read");
                        return errno;
                }
                if (write(1, buf + 1 , size - 1) == -1) {
                        perror("Error: can't write data from buffer");
                        return errno;
                }
        }
        if (close(client_fd) == -1) {
                perror("Error: cant't close client fifo");
        }
        if (close(server_fd) == -1) {
                perror("Error: cant't close server fifo");
        }
}
