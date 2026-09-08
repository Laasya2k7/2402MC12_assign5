Assignment 5 – Process Synchronization using xv6

This assignment implements four classical process synchronization problems in the xv6 operating system using shared memory, semaphores, mutual exclusion, and process coordination.

Questions Implemented
Q1 – Peterson's Algorithm

Peterson's Algorithm is used to provide mutual exclusion between two processes. Shared variables flag[] and turn are used to coordinate access to a shared counter. The parent and child process each enter the critical section 10 times. The final counter value verifies that both processes accessed the critical section correctly without overlapping.

Q2 – Producer–Consumer

The bounded-buffer Producer–Consumer problem is implemented using a shared buffer and three semaphores:

empty – keeps track of available buffer slots.
full – keeps track of filled buffer slots.
mutex – provides mutual exclusion while accessing the buffer.

The producer generates values 1–20, while the consumer removes them in the same order. The synchronization prevents buffer overflow, underflow, data loss, and duplication.

Q3 – Readers–Writers

The Readers–Writers problem allows multiple readers to access shared data concurrently, while writers require exclusive access. rmutex protects the reader count, resource controls access to the shared resource, and serviceQueue provides fair admission so that writers do not starve indefinitely.

The implementation uses 3 readers and 2 writers, with each process performing multiple operations.

Q4 – Dining Philosophers

The Dining Philosophers problem is implemented using five fork semaphores and a ROOM semaphore. Each philosopher follows:

THINKING → HUNGRY → EATING

A philosopher must acquire the ROOM slot and both neighboring forks before eating. The ROOM semaphore allows at most four philosophers to compete for forks at the same time, breaking the circular-wait condition and preventing deadlock. Each philosopher completes five eating cycles.

xv6 Shared Synchronization Support

The implementations use lightweight kernel semaphores added to xv6. Shared memory is also used where processes need to access common state.

The semaphore interface provides:

sem_init(id, value);
sem_wait(id);
sem_post(id);

These operations allow user processes to synchronize access to shared resources.

How to Build

Clone the repository and enter the xv6 directory:

cd xv6-riscv

Clean any previous build:

make clean

Build xv6:

make

If the build completes successfully, start xv6 using:

make qemu
How to Run the Programs

Once the xv6 shell appears, run each program separately:

Q1 – Peterson
$ peterson

The output should show both processes entering the critical section and a final shared counter of 20.

Q2 – Producer–Consumer
$ prodcons

The producer should generate values from 1 to 20, and the consumer should consume them in the same order.

Q3 – Readers–Writers
$ readwrite

The output shows multiple readers accessing the shared data concurrently and writers accessing it exclusively.

Q4 – Dining Philosophers
$ dining

The output shows the five philosophers transitioning between THINKING, HUNGRY, and EATING, with all philosophers completing their required cycles.
