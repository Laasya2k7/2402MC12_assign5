#include "kernel/types.h"
#include "user/user.h"

#define BUFFER_SIZE 5
#define ITEMS 20

#define EMPTY 0
#define FULL  1
#define MUTEX 2

int
main(void)
{
    int p[2];
    int pid;

    pipe(p);

    sem_init(EMPTY, BUFFER_SIZE);
    sem_init(FULL, 0);
    sem_init(MUTEX, 1);

    pid = fork();

    if(pid < 0){
        printf("fork failed\n");
        exit(1);
    }

    if(pid == 0){
        // Consumer
        close(p[1]);

        for(int i = 0; i < ITEMS; i++){
            int item;

            sem_wait(FULL);
            sem_wait(MUTEX);

            if(read(p[0], &item, sizeof(item)) != sizeof(item)){
                sem_post(MUTEX);
                sem_post(EMPTY);
                printf("Consumer: read failed\n");
                exit(1);
            }

            printf("Consumer: consumed %d\n", item);

            sem_post(MUTEX);
            sem_post(EMPTY);

            pause(2);
        }

        close(p[0]);
        exit(0);
    }
    else{
        // Producer
        close(p[0]);

        for(int i = 1; i <= ITEMS; i++){
            sem_wait(EMPTY);
            sem_wait(MUTEX);

            if(write(p[1], &i, sizeof(i)) != sizeof(i)){
                sem_post(MUTEX);
                sem_post(FULL);
                printf("Producer: write failed\n");
                exit(1);
            }

            printf("Producer: produced %d\n", i);

            sem_post(MUTEX);
            sem_post(FULL);

            pause(1);
        }

        close(p[1]);

        wait(0);
        exit(0);
    }
}