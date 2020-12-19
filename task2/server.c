#include "header.h"

int main(int argc, char* argv[])
{
        setbuf(stdout, NULL);
        while (1) {
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
                
                // A(s2,1)
                sb.sem_num = 2;
                sb.sem_op = 1;
                sb.sem_flg = 0;
                if (semop(semid, &sb, 1) == -1) {
                        perror("can't add to 3 sem");
                        return errno;
                }
                
                int* r = (int*) ptr;
                while (1) {
                        if (semctl(shmid, 0, GETVAL) == 2) {
                                return 0;
                        }
                        // D(s1, 1)
                        sb.sem_num = 1;
                        sb.sem_op = -1;
                        sb.sem_flg = 0;
                        if (semop(semid, &sb, 1) == -1) {
                                perror("can't drop 2 sem");
                                return errno;
                        }
                        
                        int wr = -1;
                        if ((wr = write(1, &r[1], r[0])) == -1) {
                                perror("can't write file to stdout");
                                return errno;
                        }
                        if (r[0] == 0) {
                                exit(0);
                        }
                        
                        // A(s2,1)
                        sb.sem_num = 2;
                        sb.sem_op = 1;
                        sb.sem_flg = 0;
                        if (semop(semid, &sb, 1) == -1) {
                                perror("can't add to 3 sem");
                                return errno;
                        }
                }
        }
}

