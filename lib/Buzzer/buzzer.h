#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>

void Buzzer_Init(void);
void Buzzer_SetFrequency(uint16_t freq); // directe (immediate) instelling
void Buzzer_Update(uint16_t targetFreq); // smooth update (use in main loop)
void Buzzer_SetVolume(uint8_t vol);      // store volume (no hardware gate by default)
uint8_t Buzzer_GetVolume(void);

#endif
