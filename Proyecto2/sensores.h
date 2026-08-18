#ifndef SENSORES_H_ // Verifica si la librería no ha sido definida previamente
#define SENSORES_H_ // Define la librería para evitar inclusiones múltiples

#include <stdint.h> // Incluye la librería estándar para tipos de datos enteros
#include "STM32F407G_H_.h" // Incluye las definiciones principales del microcontrolador

// Prototipos de funciones públicas del módulo
void Sensores_Init(void); // Inicializa el hardware de los sensores analógicos
uint16_t ADC_Leer_Canal(uint8_t canal); // Realiza la lectura de un canal analógico específico

#endif /* SENSORES_H_ */