#ifndef ACTUADORES_H_
#define ACTUADORES_H_

#include <stdint.h>
#include "STM32F407G_H_.h"

#define PWM_MAX_DUTY 1000

void Actuador_PWM_Init(void);
void PWM_Ajustar_Foco(uint32_t duty);

#endif /* ACTUADORES_H_ */
