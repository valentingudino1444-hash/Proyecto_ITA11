#include "actuadores.h"

#define RCC_AHB1ENR  (*((volatile uint32_t *)0x40023830))
#define RCC_APB1ENR  (*((volatile uint32_t *)0x40023840))
#define GPIOD_MODER  (*((volatile uint32_t *)0x40020C00))
#define GPIOD_AFRH   (*((volatile uint32_t *)0x40020C24))

#define TIM4_CR1     (*((volatile uint32_t *)0x40000800))
#define TIM4_CCMR2   (*((volatile uint32_t *)0x4000081C))
#define TIM4_CCER    (*((volatile uint32_t *)0x40000820))
#define TIM4_PSC     (*((volatile uint32_t *)0x40000828))
#define TIM4_ARR     (*((volatile uint32_t *)0x4000082C))
#define TIM4_CCR4    (*((volatile uint32_t *)0x40000840))

void Actuador_PWM_Init(void) {
    // Encender reloj de GPIOD y TIM4
    RCC_AHB1ENR |= (1 << 3);
    RCC_APB1ENR |= (1 << 2);

    // PD15 (Salida MOC3021 y LED Azul) a Función Alterna (AF2)
    GPIOD_MODER &= ~(3UL << 30);
    GPIOD_MODER |= (2UL << 30);
    GPIOD_AFRH &= ~(0xFUL << 28);
    GPIOD_AFRH |= (2UL << 28);

    // --- FRECUENCIA LENTA (1 Hz) PARA EL TRIAC ---
    // APB1 Timer Clock = 84 MHz
    TIM4_PSC = 84000 - 1;        // Reloj del Timer a 1 kHz (1ms por tick)
    TIM4_ARR = PWM_MAX_DUTY - 1; // Periodo de 1000ms = 1 Segundo (1 Hz)

    // PWM Modo 1 en CH4
    TIM4_CCMR2 &= ~(0x7UL << 12);
    TIM4_CCMR2 |= (6UL << 12);
    TIM4_CCMR2 |= (1UL << 11);
    TIM4_CCER |= (1UL << 12);

    // Iniciar Timer
    TIM4_CR1 |= (1UL << 0);
}

void PWM_Ajustar_Foco(uint32_t duty) {
    if(duty > PWM_MAX_DUTY) duty = PWM_MAX_DUTY;
    TIM4_CCR4 = duty;
}

