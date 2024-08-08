/**
 ******************************************************************************
 * @file           : main.c
 * @author         : laurasofia
 * @brief          : Configuración básica de un proyecto.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32_assert.h"
#include "gpio_driver_hal.h"

//Definición de pines a utilizar
GPIO_Handler_t userLed      = {0}; //PinA5
GPIO_Handler_t userLed01    = {0}; //PinH1

/*  Main function  */
int main(void)
{
	//VERIFIFICACIÓN DE BasicConfig del UpdateGPIO

	/* Configuramos el pin A5 */
	userLed.pGPIOx                         = GPIOA;
	userLed.pinConfig.GPIO_PinNumber       = PIN_5;
	userLed.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	userLed.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	userLed.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
	userLed.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&userLed);

	//Ejecutamos la configuración realizada del pin A5
	//gpio_WritePin(&userLed, SET);

	/* Configuramos el pin H1*/
		userLed01.pGPIOx                         = GPIOH;
		userLed01.pinConfig.GPIO_PinNumber       = PIN_1;
		userLed01.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		userLed01.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		userLed01.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		userLed01.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&userLed01);

		//Ejecutamos la configuración realizada del pin H1
		//gpio_WritePin(&userLed01, SET);

    /* Loop forever */
	while(1){

		//Llamamos función TooglePin para pin A5
		//gpio_TooglePin(&userLed);
		//for (uint32_t j=0; j < 200000; j++){
			//__NOP();
		//}

		//Llamamos función TooglePin para pin H1
		gpio_TooglePin(&userLed01);
		for (uint32_t j=0; j < 200000; j++){
			__NOP();
		}

	}

	return 0;
}
