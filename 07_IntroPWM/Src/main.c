/**
 ******************************************************************************
 * @file           : main.c
 * @author         : labravo (Laura Sofia Bravo Revelo)
 * @brief          : Configuración básica commSerial.
 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32_assert.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"
#include "adc_driver_hal.h"

//Definimos pin de prueba
GPIO_Handler_t userLed    = {0}; //PinA5

//Blinky timer
Timer_Handler_t blinkTimer = {0};

//Variables a usar
uint16_t conversionValue    = 0;
uint8_t  banderaTimer       = 0;

//Definimos ADC channel a usar
ADC_Config_t adcValue       = {0};


//Definición de las cabeceras de las funciones del main
void initialSystem(void);


/*  Main function  */
int main(void)
{
	//Inicialización de los elementos del sistema
	initialSystem();

    /* Loop forever */
	while(1){

		if(banderaTimer == 1){
			banderaTimer = 0;
			adc_StartSingleConv();
		}

	}

}

//Definimos función para configuración inicial
void initialSystem(void){

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


	/* Configuramos el timer del blinky*/
	blinkTimer.pTIMx                             = TIM2;
	blinkTimer.TIMx_Config.TIMx_Prescaler        = 16000;  //Genera incrementos de 1 ms
	blinkTimer.TIMx_Config.TIMx_Period           = 1000;     //De la mano con el prescaler, genera int ada 500 ms
	blinkTimer.TIMx_Config.TIMx_mode             = TIMER_UP_COUNTER;
	blinkTimer.TIMx_Config.TIMx_InterruptEnable  = TIMER_INT_ENABLE;

	/* Configuramos el Timer */
	timer_Config(&blinkTimer);

	//Encendemos el Timer
	timer_SetState(&blinkTimer, TIMER_ON);

	/* Configuramos ADC*/
	adcValue.channel             = CHANNEL_0;
	adcValue.resolution          = RESOLUTION_12_BIT;
	adcValue.dataAlignment       = ALIGNMENT_RIGHT;
	adcValue.interrupState       = ADC_INT_ENABLE;
	adcValue.samplingPeriod      = SAMPLING_PERIOD_84_CYCLES;

	adc_ConfigSingleChannel(&adcValue);
	adc_peripheralOnOFF(ADC_ON);


}

/*
 * Overwrite function for A5
 * */
void Timer2_Callback(void){
	gpio_TooglePin(&userLed);
	banderaTimer = 1;
}


void adc_CompleteCallback(void){
	conversionValue = adc_Get_Value();
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
