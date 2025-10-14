#ifndef ADC_H
#define ADC_H

#include <stdint.h>

void adc_init_freerun(void);
uint8_t adc_get_last(void); // 8-bit top
extern volatile uint8_t adc_value;

#endif
