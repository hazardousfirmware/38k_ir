#include "ring.h"

// Ring buffer to queue timestamps for processing
uint32_t queue[RING_BUFFER_MAX] = {0};
volatile int head = 0;
volatile int tail = 0;

inline int ring_empty(void)
{
    return head == tail;
}

static inline int ring_full(void)
{
    return (head + 1 == tail);
}

void ring_put(uint32_t value)
{
    queue[head] = value;
    
    if (ring_full())
    {
        tail++;
        if (tail == RING_BUFFER_MAX)
        {
            tail = 0;
        }
    }

    if (++head >= RING_BUFFER_MAX)
    {
        head = 0;
    }
}

uint32_t ring_get(void)
{
    if (ring_empty())
    {
        return RING_EMPTY_VALUE;
    }

    uint32_t value = queue[tail];
    
    tail++;

    if (tail == RING_BUFFER_MAX)
    {
        tail = 0;
    }

    return value;
}
