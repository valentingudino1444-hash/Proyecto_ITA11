/*
 * comunicacion.c
 *
 *  Created on: 17 ago 2026
 *      Author: eduar
 */
#include "comunicacion.h"

void Comunicacion_Init(void) {
    // 1. Habilitar reloj de GPIOA y USART2
    ADDR_RCC_AHB1ENR->gpioa_en = 1;	// Habilitar el reloj de GPIOA
    ADDR_RCC_APB1ENR->usart2en = 1;	// Habilitar el Reloj de USART2
 
    // 2. Configurar PA2 (TX) y PA3 (RX) en modo Funcion Alterna (10 binario)
    ADDR_GPIOA_MODER->moder2 = 2;	// Funcion alterna A2
    ADDR_GPIOA_MODER->moder3 = 2;	// Funcion alterna A3
 
    // Mapear los pines a la Funcion Alterna 7 (AF7 = USART2) en el registro AFRL
    ADDR_GPIOA_AFRL->AFRL2 = 7;	// AF7 = UART para el GPIOA2
    ADDR_GPIOA_AFRL->AFRL3 = 7;	// AF7 = UART para el GPIOA3
 
    // 3. Configurar el Baud Rate (Velocidad de comunicacion)
    // Con SystemClock_Config_168MHz(), APB1 (PCLK1) = 42 MHz. Para 9600 baudios:
    // 42,000,000 / (16 * 9600) = 273.4375
    // Mantisa = 273 (0x111), Fraccion = 0.4375 * 16 = 7 (exacto, sin redondeo)
    // Combinado en el registro BRR: 0x1117
    ADDR_USART2_BRR->DIV_Mantissa = 273;
    ADDR_USART2_BRR->DIV_Fraction = 7;
 
    // 4. Habilitar USART2 (Bit 13: UE), Transmisor TX (Bit 3: TE) y Receptor RX (Bit 2: RE)
    ADDR_USART2_CR1->UE = 1;
    ADDR_USART2_CR1->TE = 1;
    ADDR_USART2_CR1->RE = 1;
}
 
//envia un solo caracter por sondeo (polling), uso interno de este archivo
static void Comunicacion_EnviarCaracter(char c) {
    while (!(ADDR_USART2_SR->TXE)); //esperar a que el buffer de transmision este vacio
    ADDR_USART2_DR->DR = c;
}
 
//envia una cadena completa, caracter por caracter, hasta encontrar el \0
void Comunicacion_EnviarCadena(char *str) {
    while (*str) Comunicacion_EnviarCaracter(*str++);
}
 
