#include "sensores.h" // Incluye la cabecera del propio módulo

// Direcciones de memoria de los registros utilizados
#define RCC_AHB1ENR  (*((volatile uint32_t *)0x40023830)) // Registro para habilitar el reloj de los puertos GPIO
#define RCC_APB2ENR  (*((volatile uint32_t *)0x40023844)) // Registro para habilitar el reloj del periférico ADC
#define GPIOA_MODER  (*((volatile uint32_t *)0x40020000)) // Registro para configurar el modo de los pines del Puerto A

#define ADC1_SR      (*((volatile uint32_t *)0x40012000)) // Registro de estado del ADC1
#define ADC1_CR2     (*((volatile uint32_t *)0x40012008)) // Registro de control 2 del ADC1
#define ADC1_SMPR2   (*((volatile uint32_t *)0x40012010)) // Registro para configurar los tiempos de muestreo
#define ADC1_SQR3    (*((volatile uint32_t *)0x40012034)) // Registro para establecer la secuencia de conversión
#define ADC1_DR      (*((volatile uint32_t *)0x4001204C)) // Registro que almacena el dato resultante de la conversión
#define ADC_CCR      (*((volatile uint32_t *)0x40012304)) // Registro de control común para todos los ADC

void Sensores_Init(void) {
    // Configura los relojes del sistema
    RCC_AHB1ENR |= (1 << 0); // Habilita el reloj para el Puerto A
    RCC_APB2ENR |= (1 << 8); // Habilita el reloj para el convertidor ADC1

    // Configura los pines de entrada
    GPIOA_MODER |= (3 << 8) | (3 << 10); // Establece los pines PA4 y PA5 en modo analógico

    // Configura la velocidad del convertidor
    ADC_CCR &= ~(3UL << 16); // Limpia los bits del divisor de reloj
    ADC_CCR |= (1UL << 16);  // Aplica un divisor para mantener la velocidad dentro de los límites seguros

    // Configura la precisión de la lectura
    ADC1_SMPR2 |= (1 << 12) | (1 << 15); // Aumenta el tiempo de muestreo para los canales utilizados

    // Inicializa el hardware
    ADC1_CR2 |= (1 << 0); // Enciende el módulo ADC1
}

uint16_t ADC_Leer_Canal(uint8_t canal) {
    ADC1_SQR3 = canal;            // Selecciona el canal analógico a convertir
    ADC1_CR2 |= (1 << 30);        // Inicia el proceso de conversión analógico a digital
    while(!(ADC1_SR & (1 << 1))); // Espera hasta que la conversión termine por completo
    return (uint16_t)ADC1_DR;     // Devuelve el valor digitalizado de la medición
}