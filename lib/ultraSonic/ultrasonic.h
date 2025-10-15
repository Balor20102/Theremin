#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

void Ultrasonic_Init(void);
void Ultrasonic_Trigger(void);
bool Ultrasonic_IsReady(void);
uint16_t Ultrasonic_GetDistance(void);

#endif
