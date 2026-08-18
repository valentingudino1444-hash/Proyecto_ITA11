/*
 * comunicacion.h
 *
 *  Created on: 17 ago 2026
 *      Author: eduar
 */
#ifndef COMUNICACION_H_
#define COMUNICACION_H_
 
#include <stdint.h>
#include "stm32f407g_uteq.h" //libreria del profe con las estructuras de registros
 
// Inicializa el pin PA2 (TX), PA3 (RX) y el USART2 a 9600 baudios
// requiere que el reloj ya este a 168MHz (SystemClock_Config_168MHz) por el valor del BRR
void Comunicacion_Init(void);
 
// Envía una cadena de texto completa por el puerto serial
void Comunicacion_EnviarCadena(char *str);
 
#endif /* COMUNICACION_H_ */
