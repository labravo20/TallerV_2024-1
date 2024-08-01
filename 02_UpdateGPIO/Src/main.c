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

#include <stm32f4xx.h>
#include <stdint.h>
#include <stdio.h>
#include "Ejemplo.h"

//Definicion constantes

#define RCC_BASE_ADDRESS      0x40023800UL
#define RCC_AHB1ENR_OFFSET    0x30
#define RCC_AHB1ENR           (RCC_BASE_ADDRESS + RCC_AHB1ENR_OFFSET)

#define GPIOA_BASE_ADDRESS    0x40020000UL
#define GPIOA_MODE_REG_OFFSET 0x00
#define GPIOA_MODE_ODR_OFFSET 0x14
#define GPIOA_MODE_REG        (GPIOA_BASE_ADDRESS + GPIOA_MODE_REG_OFFSET)
#define GPIOA_OPD_REG         (GPIOA_BASE_ADDRESS + GPIOA_MODE_ODR_OFFSET)

//Definicion variables
BasicExample ejemploClase = {0};
BasicExample datosEjemplo = {0};

//uint32_t *registerAHB1enb;
unsigned int day;

int main(void)
{

	uint32_t *registerAHB1enb = (uint32_t *)RCC_AHB1ENR;
	//uint32_t *registerAHB1enb = (uint32_t *)0x40023830UL;

	//ESTAMOS ENCENDIENDO LA SEÑAL DE RELOJ PARA PUERTOS ESPECÍFICOS
	*registerAHB1enb |= (1 << 2); //Activando la señal de reloj para el puerto GPIOc
	*registerAHB1enb |= (1 << 0); //Activando la señal de reloj para el puerto GPIOA

	uint32_t *registerGPIOA_MODE = (uint32_t *)GPIOA_MODE_REG;
	*registerGPIOA_MODE |= (1 << 10);

	uint32_t *registerGPIOA_ODR = (uint32_t *)GPIOA_OPD_REG;
	*registerGPIOA_ODR |= (1 << 5); //LED2 (green) set

	*registerGPIOA_ODR &= ~(1 << 5); //LED2 (green) off
	*registerGPIOA_ODR |= (0 << 5); //LED2 (green) off

	ejemploClase.ID = 'w';
	ejemploClase.counterUp = 100;
	ejemploClase.dummy = 25;

	datosEjemplo.ID = 'f';
	datosEjemplo.dummy = 28;
	datosEjemplo.timestamp = 0x1234;
	//Elemento tipo float: cuando estamos trabajando con elementos de punto flotante se anexa la "f"
	datosEjemplo.promedio = 32.45f;

	for(datosEjemplo.counterUp=0;datosEjemplo.counterUp < ejemploClase.dummy;datosEjemplo.counterUp++){
		datosEjemplo.promedio += 0.25f;
		ejemploClase.counterUp -=2;
	}

    /* Loop forever */
	while(1){

	}

	return 0;
}
