#include "pantalla.h"

// Registros de I2C1 y GPIOB
#define I2C1_BASE       0x40005400UL
#define I2C1_CR1        (*((volatile uint32_t *)(I2C1_BASE + 0x00)))
#define I2C1_CR2        (*((volatile uint32_t *)(I2C1_BASE + 0x04)))
#define I2C1_DR         (*((volatile uint32_t *)(I2C1_BASE + 0x10)))
#define I2C1_SR1        (*((volatile uint32_t *)(I2C1_BASE + 0x14)))
#define I2C1_SR2        (*((volatile uint32_t *)(I2C1_BASE + 0x18)))
#define I2C1_CCR        (*((volatile uint32_t *)(I2C1_BASE + 0x1C)))
#define I2C1_TRISE      (*((volatile uint32_t *)(I2C1_BASE + 0x20)))

#define RCC_AHB1ENR     (*((volatile uint32_t *)0x40023830))
#define RCC_APB1ENR     (*((volatile uint32_t *)0x40023840))

#define GPIOB_MODER     (*((volatile uint32_t *)0x40020400))
#define GPIOB_OTYPER    (*((volatile uint32_t *)0x40020404))
#define GPIOB_OSPEEDR   (*((volatile uint32_t *)0x40020408))
#define GPIOB_PUPDR     (*((volatile uint32_t *)0x4002040C))
#define GPIOB_AFRL      (*((volatile uint32_t *)0x40020420))

extern void Delay_ms(uint32_t ms);

static void I2C1_EscribirByte(uint8_t addr, uint8_t data) {
    while(I2C1_SR2 & (1 << 1));
    I2C1_CR1 |= (1 << 8);
    while(!(I2C1_SR1 & (1 << 0)));
    I2C1_DR = addr;
    while(!(I2C1_SR1 & (1 << 1)));
    uint32_t temp = I2C1_SR1;
    temp = I2C1_SR2;
    (void)temp;
    while(!(I2C1_SR1 & (1 << 7)));
    I2C1_DR = data;
    while(!(I2C1_SR1 & (1 << 2)));
    I2C1_CR1 |= (1 << 9);
}

static void LCD_Enviar(uint8_t data, uint8_t rs) {
    uint8_t data_u = (data & 0xF0);
    uint8_t data_l = ((data << 4) & 0xF0);
    uint8_t backlight = 0x08;
    uint8_t trama[4];
    trama[0] = data_u | backlight | 0x04 | rs;
    trama[1] = data_u | backlight | rs;
    trama[2] = data_l | backlight | 0x04 | rs;
    trama[3] = data_l | backlight | rs;
    for(int i = 0; i < 4; i++) {
        I2C1_EscribirByte(LCD_ADDR, trama[i]);
    }
}

void Pantalla_Comando(uint8_t cmd) { LCD_Enviar(cmd, 0); }
void Pantalla_Caracter(uint8_t data) { LCD_Enviar(data, 1); }

void Pantalla_Init(void) {
    RCC_AHB1ENR |= (1 << 1);
    RCC_APB1ENR |= (1 << 21);

    // PB6(SCL) y PB7(SDA)
    GPIOB_MODER &= ~((3UL << 12) | (3UL << 14));
    GPIOB_MODER |= ((2UL << 12) | (2UL << 14));
    GPIOB_OTYPER |= (1 << 6) | (1 << 7);
    GPIOB_OSPEEDR |= (3 << 12) | (3 << 14);
    GPIOB_PUPDR &= ~((3UL << 12) | (3UL << 14));
    GPIOB_PUPDR |= ((1UL << 12) | (1UL << 14));
    GPIOB_AFRL &= ~((0xFUL << 24) | (0xFUL << 28));
    GPIOB_AFRL |= ((4UL << 24) | (4UL << 28));

    // I2C a 100kHz
    I2C1_CR1 |= (1 << 15);
    I2C1_CR1 &= ~(1 << 15);
    I2C1_CR2 = 42;
    I2C1_CCR = 210;
    I2C1_TRISE = 43;
    I2C1_CR1 |= (1 << 0);
    Delay_ms(50);

    Pantalla_Comando(0x33); Delay_ms(5);
    Pantalla_Comando(0x32); Delay_ms(5);
    Pantalla_Comando(0x28); Delay_ms(1);
    Pantalla_Comando(0x0C); Delay_ms(1);
    Pantalla_Comando(0x01); Delay_ms(2);
    Pantalla_Comando(0x06); Delay_ms(1);
}

void Pantalla_Cadena(char *str) {
    while (*str) Pantalla_Caracter(*str++);
}

void Pantalla_Cursor(uint8_t fila, uint8_t columna) {
    Pantalla_Comando((fila == 0) ? (0x80 + columna) : (0xC0 + columna));
}

void Pantalla_Limpiar(void) {
    Pantalla_Comando(0x01); Delay_ms(2);
}
