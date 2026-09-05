#include "sched.h"

static struct thread threads[MAX_THREADS];

static uint64_t next_thread_id = 1;

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
     * Create the first kernel thread.
     * This represents the code that is already running.
     */
    threads[0].id = next_thread_id++;
    threads[0].state = THREAD_RUNNING;
}