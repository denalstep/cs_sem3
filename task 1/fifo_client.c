#include "fifo.h"

int main (int argc, char* argv[])
{
        if (argc != 2) {
                perror("invalid input");
                exit(1);
        }
        
        umask(0);
        char client_fifo[CLIENT_FIFO_NAME_LEN];
        snprintf(client_fifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) getpid());
        if (mkfifo(client_fifo, 0666) == -1  && errno != EEXIST) {
                perror("can't create client fifo");
                return errno;
        }
        
        struct request req;
        req.pid = getpid();
        strcpy(req.file_name, argv[1]);
        
        int server_fd = -1;
        if ((server_fd = open(SERVER_FIFO, O_WRONLY, NULL)) == -1) {
                unlink(client_fifo);
                perror("client can't open server fifo.");
                return errno;
        }
        if (write(server_fd, &req, sizeof(struct request)) == -1) {
                unlink(client_fifo);
                perror("client can't send request.");
                return errno;
        }
        if (close(server_fd) == -1) {
                perror("cant't close server fifo");
        }
        
        int client_fd = -1;
        if ((client_fd = open(client_fifo, O_RDONLY, NULL)) == -1) {
                unlink(client_fifo);
                perror("client can't open own fifo");
                return errno;
        }
        
        char buf[PAGE_SIZE] = "";
        
        while (1) {
                int rd = -1;
                if ((rd = read(client_fd, buf, PAGE_SIZE)) < 0) {
                        perror("can't read");
                        return errno;
                }
                if (rd == 0) {
                        break;
                }
                if (write(1, buf, rd) < 0 ) {
                        perror("can't write data from buffer");
                        return errno;
                }
        }
        
        if (close(client_fd) == -1) {
                perror("cant't close client fifo");
        }
        unlink(client_fifo);
}
