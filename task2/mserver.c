#include "header.h"

//запускается сервер, инициализируем семафор 1. Клиент, 2. Управление
//после запуска сервер добавляет 1 к 2. и блокируется
//запускается клиент, добавляет 1 к 1. -> другие процессы не смогут взять управление
//через 2. начинается процесс перекачки файла, заканчивается, причем управляет сервер
//клиент снимает 1.; сервер добавляет 1 к 2. и блокируется, ожидая клиента.

int main(int argc, char* argv[])
{
        setbuf(stdout, NULL);
        while (1) {
                key_t key;
                if ((key = ftok(PATHNAME, 0)) == -1) {
                        perror("can't generate key");
                        return errno;
                }
                
                int semid = -1;
                if ((semid = semget(key, 3, 0666 | IPC_CREAT)) == -1) {
                        perror("can't get semid");
                        return errno;
                }
                
                struct sembuf sb;
                
                
                sb.sem_num = 1;
                sb.sem_op = 1;
                sb.sem_flg = 0;
                if (semop(semid, &sb, 1) == -1) {
                        perror("can't add to 2s");
                        return errno;
                }
                printf("add 1 to 2s\n");
                
                // server blocks till 2s != 0
                sb.sem_num = 1;
                sb.sem_op = 0;
                sb.sem_flg = 0;
                if (semop(semid, &sb, 1) == -1) {
                        perror("can't add to 2 sem");
                        return errno;
                }
                printf("server blocks till 2s != 0\n");
                
                key_t skey;
                if ((skey = ftok(PATHNAME, 1)) == -1) {
                        perror("can't generate key");
                        return errno;
                }
                //data transfer
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
                if ((ptr = shmat(shmid, NULL, 0)) == (void*) -1) {
                        perror("can't attach shmem");
                        return errno;
                }
                int wr = -1;
                if ((wr = write(1, ptr, PAGE_SIZE)) == -1) {
                        perror("can't write from shmem");
                        return errno;
                }
                printf("dfdf");
                
                int a = 1;
                while (1) {
                        
                        printf("%d serv\n", a);
                        a = a + 1;
                        sleep(1);
                        //give control to client
                        sb.sem_num = 2;
                        sb.sem_op = -1;
                        sb.sem_flg = 0;
                        if (semop(semid, &sb, 2) == -1) {
                                perror("can't add to 2 sem");
                                return errno;
                        }
                        
                        sb.sem_num = 1;
                        sb.sem_op = 1;
                        sb.sem_flg = 0;
                        if (semop(semid, &sb, 2) == -1) {
                                perror("can't add to 2 sem");
                                return errno;
                        }
                        
                        sb.sem_num = 1;
                        sb.sem_op = 0;
                        sb.sem_flg = 0;
                        if (semop(semid, &sb, 2) == -1) {
                                perror("can't add to 2 sem");
                                return errno;
                        }
                        
                }
        }
        //delete sem
}
