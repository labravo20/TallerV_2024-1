/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Laura Sofia Bravo Revelo (labravo)
 * @brief          : Solución Tarea 3
 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32_assert.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"
#include "exti_driver_hal.h"
#include "usart_driver_hal.h"

//Definimos pines a utilizar para verificación
GPIO_Handler_t verificationLed    = {0}; //PinA5 (Led para verificación de correcto funcionamiento)
GPIO_Handler_t stateLed           = {0}; //PinH1 (Led de estado)

//Definimos pines a utilizar para EXTI
GPIO_Handler_t userOutputSensor   = {0};//Pin B2  //EXTI output sensor RGB --> interrupción

//Definimos pines a utilizar para USART
GPIO_Handler_t   userUsart2Tx     = {0};//Pin A2 //USART pin de transmisón
GPIO_Handler_t   userUsart2Rx     = {0};//Pin A3 //USART pin de recepción

//Definimos timers a utilizar
Timer_Handler_t blinkTimer        = {0}; // Timer para el blinking
Timer_Handler_t controlTimer      = {0}; // Timer asociado al control del tiempo

//Definición lineas EXTI que vamos a utilizar
EXTI_Config_t outputSensorExti    = {0}; //EXTI linea 2 para el output del sensor RGB

//Definimos USART a usar
USART_Handler_t   usart2          = {0};

//Definimos el caracter para ejecución del USART
char bufferMsg[128]               = {0};

//Definimos variable para recibir el valor del received char en análisis USART
uint8_t   receivedChar            = 0;

// Definimos variable para contar rising edges en señal PWM output sensor
uint16_t counterOutputSensor      = 0;

// Definimos variables para cargar características de la señal PWM resultado del output del sensor RGB
uint8_t outputSensorState            = 0;

//Definimos variables para asignar el estado de la bandera correspondiente a cada interrupción
uint8_t banderaControlTimer       = 0;
uint8_t banderaOutputSensorExti   = 0;
uint8_t banderaUSARTTx            = 0;
uint8_t banderaUSARTRx            = 0;

//Definición función para configuración inicial
void initialConfig(void);

//Definición función para configuración counter encoder
void counterOutputSensorConfig(void);

//Definición función para ejecutar counter encoder
void counterOutputSensorAction(void);


/*  Main function  */
int main(void)
{
	//Llamamos función para realizar configuración inicial
	initialConfig();

    /* Loop forever */
	while(1){

		//Evaluamos si la bandera de la interrupción responsable del output sensor RGB
		//está levantada y en caso de ser asi se ejecuta la configuración del counter output sensor
		counterOutputSensorAction();

	}//Fin ciclo while

}//Fin función main

//Definimos función para realizar todas las configuraciones iniciales
void initialConfig(){

	    //VERIFICACIÓN DE FUNCIONAMIENTO CONFIGURACIÓN DE DRIVERS
		/* Configuramos el pin A5*/
		//verificationLed.pGPIOx                         = GPIOA;
		//verificationLed.pinConfig.GPIO_PinNumber       = PIN_5;
		//verificationLed.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		//verificationLed.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		//verificationLed.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		//verificationLed.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		//gpio_Config(&verificationLed);


		//Ejecutamos la configuración realizada en A5
		//gpio_WritePin(&verificationLed, SET);

		/* A continuación se realiza la configuración del led de estado (este está ubicado en
		 * la "board táctica")*/

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

		//Configuración Timer2 --> blinking
		blinkTimer.pTIMx                             = TIM2;
		blinkTimer.TIMx_Config.TIMx_Prescaler        = 16000;  //Genera incrementos de 1 ms
		blinkTimer.TIMx_Config.TIMx_Period           = 500;    //Periodo asociado a 0.5s
		blinkTimer.TIMx_Config.TIMx_mode             = TIMER_UP_COUNTER;
		blinkTimer.TIMx_Config.TIMx_InterruptEnable  = TIMER_INT_ENABLE;

		/* Configuramos el Timer */
		timer_Config(&blinkTimer);

		//Encendemos el Timer
		timer_SetState(&blinkTimer, TIMER_ON);

		//A continuación se está realizando la configuración de los timers a usar

		//Configuración Timer5 --> control del tiempo
		controlTimer.pTIMx                             = TIM5;
		controlTimer.TIMx_Config.TIMx_Prescaler        = 16000;  //Genera incrementos de 0.1 s
		controlTimer.TIMx_Config.TIMx_Period           = 100;     //De la mano con el prescaler...
		controlTimer.TIMx_Config.TIMx_mode             = TIMER_UP_COUNTER;
		controlTimer.TIMx_Config.TIMx_InterruptEnable  = TIMER_INT_ENABLE;

		/* Configuramos el Timer */
		timer_Config(&controlTimer);

		//Encendemos el Timer
		timer_SetState(&controlTimer, TIMER_ON);

		//A continuación se está realizando la configuración de las lineas para EXTI a usar

		/*Configuramos el pin B2  --> Recibe la señal asociada al output del sensor RGB*/
		userOutputSensor.pGPIOx                         = GPIOB;
		userOutputSensor.pinConfig.GPIO_PinNumber       = PIN_2;
		userOutputSensor.pinConfig.GPIO_PinMode         = GPIO_MODE_IN;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&userOutputSensor);

		//A continuación se está realizando la configuración de los EXTI a usar

		/*Configuramos el EXTI ck que será en la linea 2 --> Clock*/
		outputSensorExti.pGPIOHandler = &userOutputSensor;
		outputSensorExti.edgeType     = EXTERNAL_INTERRUPT_RISING_EDGE;

		//Cargamos la configuración de la interrupción externa (EXTI)
		exti_Config(&outputSensorExti);

		//A continuación se está realizando configuración del puerto serial

		/* Pin sobre los que funciona el USART2 (TX)*/
		userUsart2Tx.pGPIOx                          = GPIOA;
		userUsart2Tx.pinConfig.GPIO_PinNumber        = PIN_2;
		userUsart2Tx.pinConfig.GPIO_PinMode          = GPIO_MODE_ALTFN;
		userUsart2Tx.pinConfig.GPIO_PinOutputType    = GPIO_OTYPE_PUSHPULL;
		userUsart2Tx.pinConfig.GPIO_PinOutputSpeed   = GPIO_OSPEED_MEDIUM;
		userUsart2Tx.pinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
		userUsart2Tx.pinConfig.GPIO_PinAltFunMode    = AF7;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&userUsart2Tx);

		/* Pin sobre los que funciona el USART2 (TX)*/
		userUsart2Rx.pGPIOx                          = GPIOA;
		userUsart2Rx.pinConfig.GPIO_PinNumber        = PIN_3;
		userUsart2Rx.pinConfig.GPIO_PinMode          = GPIO_MODE_ALTFN;
		userUsart2Rx.pinConfig.GPIO_PinOutputType    = GPIO_OTYPE_PUSHPULL;
		userUsart2Rx.pinConfig.GPIO_PinOutputSpeed   = GPIO_OSPEED_MEDIUM;
		userUsart2Rx.pinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
		userUsart2Rx.pinConfig.GPIO_PinAltFunMode    = AF7;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&userUsart2Rx);

		/* Configuramos el puerto serial USART2 */
		usart2.ptrUSARTx                  = USART2;
		usart2.USART_Config.baudrate      = USART_BAUDRATE_115200;
		usart2.USART_Config.datasize      = USART_DATASIZE_8BIT;
		usart2.USART_Config.parity        = USART_PARITY_NONE;
		usart2.USART_Config.stopbits      = USART_STOPBIT_1;
		usart2.USART_Config.mode          = USART_MODE_RXTX;
		usart2.USART_Config.enableIntRX   = USART_RX_INTERRUP_ENABLE;
		usart2.USART_Config.enableIntTX   = USART_TX_INTERRUP_DISABLE;

		//Cargamos la configuración en los registros que gobiernan el puerto
		usart_Config(&usart2);

}

//Función para configuración counter encoder
void counterOutputSensorConfig(void){

	if(outputSensorState == 1){

		counterOutputSensor++;
	}

}

//Función para ejecutar counter encoder
void counterOutputSensorAction(void){

	//Evaluamos si la bandera de la interrupción responsable del counter encoder
	//está levantada
	if(banderaOutputSensorExti == 1){

	    //Llamamos a la función encargada del counter encoder
	    counterOutputSensorConfig();

		//Bajamos la bandera de la interrupción de Counter encoder
	    banderaOutputSensorExti = 0;

	}
}


/*
 * Overwrite function for A5
 * */
//void Timer2_Callback(void){
	//gpio_TooglePin(&verificationLed);
//}

/*
 * Overwrite function for H1
 * */
void Timer2_Callback(void){
	gpio_TooglePin(&stateLed);

	//Activamos bandera correspondiente a USART para transmisión
	banderaUSARTTx = 1;
}

/*
 * Overwrite function for control del tiempo
 * */
void Timer5_Callback(void){

	//Subimos la bandera de la interrupción de Control Timer
	banderaControlTimer = 1;
}


/*
 * Overwrite function for clock
 * */
void callback_ExtInt2(void){

	//Activamos bandera de la interrupción
	banderaOutputSensorExti = 1;

	//Almacenamos la informacion recibida por los datos de la señal clock y la señal data
	//Es necesario establecer los valores en el callback para tener una velocidad
	//correcta en la lectura del dato.
	outputSensorState = gpio_ReadPin(&userOutputSensor);

	//La siguiente función se estableció para poder evaluar como están cambiando los valores de la variable

	__NOP();
}

/*
 * Overwrite function for usart Rx
 * */
void usart2_RxCallback(void){

	banderaUSARTRx     = 1; // ==== REVISAR SI ES NECESARIO USAR ====

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
