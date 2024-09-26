/**
 ******************************************************************************
 * @file           : main.c
 * @author         : labravo (Laura Sofia Bravo Revelo)
 * @brief          : Configuración básica PWM.
 ******************************************************************************
 */

#include <stdint.h>
#include <string.h>
#include "stm32f4xx.h"
#include "stm32_assert.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"
#include "pwm_driver_hal.h"

//Definición de los handlers necesarios para el led de estado
GPIO_Handler_t   blinkyPin       = {0};
Timer_Handler_t  blinkyTimer     = {0};

//Elementos para el PWM
GPIO_Handler_t   pinPWMChannel   = {0};
PWM_Handler_t    signalPWM       = {0};

uint16_t   duttyValue    	     = 0;

//Definición de cabeceras de las funciones main
void initialSystem(void);


/*  Main function  */
int main(void)
{

	initialSystem();

    /* Loop forever */
	while(1){


	}
}

void initialSystem(void){

	/* Configuramos el pin H1*/
	blinkyPin.pGPIOx                         = GPIOH;
	blinkyPin.pinConfig.GPIO_PinNumber       = PIN_1;
	blinkyPin.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	blinkyPin.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	blinkyPin.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
	blinkyPin.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&blinkyPin);

	//Ejecutamos la configuración realizada en H1
	//gpio_WritePin(&blinkyPIn, SET);


	/* Configuramos el timer del blinky*/
	blinkyTimer.pTIMx                             = TIM2;
	blinkyTimer.TIMx_Config.TIMx_Prescaler        = 16000;  //Genera incrementos de 1 ms
	blinkyTimer.TIMx_Config.TIMx_Period           = 1000;     //De la mano con el prescaler, genera int ada 500 ms
	blinkyTimer.TIMx_Config.TIMx_mode             = TIMER_UP_COUNTER;
	blinkyTimer.TIMx_Config.TIMx_InterruptEnable  = TIMER_INT_ENABLE;

	/* Configuramos el Timer */
	timer_Config(&blinkyTimer);

	//Encendemos el Timer
	timer_SetState(&blinkyTimer, TIMER_ON);

	/*Configuración PWM*/
	pinPWMChannel.pGPIOx                        = GPIOC;
	pinPWMChannel.pinConfig.GPIO_PinNumber      = PIN_7;
	pinPWMChannel.pinConfig.GPIO_PinMode        = GPIO_MODE_ALTFN;
	pinPWMChannel.pinConfig.GPIO_PinOutputType  = GPIO_OTYPE_PUSHPULL;
	pinPWMChannel.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	pinPWMChannel.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	pinPWMChannel.pinConfig.GPIO_PinAltFunMode  = AF2;

	gpio_Config(&pinPWMChannel);

	/*Configuración timer para generar señal pwm*/
	signalPWM.ptrTIMx                = TIM3;
	signalPWM.config.channel         = PWM_CHANNEL_2;
	signalPWM.config.duttyCicle      = 5;
	signalPWM.config.prescaler       = 1600;
	signalPWM.config.periodo         = 10; //Periodo es de 0.001s

	pwm_Config(&signalPWM);
	pwm_Enable_Output(&signalPWM);
	pwm_Start_Signal(&signalPWM);


}

/*
 * Overwrite function for A5
// * */
void Timer2_Callback(void){
	gpio_TooglePin(&blinkyPin);
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
