#ifndef FIFO_H
#define FIFO_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint8_t *buffer;
    uint16_t capacity;
    volatile uint16_t head;
    volatile uint16_t tail;
    volatile uint16_t count;
} FIFO_t;

bool FIFO_Init(FIFO_t *fifo, uint8_t *buffer, uint16_t capacity);
bool FIFO_Push(FIFO_t *fifo, uint8_t value);
bool FIFO_Pop(FIFO_t *fifo, uint8_t *value);
uint16_t FIFO_Count(const FIFO_t *fifo);
uint16_t FIFO_Space(const FIFO_t *fifo);
void FIFO_Clear(FIFO_t *fifo);

#endif
