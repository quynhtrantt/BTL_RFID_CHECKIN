#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdatomic.h>

typedef struct {
    uint8_t *data;
    uint16_t capacity;
    atomic_uint_least16_t head;
    atomic_uint_least16_t tail;
} RingBuffer_t;

bool RingBuffer_Init(RingBuffer_t *buffer, uint8_t *data, uint16_t capacity);
void RingBuffer_Clear(RingBuffer_t *buffer);
uint16_t RingBuffer_Count(const RingBuffer_t *buffer);
uint16_t RingBuffer_Space(const RingBuffer_t *buffer);
bool RingBuffer_Put(RingBuffer_t *buffer, uint8_t value);
bool RingBuffer_Get(RingBuffer_t *buffer, uint8_t *value);

#endif
