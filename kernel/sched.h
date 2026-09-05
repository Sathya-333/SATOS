#ifndef SCHED_H
#define SCHED_H

#include <stdint.h>

#define MAX_THREADS 16

struct thread
{
    uint64_t id;
    uint64_t rsp;
    uint64_t rip;

    uint8_t state;
};

void scheduler_init(void);

#endif
