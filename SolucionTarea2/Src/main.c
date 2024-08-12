/**
 ******************************************************************************
 * @file           : main.c
 * @author         : laurasofia
 * @brief          : Configuración básica de un proyecto.
 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32_assert.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"

//Definimos pin de prueba
GPIO_Handler_t userLed    = {0}; //PinA5
GPIO_Handler_t userLed01    = {0}; //PinH1

Timer_Handler_t blinkTimer = {0};

/*  Main function  */
int main(void)
{
	//VERIFICACIÓN DE FUNCIONAMIENTO CONFIGURACIÓN DE DRIVERS
	/* Configuramos el pin A5*/
	userLed.pGPIOx                         = GPIOA;
	userLed.pinConfig.GPIO_PinNumber       = PIN_5;
	userLed.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	userLed.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	userLed.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
	userLed.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&userLed);

	//Ejecutamos la configuración realizada en A5
	//gpio_WritePin(&userLed, SET);

	/* ========== SOLUCIÓN TAREA 2 ========== */

	/* 1) A continuación se realiza la configuración del led de estado (este está ubicado en
	 * la "board táctica")*/
	/* Configuramos el pin H1 --> LED DE ESTADO*/
	userLed01.pGPIOx                         = GPIOH;
	userLed01.pinConfig.GPIO_PinNumber       = PIN_1;
	userLed01.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	userLed01.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	userLed01.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
	userLed01.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&userLed01);

	//Ejecutamos la configuración realizada en H1
	gpio_WritePin(&userLed01, SET);

	blinkTimer.pTIMx                             = TIM2;
	blinkTimer.TIMx_Config.TIMx_Prescaler        = 16000;  //Genera incrementos de 1 ms
	blinkTimer.TIMx_Config.TIMx_Period           = 250;     //De la mano con el prescaler...
	blinkTimer.TIMx_Config.TIMx_mode             = TIMER_UP_COUNTER;
	blinkTimer.TIMx_Config.TIMx_InterruptEnable  = TIMER_INT_ENABLE;

	/* Configuramos el Timer */
	timer_Config(&blinkTimer);

	//Encendemos el Timer
	timer_SetState(&blinkTimer, TIMER_ON);

    /* Loop forever */
	while(1){


	}

}

/*
 * Overwrite function for A5
 * */
//void Timer2_Callback(void){
	//gpio_TooglePin(&userLed);
//}

/*
 * Overwrite function for H1
 * */
void Timer2_Callback(void){
	gpio_TooglePin(&userLed01);
}

/*
 * Esta función sirve para detectar problemas de parámetros
 * incorrectos al momento de ejecutar un programa.
 * */
void assert_failed(uint8_t* file, uint32_t line){
	while(1){
		//Problems...
	}
}
