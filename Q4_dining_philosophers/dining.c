#include "kernel/types.h"
#include "user/user.h"

#define FORK0 0
#define FORK1 1
#define FORK2 2
#define FORK3 3
#define FORK4 4

#define ROOM 5
#define PRINT_MUTEX 6

#define NPHIL 5
#define CYCLES 5

void
philosopher(int id)
{
  int left = id;
  int right = (id + 1) % NPHIL;

  for(int i = 0; i < CYCLES; i++){

    // THINKING
    sem_wait(PRINT_MUTEX);
    printf("Philosopher %d: THINKING\n", id);
    sem_post(PRINT_MUTEX);

    pause(2);

    // HUNGRY
    sem_wait(PRINT_MUTEX);
    printf("Philosopher %d: HUNGRY\n", id);
    sem_post(PRINT_MUTEX);

    // Allow at most 4 philosophers to compete for forks
    sem_wait(ROOM);

    // Pick up both forks
    sem_wait(left);
    sem_wait(right);

    // EATING
    sem_wait(PRINT_MUTEX);
    printf("Philosopher %d: EATING\n", id);
    sem_post(PRINT_MUTEX);

    pause(2);

    // Put down both forks
    sem_post(right);
    sem_post(left);

    // Leave the room
    sem_post(ROOM);
  }

  sem_wait(PRINT_MUTEX);
  printf("Philosopher %d completed all %d cycles\n", id, CYCLES);
  sem_post(PRINT_MUTEX);

  exit(0);
}

int
main(void)
{
  int pid;

  // Initialize the five forks
  for(int i = 0; i < NPHIL; i++)
    sem_init(i, 1);

  // At most four philosophers can compete for forks
  sem_init(ROOM, 4);

  // Protect terminal output
  sem_init(PRINT_MUTEX, 1);

  // Create five philosopher processes
  for(int i = 0; i < NPHIL; i++){
    pid = fork();

    if(pid < 0){
      printf("fork failed\n");
      exit(1);
    }

    if(pid == 0){
      philosopher(i);
    }
  }

  // Wait for all philosophers
  for(int i = 0; i < NPHIL; i++)
    wait(0);

  sem_wait(PRINT_MUTEX);
  printf("All philosophers completed successfully.\n");
  sem_post(PRINT_MUTEX);

  exit(0);
}