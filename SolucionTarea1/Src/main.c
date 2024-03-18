/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
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
#include "stm32f4xx_hal.h"
#include "stm32_assert.h"
#include "gpio_driver_hal.h"

//Headers definition
int add(int x, int y);

//Definimos un Pin de prueba
GPIO_Handler_t userLed = {0}; //PinA5

/*
 * The main function, where everything happens
 */
int main(void)
{
    /* Configuramos el pin */
	userLed.pGPIOx                        = GPIOA;
	userLed.pinConfig.GPIO_PinNumber      = PIN_5;
	userLed.pinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	userLed.pinConfig.GPIO_PinOutputType  = GPIO_OTYPE_PUSHPULL;
	userLed.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
	userLed.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	/* Cargamos la configuracion en los registros que gobiernan el puerto */
	gpio_Config(&userLed);

	gpio_WritePin(&userLed,SET);

	/* ++++====== PRIMER PUNTO ======++++ */

	//Llamamos a la función que se desea analizar
	gpio_ReadPin(&userLed);

	/* Loop forever */
	while(1){

	}
}

/*
 * Esta funcion sirve para detectar problemas de parametros
 * incorrectos al momento de ejecutar el programa
 */
void assert_failed(uint8_t* file, uint32_t line){
	while(1){
		//PROBLEMS...
	}
}
