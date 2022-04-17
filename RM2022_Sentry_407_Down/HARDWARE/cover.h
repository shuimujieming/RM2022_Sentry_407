#ifndef __COVER_H
#define __COVER_H

#include "main.h"
void Cover_Init();

#define COVER_OPEN_VAL 2100
#define COVER_CLOSE_VAL 1900

#define COVER_ON	TIM_SetCompare1(TIM1,COVER_OPEN_VAL)
#define COVER_OFF	TIM_SetCompare1(TIM1,COVER_CLOSE_VAL)

#define COVER_CCR TIM1->CCR1

#endif