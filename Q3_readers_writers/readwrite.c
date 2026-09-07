#include "kernel/types.h"
#include "user/user.h"

#define RMUTEX 0
#define RESOURCE 1
#define SERVICE_QUEUE 2
#define PRINT_MUTEX 3

struct shared_state {
  int shared_data;
  int read_count;
};

volatile struct shared_state *shared;

void
reader(int id)
{
  for(int i = 0; i < 5; i++){
    sem_wait(PRINT_MUTEX);

printf("[%d] Reader %d (PID %d) waiting\n",
       uptime(), id, getpid());

sem_post(PRINT_MUTEX);

    // Join the fair queue.
    sem_wait(SERVICE_QUEUE);

    // Update reader count safely.
    sem_wait(RMUTEX);

    shared->read_count++;

    // First reader locks the shared resource.
    if(shared->read_count == 1)
      sem_wait(RESOURCE);

    // Allow the next process to enter the queue.
    sem_post(SERVICE_QUEUE);

    sem_post(RMUTEX);

    // Multiple readers can be here at the same time.
    sem_wait(PRINT_MUTEX);

printf("[%d] Reader %d (PID %d) START: data=%d, readers=%d\n",
       uptime(), id, getpid(),
       shared->shared_data, shared->read_count);

sem_post(PRINT_MUTEX);

    pause(2);

    sem_wait(PRINT_MUTEX);

printf("[%d] Reader %d (PID %d) END\n",
       uptime(), id, getpid());

sem_post(PRINT_MUTEX);

    // Leave the reader group.
    sem_wait(RMUTEX);

    shared->read_count--;

    // Last reader releases the shared resource.
    if(shared->read_count == 0)
      sem_post(RESOURCE);

    sem_post(RMUTEX);

    pause(1);
  }

  exit(0);
}

void
writer(int id)
{
  for(int i = 0; i < 5; i++){
    sem_wait(PRINT_MUTEX);

printf("[%d] Writer %d (PID %d) waiting\n",
       uptime(), id, getpid());

sem_post(PRINT_MUTEX);

    // Enter the fair queue.
    sem_wait(SERVICE_QUEUE);

    // Writer needs exclusive access.
    sem_wait(RESOURCE);

    // Allow the next process to enter the queue.
    sem_post(SERVICE_QUEUE);

    sem_wait(PRINT_MUTEX);

printf("[%d] Writer %d (PID %d) START: old data=%d\n",
       uptime(), id, getpid(), shared->shared_data);

sem_post(PRINT_MUTEX);

    shared->shared_data++;

    pause(2);

    sem_wait(PRINT_MUTEX);

printf("[%d] Writer %d (PID %d) END: new data=%d\n",
       uptime(), id, getpid(), shared->shared_data);

sem_post(PRINT_MUTEX);

    // Release shared resource.
    sem_post(RESOURCE);

    pause(1);
  }

  exit(0);
}

int
main(void)
{
  int pid;
  int children = 5;

  // Create the shared page before forking.
  uint64 addr = shm_get();

  if(addr == 0){
    printf("shm_get failed\n");
    exit(1);
  }

  shared = (volatile struct shared_state *)addr;

  shared->shared_data = 0;
  shared->read_count = 0;

  // Initialize:
  // RMUTEX       -> protects read_count
  // RESOURCE     -> protects shared_data
  // SERVICE_QUEUE -> provides fairness
 sem_init(RMUTEX, 1);
sem_init(RESOURCE, 1);
sem_init(SERVICE_QUEUE, 1);
sem_init(PRINT_MUTEX, 1);

  // Create 3 readers and 2 writers.
  for(int i = 0; i < children; i++){
    pid = fork();

    if(pid < 0){
      printf("fork failed\n");
      exit(1);
    }

    if(pid == 0){
      if(i < 3)
        reader(i + 1);
      else
        writer(i - 2);
    }
  }

  // Parent waits for all five children.
  for(int i = 0; i < children; i++)
    wait(0);

  printf("\nFinal shared_data = %d\n", shared->shared_data);
  printf("Final read_count = %d\n", shared->read_count);

  exit(0);
}