#ifndef FILTER_H
#define FILTER_H

#include <stdint.h>

void Filter_Init(uint8_t size);
void Filter_AddValue(uint16_t value);
uint16_t Filter_GetMedian(void);
void Filter_SetSize(uint8_t size);
uint8_t Filter_GetSize(void);

#endif
