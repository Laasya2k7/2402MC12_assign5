#include "kernel/types.h"
#include "user/user.h"

struct shared_data {
    int flag[2];
    int turn;
    int shared_counter;
};

int
main(void)
{
    volatile struct shared_data *shared;
    int pid;

    shared = (struct shared_data *)shm_get();

    if(shared == 0){
        printf("shm_get failed\n");
        exit(1);
    }

    shared->flag[0] = 0;
    shared->flag[1] = 0;
    shared->turn = 0;
    shared->shared_counter = 0;

    pid = fork();

    if(pid < 0){
        printf("fork failed\n");
        exit(1);
    }

    if(pid == 0){
        // Child = Process 1
        for(int i = 0; i < 10; i++){
            shared->flag[1] = 1;
            shared->turn = 0;

            while(shared->flag[0] == 1 && shared->turn == 0);

            printf("Process 1 in CS, counter = %d\n",
       ++shared->shared_counter);

printf("Process 1 exiting CS\n");

shared->flag[1] = 0;

            pause(2);
        }

        exit(0);
    }
    else{
        // Parent = Process 0
        for(int i = 0; i < 10; i++){
            shared->flag[0] = 1;
            shared->turn = 1;

            while(shared->flag[1] == 1 && shared->turn == 1);

            printf("Process 0 in CS, counter = %d\n",
       ++shared->shared_counter);

printf("Process 0 exiting CS\n");

shared->flag[0] = 0;

            pause(2);
        }

        wait(0);

        printf("Final counter = %d\n", shared->shared_counter);

        exit(0);
    }
}