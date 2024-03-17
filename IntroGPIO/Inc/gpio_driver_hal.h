/*
 * gpio_driver_hal.h
 *
 *  Created on: 17/03/2024
 *      Author: laurasofia
 */

#ifndef GPIO_DRIVER_HAL_H_
#define GPIO_DRIVER_HAL_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"

/* Descripcion de cada uno de los registros del periferico (no es necesario para este) */

/*
 * GPIO bit SET and bit RESET enumeration
 */

enum
{
	GPIO_PIN_RESET = 0,
	GPIO_PIN_SET
};

/* Valores estandar para las configuraciones */
/* 8.4.1 GPIOx_MODER (dos bit por cada PIN) */
enum
{
	GPIO_MODE_IN = 0,
	GPIO_MODE_OUT,
	GPIO_MODE_ALTFN,
	GPIO_MODE_ANALOG
};

/* 8.4.2 GPIOx_OTYPER (un bit por PIN) */
enum
{
	GPIO_OTYPE_PUSHPULL = 0,
	GPIO_OTYPE_OPENDRAIN
};

/* 8.4.3 GPIOx_OSPEEDR (dos bit por cada PIN) */
enum
{
	GPIO_OSPEED_LOW = 0,
	GPIO_OSPEED_MEDIUM,
	GPIO_OSPEED_FAST,
	GPIO_OSPEED_HIGH
};

/* 8.4.4 GPIOx_PUPDR (dos bit por cada PIN) */
enum
{
	GPIO_PUPDR_NOTHING = 0,
	GPIO_PUPDR_PULLUP,
	GPIO_PUPDR_PULLDOWN,
	GPIO_PUPDR_RESERVED //IMPORTANTE: Lleva coma el último término o no???
};

/* 8.4.5 GPIOx_IDR (un bit por PIN) - este es el registro para leer el estado de un PIN*/

/* 8.4.6 GPIOx_ODR (un bit por PIN) - este es el registro para escribir el estado de un
 * PIN (1 o 0). Este registro puede ser escrito y leido desde el sofware, pero no garantiza
 * una escritura "atomica", por lo cual es preferible utilizar el registro BSRR */

/* Definicion de los nombres de los pines */
enum
{
	PIN_0 = 0,
	PIN_1,
	PIN_2,
	PIN_3,
	PIN_4,
	PIN_5,
	PIN_6,
	PIN_7,
	PIN_8,
	PIN_9,
	PIN_10,
	PIN_11,
	PIN_12,
	PIN_13,
	PIN_14,
	PIN_15
};



#endif /* GPIO_DRIVER_HAL_H_ */
