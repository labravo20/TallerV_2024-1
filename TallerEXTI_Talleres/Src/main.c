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
#include "exti_driver_hal.h"

//Definimos los pines que se van a utilizar
GPIO_Handler_t userLed       = {0};//Pin A5
GPIO_Handler_t userLed1      = {0};//Pin C6
GPIO_Handler_t userLed2      = {0};//Pin A7
GPIO_Handler_t userLed3      = {0};//Pin C8
GPIO_Handler_t userSwitch    = {0};//Pin A10
GPIO_Handler_t userData      = {0};//Pin B5
GPIO_Handler_t userSWenc     = {0};//Pin B3
GPIO_Handler_t userCKenc     = {0};//Pin B13

//Definimos los timers que se emplean
Timer_Handler_t blinkTimer   = {0}; //Este timer controla el led blink

//Definición lineas EXTI
EXTI_Config_t swExti    = {0};
EXTI_Config_t ckExti    = {0};

//Variables auxiliares que ayudarán al código
uint8_t dir0      = {0};
uint8_t dir1      = {0};
uint8_t dirR      = {0};
uint8_t numero    = {0};

//Creación de un enum con los cuatro casos
enum{

};

/*  Main function  */
int main(void)
{
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
		//gpio_WritePin(&userLed01, SET);

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
