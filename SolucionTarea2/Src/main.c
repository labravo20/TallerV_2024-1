/**
 ******************************************************************************
 * @file           : main.c
 * @author         : laurasofia
 * @brief          : Solución Tarea 2
 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32_assert.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"

//Definimos pin de prueba
GPIO_Handler_t userLed      = {0}; //PinA5
GPIO_Handler_t userLed00    = {0}; //PinH1
GPIO_Handler_t userLed01    = {0}; //PinC12 (led "a")
GPIO_Handler_t userLed02    = {0}; //PinA12 (led "b")
GPIO_Handler_t userLed03    = {0}; //PinC6  (led "c")
GPIO_Handler_t userLed04    = {0}; //PinB13 (led "d")
GPIO_Handler_t userLed05    = {0}; //PinB10 (led "e")
GPIO_Handler_t userLed06    = {0}; //PinB7  (led "f")
GPIO_Handler_t userLed07    = {0}; //PinC10 (led "g")
GPIO_Handler_t vcc_uni      = {0}; //PinA0
GPIO_Handler_t vcc_dec      = {0}; //PinB12

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

	/* A continuación se realiza la configuración del led de estado (este está ubicado en
	 * la "board táctica")*/

	/* Configuramos el pin H1 --> LED DE ESTADO*/
	userLed00.pGPIOx                         = GPIOH;
	userLed00.pinConfig.GPIO_PinNumber       = PIN_1;
	userLed00.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	userLed00.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	userLed00.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
	userLed00.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&userLed00);

	//Ejecutamos la configuración realizada en H1
	//gpio_WritePin(&userLed00, SET);

	blinkTimer.pTIMx                             = TIM2;
	blinkTimer.TIMx_Config.TIMx_Prescaler        = 16000;  //Genera incrementos de 1 ms
	blinkTimer.TIMx_Config.TIMx_Period           = 250;     //De la mano con el prescaler...
	blinkTimer.TIMx_Config.TIMx_mode             = TIMER_UP_COUNTER;
	blinkTimer.TIMx_Config.TIMx_InterruptEnable  = TIMER_INT_ENABLE;

	/* Configuramos el Timer */
	timer_Config(&blinkTimer);

	//Encendemos el Timer
	timer_SetState(&blinkTimer, TIMER_ON);

	/* A continuación se empieza con la configuración de los pines seleccionados para
	 * activar los LEDs de la cuenta respectiva en el 7 segmentos */

	/* Configuramos el pin C12 --> LED a*/
	userLed01.pGPIOx                         = GPIOC;
	userLed01.pinConfig.GPIO_PinNumber       = PIN_12;
	userLed01.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	userLed01.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	userLed01.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
	userLed01.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&userLed01);

	/* Configuramos el pin A12 --> LED b*/
	userLed02.pGPIOx                         = GPIOA;
	userLed02.pinConfig.GPIO_PinNumber       = PIN_12;
	userLed02.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	userLed02.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	userLed02.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
	userLed02.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&userLed02);

	/* Configuramos el pin C6 --> LED c*/
	userLed03.pGPIOx                         = GPIOC;
	userLed03.pinConfig.GPIO_PinNumber       = PIN_6;
	userLed03.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	userLed03.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	userLed03.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
	userLed03.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&userLed03);

	/* Configuramos el pin B13 --> LED d*/
	userLed04.pGPIOx                         = GPIOB;
	userLed04.pinConfig.GPIO_PinNumber       = PIN_13;
	userLed04.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	userLed04.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	userLed04.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
	userLed04.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&userLed04);

	/* Configuramos el pin B10 --> LED e*/
	userLed05.pGPIOx                         = GPIOB;
	userLed05.pinConfig.GPIO_PinNumber       = PIN_10;
	userLed05.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	userLed05.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	userLed05.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
	userLed05.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&userLed05);

	/* Configuramos el pin B7 --> LED f*/
	userLed06.pGPIOx                         = GPIOB;
	userLed06.pinConfig.GPIO_PinNumber       = PIN_7;
	userLed06.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	userLed06.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	userLed06.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
	userLed06.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&userLed06);

	/* Configuramos el pin C10 --> LED g*/
	userLed07.pGPIOx                         = GPIOC;
	userLed07.pinConfig.GPIO_PinNumber       = PIN_10;
	userLed07.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	userLed07.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	userLed07.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
	userLed07.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&userLed07);

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
	gpio_TooglePin(&userLed00);
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
