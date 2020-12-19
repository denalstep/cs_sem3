#include "header.h"

//запускается сервер, инициализируем семафор 1. Клиент, 2. Управление
//после запуска сервер добавляет 1 к 2. и блокируется
//запускается клиент, добавляет 1 к 1. -> другие процессы не смогут взять управление
//через 2. начинается процесс перекачки файла, заканчивается, причем управляет сервер
//клиент снимает 1.; сервер добавляет 1 к 2. и блокируется, ожидая клиента.

int main(int argc, char* argv[])
{
        key_t skey;
        if ((skey = ftok(PATHNAME, 0)) == -1) {
                perror("can't generate key");
                return errno;
        }
        int shmid = -1;
        if ((shmid = shmget(skey, PAGE_SIZE, 0666 | IPC_CREAT | IPC_EXCL)) == -1) {
                if (errno != EEXIST) {
                        perror("can't create shmem");
                        return errno;
                }
                else {
                        if ((shmid = shmget(skey, PAGE_SIZE, 0)) == -1) {
                                perror("can't find shmem");
                                return errno;
                        }
                }
                
        }
        void* ptr;
        ptr = shmat(shmid, NULL, 0);
        
        int  fh = -1;
        if ((fh = open(argv[1], O_RDONLY) == -1)) {
                perror("can't open file");
                return errno;
        }
        
        int rd = -1;
        if ((rd = read(fh, ptr, PAGE_SIZE)) == -1) {
                perror("can't write to shmem");
                return errno;
        }
        int wr = -1;
        if ((wr = write(1, ptr, PAGE_SIZE)) == -1) {
                perror("can't write from shmem");
                return errno;
        }
        //delete sem
}
