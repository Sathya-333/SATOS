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
        threads[i].state = 0;
    }

    threads[0].id = next_thread_id++;
    threads[0].state = 1;
}