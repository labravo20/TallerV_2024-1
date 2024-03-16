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


#endif /* STM32F4XX_HAL_H_ */
