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
#include "exti_driver_hal.h"
#include "usart_driver_hal.h"
#include "pwm_driver_hal.h"

//Definición de los handlers necesarios para el led de estado
GPIO_Handler_t   blinkyPin       = {0};
Timer_Handler_t  blinkyTimer     = {0};

//Elementos para el PWM
GPIO_Handler_t   pinPWMChannel    = {0};
PWM_Handler_t    signalPWM        = {0};


//Definición de elementos para realizar la comunicación serial
USART_Handler_t  usart2commSerial = {0};
GPIO_Handler_t   pinTx            = {0};
GPIO_Handler_t   pinRx            = {0};

uint16_t   duttyValue    	= 0;

uint8_t banderaUSART2       = 0;
uint8_t getMsg              = 0;
char bufferMsg[128]  		= {0};

//Definición de cabeceras de las funciones main
void initialSystem(void);


/*  Main function  */
int main(void)
{
	duttyValue = 30;

	initialSystem();

    /* Loop forever */
	while(1){

//		//Verificando el PWM
		if(getMsg != '\0'){

			if(getMsg == 'D'){

				//Down..
				duttyValue = 10;
				pwm_Update_DuttyCycle(&signalPWM, duttyValue);

				sprintf(bufferMsg, "dutty = %u \n", duttyValue);
				usart_writeMsg(&usart2commSerial, bufferMsg);
			}

			//Para probar el seno
			if(getMsg == 'U'){

				//Up
				duttyValue = 40;
				pwm_Update_DuttyCycle(&signalPWM, duttyValue);

				sprintf(bufferMsg, "dutty = %u \n", duttyValue);
				usart_writeMsg(&usart2commSerial, bufferMsg);
			}

			//Limpiamos el valor de la variable de recepción
			getMsg = '\0';
		}

	}
}


void initialSystem(void){

	/* Configuramos el pin A5*/
	blinkyPin.pGPIOx                         = GPIOA;
	blinkyPin.pinConfig.GPIO_PinNumber       = PIN_5;
	blinkyPin.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	blinkyPin.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	blinkyPin.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
	blinkyPin.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&blinkyPin);

	//Ejecutamos la configuración realizada en A5
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

	/*Configuración Comm serial*/
	usart2commSerial.ptrUSARTx                = USART2;
	usart2commSerial.USART_Config.baudrate    = USART_BAUDRATE_115200;
	usart2commSerial.USART_Config.datasize    = USART_DATASIZE_8BIT;
	usart2commSerial.USART_Config.parity      = USART_PARITY_NONE;
	usart2commSerial.USART_Config.stopbits    = USART_STOPBIT_1;
	usart2commSerial.USART_Config.mode        = USART_MODE_RXTX;
	usart2commSerial.USART_Config.enableIntRX = USART_RX_INTERRUP_ENABLE;
	usart2commSerial.USART_Config.enableIntTX = USART_TX_INTERRUP_DISABLE;

	usart_Config(&usart2commSerial);

	pinTx.pGPIOx                           = GPIOA;
	pinTx.pinConfig.GPIO_PinNumber         = PIN_2;
	pinTx.pinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
	pinTx.pinConfig.GPIO_PinAltFunMode     = AF7;
	pinTx.pinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
	pinTx.pinConfig.GPIO_PinOutputSpeed    = GPIO_OSPEED_FAST;

	gpio_Config(&pinTx);

	pinRx.pGPIOx                           = GPIOA;
	pinRx.pinConfig.GPIO_PinNumber         = PIN_3;
	pinRx.pinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
	pinRx.pinConfig.GPIO_PinAltFunMode     = AF7;
	pinRx.pinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
	pinRx.pinConfig.GPIO_PinOutputSpeed    = GPIO_OSPEED_FAST;

	gpio_Config(&pinRx);

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
	signalPWM.config.duttyCicle      = duttyValue;
	signalPWM.config.prescaler       = 16000;
	signalPWM.config.periodo         = 60;

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

void usart2_RxCallback(void){

	//Importante!!!
	// Asignamos es valor de la función que llama usrt_getRxData, puesto que esta toma el
	//valor que está cargado en el DR
	getMsg = usart_getRxData();
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
