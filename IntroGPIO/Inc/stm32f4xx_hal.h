/*
 * stm32f4xx_hal.h
 *
 *  Created on: Mar 14, 2024
 *      Author: laurasofia
 *
 *  Este archivo contiene la informacion mas basica del MCU:
 *  - Valores del reloj principal
 *  - Distribucion basica de la memoria (descrito en la figura 14 de la hoja
 *  de datos del MCU)
 *  - Posiciones de memoria de los perifericos disponibles en el micro descrito
 *  en la tabla 1 (Memory Map)
 *  - Incluir los demas drivers en los perifericos
 *  - Definiciones de las constantes mas basicas
 *
 *  NOTA: La definicion del NVIC sera realizada al momento de describir el uso
 *  de las interrupciones
 */

#ifndef STM32F4XX_HAL_H_
#define STM32F4XX_HAL_H_

#include <stdint.h>
#include <stddef.h>

#define HSI_CLOCK_SPEED    8000000  //Value for the main clock signal (HSI -> High Speed Internal)
#define HSE_CLOCK_SPEED    16000000 //Value for the main clock signal (HSE -> High Speed External)

///NOTA IMPORTANTE!!! Aquí usamos 1 espacio ( _ ) o 2 ( __ ) ?

//#define NOP()    (__asm__("NOP"))
#define NOP()      asm("NOP")
#define __weak      __attribute__((weak))

/*
 * Base addresses of Flash and SRAM memories
 * Datasheet, Memory Map, Figure 14
 * (Remember, 1KByte = 1024 bytes)
 */

#define FLASH_BASE_ADDR    0x08000000U     //Esta es la memoria del programa, 512 KB.
#define SRAM_BASE_ADDR     0x20000000U     //ESta es la memoria RAM, 128KB.

/* NOTA: Observar que existen unos registros especificos del Cortex M4 en la region 0xE0000000U
 * Los controladores de las interrupciones se encuentran allí, por ejemplo. Esto se vera a su
 * debido tiempo.
 */

/*NOTA:
 * Ahora agregamos la direccon de memoria base para cada uno de los perifericos que posee el micro
 * En el "datasheet" del micro. Figura 14 (Memory Map) encontramos el mapa de los buses:
 * - APB1 (Advance Peripheral Bus)
 * - APB2
 * - AHB1 (Advance High.performance Bus
 * - AHB2
 */

/*
 * AHBx and APBx Bus Peripherals base addresses
 */
#define APB1_BASE_ADDR    0x40000000U
#define APB2_BASE_ADDR    0x40010000U
#define AHB1_BASE_ADDR    0x40020000U
#define AHB2_BASE_ADDR    0x50000000U

/*
 * Y ahora debemos hacer lo mismo pero cada una de las posiciones de memoria de cada uno de los
 * perifericos descritos en la Tabla 1 del manual de referencia del micro.
 * Observe que en dicha tabla esta a su vez dividida en cuatro segmentos, cada uno correspondiente
 * a APB1, APB2, AHB1, AHB2.
 *
 * Comenzar de arriba hacia abajo como se muestra en la tabla. Inicia USB_OTG_FS (AHB2)
 */


#endif /* STM32F4XX_HAL_H_ */
