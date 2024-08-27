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

USART_Handler_t   usart2    = {0};
GPIO_Handler_t    usart2t   = {0};

Timer_Handler_t  blinkTimer = {0};
GPIO_Handler_t   userLed    = {0};
GPIO_Handler_t   userLed1   = {0};

EXTI_Config_t   imprimir    = {0};
GPIO_Handler_t  user13      = {0};

//USART_Handler_t usart2rx  = {0};
GPIO_Handler_t   usart2trx  = {0};

char bufferMsg[128]         = {0};

uint8_t bandera             = {0};
uint8_t sendMsg             = {0};
uint8_t receivedChar        = {0};

//Definición de las cabeceras de las funciones del main
void initSys(void);


/*  Main function  */
int main(void)
{
	//Inicialización de los elementos del sistema
	initSys();

	//Mandamos un "Hola mundo" indicando que la configuración está cargada

    /* Loop forever */
	while(1){

		if(receivedChar){

			if(receivedChar == 'P'){
				usart_writeMsg(&usart2, "Testing, Testing!!\n\r");
			}
			if(receivedChar == 's'){
				usart_writeMsg(&usart2, "Hola\n\r");
			}
			if(receivedChar == 'C'){
				usart_writeMsg(&usart2, "Led on\n\r");
				gpio_WritePin(&userLed1, SET);
			}
			if(receivedChar == 'S'){
				usart_writeMsg(&usart2, "Led off\n\r");
				gpio_WritePin(&userLed1, RESET);
			}

			receivedChar = 0;
		}

	}

}

//Definimos función para configuración inicial
void initSys(void){

	/* Configuramos el pin A5*/
	userLed.pGPIOx                         = GPIOA;
	userLed.pinConfig.GPIO_PinNumber       = PIN_5;
	userLed.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	userLed.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	userLed.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
	userLed.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&userLed);

	/* Configuramos el pin A9*/
	userLed1.pGPIOx                         = GPIOA;
	userLed1.pinConfig.GPIO_PinNumber       = PIN_9;
	userLed1.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	userLed1.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	userLed1.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
	userLed1.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&userLed1);

	/* Configuramos el timer del blinky*/
	blinkTimer.pTIMx                             = TIM2;
	blinkTimer.TIMx_Config.TIMx_Prescaler        = 16000;  //Genera incrementos de 1 ms
	blinkTimer.TIMx_Config.TIMx_Period           = 250;     //De la mano con el prescaler, genera int ada 500 ms
	blinkTimer.TIMx_Config.TIMx_mode             = TIMER_UP_COUNTER;
	blinkTimer.TIMx_Config.TIMx_InterruptEnable  = TIMER_INT_ENABLE;

	/* Configuramos el Timer */
	timer_Config(&blinkTimer);

	//Encendemos el Timer
	timer_SetState(&blinkTimer, TIMER_ON);

	/* Configuramos los pines del puerto serial*/

	/* Pin sobre los que funciona el USART2 (RX)*/
	usart2t.pGPIOx                          = GPIOA;
	usart2t.pinConfig.GPIO_PinNumber        = PIN_2;
	usart2t.pinConfig.GPIO_PinMode          = GPIO_MODE_ALTFN;
	usart2t.pinConfig.GPIO_PinOutputType    = GPIO_OTYPE_PUSHPULL;
	usart2t.pinConfig.GPIO_PinOutputSpeed   = GPIO_OSPEED_MEDIUM;
	usart2t.pinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
	usart2t.pinConfig.GPIO_PinAltFunMode    = AF7;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&usart2t);

	/* Configuramos el puerto serial USART2 */
	usart2.ptrUSARTx                  = USART2;
	usart2.USART_Config.baudrate      = USART_BAUDRATE_230400;
	usart2.USART_Config.datasize      = USART_DATASIZE_8BIT;
	usart2.USART_Config.parity        = USART_PARITY_NONE;
	usart2.USART_Config.stopbits      = USART_STOPBIT_1;
	usart2.USART_Config.mode          = USART_MODE_RXTX;
	usart2.USART_Config.enableIntRX   = USART_RX_INTERRUP_ENABLE;
	usart2.USART_Config.enableIntTX   = USART_TX_INTERRUP_DISABLE;

	//Cargamos la configuración en los registros que gobiernan el puerto
	usart_Config(&usart2);

	bufferMsg[0] = 'H';
	bufferMsg[1] = 'o';
	bufferMsg[2] = 'l';
	bufferMsg[3] = 'a';
	bufferMsg[4] = '\n';
	bufferMsg[5] = 0;

	usart_writeMsg(&usart2, bufferMsg);

	/* Pin sobre los que funciona el USART3 (RX)*/
	usart2trx.pGPIOx                          = GPIOA;
	usart2trx.pinConfig.GPIO_PinNumber        = PIN_3;
	usart2trx.pinConfig.GPIO_PinMode          = GPIO_MODE_ALTFN;
	usart2trx.pinConfig.GPIO_PinOutputType    = GPIO_OTYPE_PUSHPULL;
	usart2trx.pinConfig.GPIO_PinOutputSpeed   = GPIO_OSPEED_MEDIUM;
	usart2trx.pinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
	usart2trx.pinConfig.GPIO_PinAltFunMode    = AF7;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&usart2trx);


	user13.pGPIOx                          = GPIOC;
	user13.pinConfig.GPIO_PinNumber        = PIN_13;
	user13.pinConfig.GPIO_PinMode          = GPIO_MODE_IN;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&user13);

	imprimir.pGPIOHandler      = &user13;
	imprimir.edgeType          = EXTERNAL_INTERRUPT_FALLING_EDGE;

	exti_Config(&imprimir);

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
	gpio_TooglePin(&userLed);
	sendMsg = 1;
}

void callback_ExtInt13(void){
	bandera = 1;
}

void usart2_RxCallback(void){
	receivedChar = 1;
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
