#ifndef SCHED_H
#define SCHED_H

#include <stdint.h>

#define MAX_THREADS 16
#define STACK_SIZE 4096

enum thread_state
{
    THREAD_UNUSED,
    THREAD_READY,
    THREAD_RUNNING
};

struct thread
{
    uint64_t id;

    uint64_t rsp;
    uint64_t rip;

    uint8_t state;

    uint8_t stack[STACK_SIZE];
};

void scheduler_init(void);

#endif