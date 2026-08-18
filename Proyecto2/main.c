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

  Pantalla_Cursor(0, 0);
    Pantalla_Cadena("Iniciando...");
    Delay_ms(1500);
    Pantalla_Limpiar();

    float Kp = 50.0f; // Ganancia para el control del TRIAC

    // --- BUFFERS AMPLIADOS PARA EVITAR DESBORDAMIENTOS ---
    char buffer_lcd[32];    // Sobrado para los 16 caracteres de la pantalla
    char buffer_serial[128]; // ¡AQUÍ ESTÁ LA SOLUCIÓN! Sobrado para la cadena larga de PC

    while (1) {
        // --- A) LECTURA (En los pines PA4 y PA5) ---
        uint16_t adc_pot = ADC_Leer_Canal(4);  // PA4 (Set-Point)
        uint16_t adc_lm35 = ADC_Leer_Canal(5); // PA5 (Sensor LM35 simulado)

        // --- B) PROCESAMIENTO ---
        float T_set = 20.0f + ((float)adc_pot / 4095.0f) * 40.0f; // Mapeo de 20 C a 60 C
        float voltaje_lm35 = ((float)adc_lm35 * 3.3f) / 4095.0f;
        float T_real = voltaje_lm35 * 100.0f; // Mapeo de 0 C a 330 C

        // --- C) CONTROL PROPORCIONAL ---
        float error = T_set - T_real;
        if (error < 0) error = 0;
float pwm_flotante = Kp * error;
        if (pwm_flotante > PWM_MAX_DUTY) pwm_flotante = PWM_MAX_DUTY;

        // --- D) ACTUADOR (Tren de ondas a 1 Hz) ---
        PWM_Ajustar_Foco((uint32_t)pwm_flotante);

        // --- E) PANTALLA LCD ---
        sprintf(buffer_lcd, "Set : %.1f C   ", T_set);
        Pantalla_Cursor(0, 0);
        Pantalla_Cadena(buffer_lcd);

        sprintf(buffer_lcd, "Real: %.1f C   ", T_real);
        Pantalla_Cursor(1, 0);
        Pantalla_Cadena(buffer_lcd);

        // --- F) SALIDA FTDI / HERCULES ---
        sprintf(buffer_serial, "Temperatura Set: %.1f C | Temperatura Real: %.1f C | PWM: %lu\r\n", T_set, T_real, (uint32_t)pwm_flotante);
        Comunicacion_EnviarCadena(buffer_serial);

        // También lo mandamos a la consola ITM de CubeIDE
        printf("%s", buffer_serial);

        Delay_ms(250);
    }
