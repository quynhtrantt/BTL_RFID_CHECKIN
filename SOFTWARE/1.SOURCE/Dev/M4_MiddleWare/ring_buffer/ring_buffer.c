#include "ring_buffer.h"
#include <stddef.h>

bool RingBuffer_Init(RingBuffer_t *buffer, uint8_t *data, uint16_t capacity) {
    if (!buffer || !data || capacity < 2)
        return false;
    buffer->data = data;
    buffer->capacity = capacity;
    atomic_init(&buffer->head, 0);
    atomic_init(&buffer->tail, 0);
    return true;
}

void RingBuffer_Clear(RingBuffer_t *buffer) {
    atomic_store_explicit(&buffer->tail,
                          atomic_load_explicit(&buffer->head, memory_order_acquire),
                          memory_order_release);
}

uint16_t RingBuffer_Count(const RingBuffer_t *buffer) {
    uint16_t tail = atomic_load_explicit(&buffer->tail, memory_order_acquire);
    uint16_t head = atomic_load_explicit(&buffer->head, memory_order_acquire);
    return head >= tail ? head - tail : buffer->capacity - tail + head;
}

uint16_t RingBuffer_Space(const RingBuffer_t *buffer) {
    return buffer->capacity - 1U - RingBuffer_Count(buffer);
}

bool RingBuffer_Put(RingBuffer_t *buffer, uint8_t value) {
    uint16_t head = atomic_load_explicit(&buffer->head, memory_order_relaxed);
    uint16_t next = head + 1U;
    if (next == buffer->capacity)
        next = 0;
    if (next == atomic_load_explicit(&buffer->tail, memory_order_acquire))
        return false;
    buffer->data[head] = value;
    atomic_store_explicit(&buffer->head, next, memory_order_release);
    return true;
}

bool RingBuffer_Get(RingBuffer_t *buffer, uint8_t *value) {
    uint16_t tail = atomic_load_explicit(&buffer->tail, memory_order_relaxed);
    if (!value || tail == atomic_load_explicit(&buffer->head, memory_order_acquire))
        return false;
    *value = buffer->data[tail];
    tail++;
    if (tail == buffer->capacity)
        tail = 0;
    atomic_store_explicit(&buffer->tail, tail, memory_order_release);
    return true;
}
