#ifndef SCHED_H
#define SCHED_H

#include <stdint.h>

#define MAX_THREADS 16
#define STACK_SIZE 4096

enum thread_state
{
    THREAD_UNUSED,
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_BLOCKED
};

typedef void (*thread_function)(void);

struct thread
{
    uint64_t id;
    uint64_t rsp;
    uint64_t rip;
    uint8_t state;
    thread_function function;
    uint8_t stack[STACK_SIZE] __attribute__((aligned(16)));
};

void scheduler_init(void);
int thread_create(thread_function function);
void context_switch(uint64_t *old_rsp, uint64_t new_rsp);
void scheduler_switch_to(int thread_index);
void thread_exit(void);
void scheduler_tick(void);

#endif