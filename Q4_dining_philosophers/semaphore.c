#include "types.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"

#define NSEM 7

struct semaphore {
  struct spinlock lock;
  int value;
};

struct semaphore semaphores[NSEM];

void
sem_init(int id, int value)
{
  if(id < 0 || id >= NSEM)
    return;

  initlock(&semaphores[id].lock, "semaphore");
  semaphores[id].value = value;
}

void
sem_wait(int id)
{
  if(id < 0 || id >= NSEM)
    return;

  acquire(&semaphores[id].lock);

  while(semaphores[id].value == 0){
    sleep_prepare(&semaphores[id]);
    release(&semaphores[id].lock);

    sleep();

    acquire(&semaphores[id].lock);
  }

  semaphores[id].value--;

  release(&semaphores[id].lock);
}

void
sem_post(int id)
{
  if(id < 0 || id >= NSEM)
    return;

  acquire(&semaphores[id].lock);

  semaphores[id].value++;

  wakeup(&semaphores[id]);

  release(&semaphores[id].lock);
}