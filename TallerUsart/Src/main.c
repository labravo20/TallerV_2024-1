/**
 ******************************************************************************
 * @file           : main.c
 * @author         : labravo (Laura Sofia Bravo Revelo)
 * @brief          : Configuración básica commSerial.
 ******************************************************************************
 */

#include <stdint.h>
#include <string.h>
#include "stm32f4xx.h"
#include "stm32_assert.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"
#include "exti_driver_hal.h"
#include "usart_driver_hal.h"

//Definimos pin de prueba
GPIO_Handler_t userLed        = {0}; //PinA5
GPIO_Handler_t stateLed       = {0}; //PinH1
GPIO_Handler_t functionLed    = {0}; //

//Blinky timer
Timer_Handler_t blinkTimer = {0};

//Comunicación RS-232 con el PC, ya habilitada en la board del Nucleo
//Utiliza la conexión USB
USART_Handler_t   commSerial   = {0};
GPIO_Handler_t    pinTx        = {0};
GPIO_Handler_t    pinRx        = {0};
uint8_t           sendMsg      =  0 ;
char bufferData[64]            = {0};


//Definición de las cabeceras de las funciones del main
void initialSystem(void);


/*  Main function  */
int main(void)
{
	//Inicialización de los elementos del sistema
	initialSystem();

    /* Loop forever */
	while(1){

		if(sendMsg){
			sendMsg = 0;
			usart_writeMsg(&commSerial, "Hola Mundo!!\n\r");
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
	gpio_WritePin(&userLed, SET);

	/* Configuramos el pin H1 --> LED DE ESTADO*/
	stateLed.pGPIOx                         = GPIOH;
	stateLed.pinConfig.GPIO_PinNumber       = PIN_1;
	stateLed.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	stateLed.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	stateLed.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
	stateLed.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&stateLed);

	//Ejecutamos la configuración realizada en H1
	gpio_WritePin(&stateLed, SET);

	/* Configuramos el timer del blinky*/
	blinkTimer.pTIMx                             = TIM2;
	blinkTimer.TIMx_Config.TIMx_Prescaler        = 16000;  //Genera incrementos de 1 ms
	blinkTimer.TIMx_Config.TIMx_Period           = 500;     //De la mano con el prescaler, genera int ada 500 ms
	blinkTimer.TIMx_Config.TIMx_mode             = TIMER_UP_COUNTER;
	blinkTimer.TIMx_Config.TIMx_InterruptEnable  = TIMER_INT_ENABLE;

	/* Configuramos el Timer */
	timer_Config(&blinkTimer);

	//Encendemos el Timer
	timer_SetState(&blinkTimer, TIMER_ON);

	/* Configuramos los pines del puerto serial*/

	/* Pin sobre los que funciona el USART2 (TX)*/
	pinTx.pGPIOx                          = GPIOA;
	pinTx.pinConfig.GPIO_PinNumber        = PIN_2;
	pinTx.pinConfig.GPIO_PinMode          = GPIO_MODE_ALTFN;
	pinTx.pinConfig.GPIO_PinAltFunMode    = AF7;
	pinTx.pinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
	pinTx.pinConfig.GPIO_PinOutputSpeed   = GPIO_OSPEED_FAST;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&pinTx);

	/* Pin sobre los que funciona el USART2 (RX)*/
	pinRx.pGPIOx                          = GPIOA;
	pinRx.pinConfig.GPIO_PinNumber        = PIN_3;
	pinRx.pinConfig.GPIO_PinMode          = GPIO_MODE_ALTFN;
	pinRx.pinConfig.GPIO_PinAltFunMode    = AF7;
	pinRx.pinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
	pinRx.pinConfig.GPIO_PinOutputSpeed   = GPIO_OSPEED_FAST;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&pinRx);

	/* Configuramos el puerto serial USART2 */
	commSerial.ptrUSARTx                  = USART2;
	commSerial.USART_Config.baudrate      = USART_BAUDRATE_115200;
	commSerial.USART_Config.datasize      = USART_DATASIZE_8BIT;
	commSerial.USART_Config.parity        = USART_PARITY_NONE;
	commSerial.USART_Config.stopbits      = USART_STOPBIT_1;
	commSerial.USART_Config.mode          = USART_MODE_TX;
	commSerial.USART_Config.enableIntRX   = USART_RX_INTERRUP_DISABLE;

	//Cargamos la configuración en los registros que gobiernan el puerto
	usart_WriteChar(&commSerial, '\0');
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
	gpio_TooglePin(&stateLed);
	sendMsg = 1;
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
