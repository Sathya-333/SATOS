#include "sched.h"

static struct thread threads[MAX_THREADS];

static uint64_t next_thread_id = 1;
static int current_thread = 0;
static void thread_start(void);

int thread_create(thread_function function)
{
    for (int i = 0; i < MAX_THREADS; i++)
    {
        if (threads[i].state == THREAD_UNUSED)
        {
            uint64_t* stack_top;

            threads[i].id = next_thread_id++;
            threads[i].state = THREAD_READY;
            threads[i].function = function;

            stack_top =
                (uint64_t*)&threads[i].stack[STACK_SIZE];

            /*
             * context_switch() expects:
             *
             * RSP -> r15
             *        r14
             *        r13
             *        r12
             *        rbx
             *        rbp
             *        return address
             */

            *(--stack_top) = (uint64_t)thread_start; /* return address */
            *(--stack_top) = 0;                      /* rbp */
            *(--stack_top) = 0;                      /* rbx */
            *(--stack_top) = 0;                      /* r12 */
            *(--stack_top) = 0;                      /* r13 */
            *(--stack_top) = 0;                      /* r14 */
            *(--stack_top) = 0;                      /* r15 */

            threads[i].rsp = (uint64_t)stack_top;

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

static void thread_start(void)
{
    thread_function function;

    /*
     * A newly created thread may have been started
     * from inside a timer interrupt.
     *
     * CPU interrupts are disabled while entering an IRQ.
     * Enable them before running the thread.
     */
    __asm__ volatile ("sti");

    function = threads[current_thread].function;

    function();

    thread_exit();
}

void scheduler_switch_to(int thread_index)
{
    int previous_thread;

    previous_thread = current_thread;

    threads[previous_thread].state = THREAD_READY;
    threads[thread_index].state = THREAD_RUNNING;

    current_thread = thread_index;

    context_switch(
        &threads[previous_thread].rsp,
        threads[thread_index].rsp
    );
}

void scheduler_tick(void)
{
    int next_thread;

    /*
     * Search for the next READY thread,
     * starting immediately after the current thread.
     *
     * Thread 0 is the bootstrap kernel context,
     * so only threads 1..MAX_THREADS-1 are considered.
     */
    for (int offset = 1; offset < MAX_THREADS; offset++)
    {
        next_thread = current_thread + offset;

        if (next_thread >= MAX_THREADS)
        {
            next_thread -= (MAX_THREADS - 1);
        }

        if (next_thread == 0)
        {
            continue;
        }

        if (threads[next_thread].state == THREAD_READY)
        {
            scheduler_switch_to(next_thread);
            return;
        }
    }
}

void thread_exit(void)
{
    threads[current_thread].state = THREAD_UNUSED;

    scheduler_switch_to(0);

    while (1)
    {
        __asm__ volatile ("hlt");
    }
}