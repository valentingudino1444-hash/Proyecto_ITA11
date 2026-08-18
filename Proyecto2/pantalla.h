#ifndef PANTALLA_H_
#define PANTALLA_H_

#include <stdint.h>

#define LCD_ADDR 0x4E // O probar con 0x7E si no muestra nada

void Pantalla_Init(void);
void Pantalla_Comando(uint8_t cmd);
void Pantalla_Caracter(uint8_t data);
void Pantalla_Cadena(char *str);
void Pantalla_Cursor(uint8_t fila, uint8_t columna);
void Pantalla_Limpiar(void);

#endif /* PANTALLA_H_ */
