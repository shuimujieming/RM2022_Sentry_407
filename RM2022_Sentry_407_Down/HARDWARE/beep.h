#ifndef __BEEP_H
#define __BEEP_H

#include "main.h"

#define BEEP_TUNE        TIM4->ARR
#define BEEP_SOUND        TIM4->CCR3
void Beep_Ctrl(uint16_t tune, uint16_t sound);
void Beep_Init();


#endif