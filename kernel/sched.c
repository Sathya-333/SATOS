#include "sched.h"

static struct thread threads[MAX_THREADS];

static uint64_t next_thread_id = 1;

int thread_create(thread_function function)
{
    for (int i = 0; i < MAX_THREADS; i++)
    {
        if (threads[i].state == THREAD_UNUSED)
        {
            threads[i].id = next_thread_id++;
            threads[i].rip = (uint64_t)function;
            threads[i].rsp =
                (uint64_t)&threads[i].stack[STACK_SIZE];

            threads[i].state = THREAD_READY;

            return i;
        }
    }

    return -1;
}

void scheduler_init(void)
{
    for (int i = 0; i < MAX_THREADS; i++)
    {
        threads[i].id = 0;
        threads[i].rsp = 0;
        threads[i].rip = 0;
        threads[i].state = THREAD_UNUSED;
    }

    /*
     * Thread 0 represents the kernel code
     * that is already running.
     */
    threads[0].id = next_thread_id++;
    threads[0].state = THREAD_RUNNING;
}