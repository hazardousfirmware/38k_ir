#ifndef __RING_H__
#define __RING_H__

#define RING_BUFFER_MAX 64
#define RING_EMPTY_VALUE 0xffffffff

#include <stdint.h>

void ring_put(uint32_t value);
uint32_t ring_get(void);

int ring_empty(void);

#endif // !__RING_H__
