#include "filter.h"
#include <stdlib.h>

#define FILTER_MAX 15

typedef struct
{
    uint8_t age;
    uint16_t value;
} FilterItem;

static FilterItem buffer[FILTER_MAX];
static uint8_t filter_size = 5;

static int CompareItems(const void *a, const void *b)
{
    const FilterItem *ia = (const FilterItem *)a;
    const FilterItem *ib = (const FilterItem *)b;

    if (ia->value < ib->value)
        return -1;
    if (ia->value > ib->value)
        return 1;
    return 0;
}

void Filter_Init(uint8_t size)
{
    if (size > FILTER_MAX)
    {
        size = FILTER_MAX;
    }

    filter_size = size;

    for (uint8_t i = 0; i < FILTER_MAX; i++)
    {
        buffer[i].age = i;
        buffer[i].value = 0;
    }
}

void Filter_AddValue(uint16_t value)
{
    for (uint8_t i = 0; i < filter_size; i++)
    {
        buffer[i].age++;
    }

    uint8_t oldest = 0;
    uint8_t max_age = buffer[0].age;

    for (uint8_t i = 1; i < filter_size; i++)
    {
        if (buffer[i].age > max_age)
        {
            max_age = buffer[i].age;
            oldest = i;
        }
    }

    buffer[oldest].value = value;
    buffer[oldest].age = 0;
}

uint16_t Filter_GetMedian(void)
{
    FilterItem temp[FILTER_MAX];
    for (uint8_t i = 0; i < filter_size; i++)
    {
        temp[i] = buffer[i];
    }

    qsort(temp, filter_size, sizeof(FilterItem), CompareItems);

    uint8_t mid = filter_size / 2;
    return temp[mid].value;
}

void Filter_SetSize(uint8_t size)
{
    if (size >= 1 && size <= FILTER_MAX)
    {
        filter_size = size;
    }
}

uint8_t Filter_GetSize(void)
{
    return filter_size;
}
