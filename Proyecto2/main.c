#include <stdio.h> // Se incluye la librería estándar de entrada y salida para el manejo de flujos de datos.
#include <stdint.h> // Se incluye la librería de tipos de datos enteros de tamaño fijo.
#include "sensores.h" // Se incluye la librería desarrollada por Diego para la gestión del conversor analógico-digital.
#include "actuadores.h" // Se incluye la librería encargada del control de PWM y actuadores de potencia.
#include "pantalla.h" // Se incluye la librería responsable de la interfaz gráfica en la pantalla LCD.
#include "comunicacion.h" // Se incluye la librería orientada a la transmisión de datos por comunicación serial UART.

// Se definen los registros de la Macrocell de Instrumentación de Trazas (ITM) para la depuración en entorno de desarrollo.
#define ITM_STIM_U8   (*((volatile uint8_t  *)0xE0000000)) // Se define el puerto de estímulo ITM de 8 bits.
#define ITM_STIM_U32  (*((volatile uint32_t *)0xE0000000)) // Se define el puerto de estímulo ITM de 32 bits.
#define ITM_TER       (*((volatile uint32_t *)0xE0000E00)) // Se define el registro de habilitación de estímulos de traza.
#define ITM_TCR       (*((volatile uint32_t *)0xE0000E80)) // Se define el registro de control de traza.
#define SCB_CPACR     (*((volatile uint32_t *)0xE000ED88)) // Se define el registro de control de acceso coprocesador para la FPU.

int _write(int file, char *ptr, int len) { // Se define la función de redirección de escritura para la consola de depuración.
    for (int i = 0; i < len; i++) { // Se ejecuta un bucle iterativo para recorrer cada uno de los caracteres del búfer.
        if ((ITM_TCR & 1) && (ITM_TER & 1)) { // Se verifica si el sistema de rastreo ITM se encuentra activo y habilitado.
            while (ITM_STIM_U32 == 0); // Se retiene el flujo de ejecución mientras el registro de estímulo permanezca ocupado.
            ITM_STIM_U8 = (uint8_t)ptr[i]; // Se escribe el carácter actual en el registro de estímulo para su visualización.
        }
    }
    return len; // Se retorna la longitud total de los datos procesados.
}

// Se define la función de retardo en milisegundos recalibrada para la frecuencia del sistema de 168 MHz.
void Delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < (ms * 42000); i++) { // Se ejecuta un ciclo de retardo mediante instrucciones nulas (nop).
        __asm("nop"); // Se inserta una instrucción ensambladora sin operación para consumir ciclos de reloj.
    }
}



int main(void) {
    // Acelera la tarjeta a 168 MHz para que el UART y los retardos cuadren
    SystemClock_Config_168MHz();

    // 1. Habilitar cálculos matemáticos de hardware (FPU)
    SCB_CPACR |= (0xF << 20);
    __asm volatile ("dsb");
    __asm volatile ("isb");

    // 2. Inicialización General de Módulos
    Sensores_Init();
    Actuador_PWM_Init();
    Pantalla_Init();
    Comunicacion_Init();

    printf("--- Control de Incubadora Iniciado ---\r\n");
    Comunicacion_EnviarCadena("--- Sistema de Incubadora STM32 Iniciado ---\r\n");
