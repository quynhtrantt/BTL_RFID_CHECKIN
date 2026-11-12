#include "fifo.h"
#include "stm32f1xx_hal.h"
#include <stddef.h>

bool FIFO_Init(FIFO_t *fifo, uint8_t *buffer, uint16_t capacity)
{
    if (fifo == NULL)
    {
        return false;
    }

    fifo->buffer = NULL;
    fifo->capacity = 0U;
    fifo->head = 0U;
    fifo->tail = 0U;
    fifo->count = 0U;

    if (buffer == NULL || capacity == 0U)
    {
        return false;
    }

    fifo->buffer = buffer;
    fifo->capacity = capacity;
    return true;
}

bool FIFO_Push(FIFO_t *fifo, uint8_t value)
{
    bool pushed = false;

    if (fifo == NULL || fifo->buffer == NULL || fifo->capacity == 0U)
    {
        return false;
    }

    uint32_t mask = __get_PRIMASK();
    __disable_irq();

    if (fifo->count < fifo->capacity)
    {
        fifo->buffer[fifo->head] = value;
        fifo->head++;

        if (fifo->head == fifo->capacity)
        {
            fifo->head = 0U;
        }

        fifo->count++;
        pushed = true;
    }

    __set_PRIMASK(mask);
    return pushed;
}

bool FIFO_Pop(FIFO_t *fifo, uint8_t *value)
{
    bool popped = false;

    if (fifo == NULL || value == NULL || fifo->buffer == NULL || fifo->capacity == 0U)
    {
        return false;
    }

    uint32_t mask = __get_PRIMASK();
    __disable_irq();

    if (fifo->count > 0U)
    {
        *value = fifo->buffer[fifo->tail];
        fifo->tail++;

        if (fifo->tail == fifo->capacity)
        {
            fifo->tail = 0U;
        }

        fifo->count--;
        popped = true;
    }

    __set_PRIMASK(mask);
    return popped;
}

uint16_t FIFO_Count(const FIFO_t *fifo)
{
    uint16_t count = 0U;

    if (fifo != NULL && fifo->buffer != NULL && fifo->capacity != 0U)
    {
        uint32_t mask = __get_PRIMASK();
        __disable_irq();
        count = fifo->count;
        __set_PRIMASK(mask);
    }

    return count;
}

uint16_t FIFO_Space(const FIFO_t *fifo)
{
    if (fifo == NULL || fifo->buffer == NULL || fifo->capacity == 0U)
    {
        return 0U;
    }

    return fifo->capacity - FIFO_Count(fifo);
}

void FIFO_Clear(FIFO_t *fifo)
{
    if (fifo != NULL && fifo->buffer != NULL && fifo->capacity != 0U)
    {
        uint32_t mask = __get_PRIMASK();
        __disable_irq();
        fifo->head = 0U;
        fifo->tail = 0U;
        fifo->count = 0U;
        __set_PRIMASK(mask);
    }
}
