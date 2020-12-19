#include "header.h"

int main(int argc, char* argv[])
{
        if (argc != 2) {
                perror("incrorrect input");
                exit(1);
        }
        setbuf(stdout, NULL);
        key_t key;
        if ((key = ftok(PATHNAME, 0)) == -1) {
                perror("can't generate key");
                return errno;
        }
        key_t key1;
        if ((key1 = ftok(PATHNAME, 2)) == -1) {
                perror("can't generate key");
                return errno;
        }
                                
        int semid = -1;
        if ((semid = semget(key, 3, 0666 | IPC_CREAT)) == -1) {
             perror("can't get semid");
             return errno;
        }
                
        struct sembuf sb;
        
        int fh = -1;
        if ((fh = open(argv[1], O_RDONLY, 0666)) == -1) {
                perror("can't open file");
                return errno;
        }
        
        int shmid = -1;
        if ((shmid = shmget(key1, PAGE_SIZE, 0666 | IPC_CREAT | IPC_EXCL)) == -1) {
                if (errno != EEXIST) {
                        perror("can't create shmem");
                        return errno;
                }
                else {
                        if ((shmid = shmget(key1, PAGE_SIZE, 0)) == -1) {
                                perror("can't find shmem");
                                return errno;
                        }
                }
        }
            
        char* ptr = NULL;
        if ((ptr = shmat(shmid, NULL, 0)) == (char*) -1) {
                perror("can't attach shmem");
                return errno;
        }
        int* r = (int*) ptr;
        int rd = -1;
        while (1) {
                // D(s2, 1)
                sb.sem_num = 2;
                sb.sem_op = -1;
                sb.sem_flg = 0;
                if (semop(semid, &sb, 1) == -1) {
                        perror("can't drop 2s");
                        return errno;
                }
                // if rd is empty then stop working
                
                if ((r[0] = read(fh, (char*)(&r[1]), PAGE_SIZE)) == -1) {
                        perror("can't read file");
                        return errno;
                }
                
                if (r[0] == 0) {
                        exit(0);
                }

                // A(s1,1)
                sb.sem_num = 1;
                sb.sem_op = 1;
                sb.sem_flg = 0;
                if (semop(semid, &sb, 1) == -1) {
                        perror("can't add to 2 sem");
                        return errno;
                }
        }
}

