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
Timer_Handler_t blinkTimer   = {0}; //Timer para el blinky pin A5

//Definición lineas EXTI que vamos a utilizar
EXTI_Config_t swExti    = {0}; //EXTI linea 3 para el sw del encoder
EXTI_Config_t ckExti    = {0}; //EXTI linea 13 para el ck del encoder

//Variables auxiliares que ayudarán en el código
uint8_t dir0      = {0};
uint8_t dir1      = {0};
uint8_t dirR      = {0};
uint8_t numero    = {0};

//Llamamos las funciones definidas al final del código
void suma(uint8_t *conteo);
void resta(uint8_t *conteo);

//Creación de un enum con los cuatro posibles casos del encoder y tarea
enum{
	sumaDerecha = 00, restaIzquierda
} casosEncoder;

//Llamamos la función de la configuración inicial
void initSys(void);

/*  Main function  */
int main(void)
{
	initSys();

    /* Loop forever */
	while(1){

	}
}

//Definición de función para realizar configuración inicil
void initSys(void){
	//Configuramos los pines que se van a utilizar

	/*Configuramos el pinA5*/
	userLed.pGPIOx                         = GPIOA;
	userLed.pinConfig.GPIO_PinNumber       = PIN_5;
	userLed.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	userLed.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	userLed.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
	userLed.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&userLed);

	/*Configuramos el pinC6*/
	userLed1.pGPIOx                         = GPIOC;
	userLed1.pinConfig.GPIO_PinNumber       = PIN_6;
	userLed1.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	userLed1.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	userLed1.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
	userLed1.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&userLed1);

	/*Configuramos el pinA7*/
	userLed2.pGPIOx                         = GPIOA;
	userLed2.pinConfig.GPIO_PinNumber       = PIN_7;
	userLed2.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	userLed2.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	userLed2.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
	userLed2.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&userLed2);

	/*Configuramos el pinC8*/
	userLed3.pGPIOx                         = GPIOC;
	userLed3.pinConfig.GPIO_PinNumber       = PIN_8;
	userLed3.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	userLed3.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	userLed3.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
	userLed3.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&userLed3);

	/*Configuramos el pinA10  --> A0*/
	userSwitch.pGPIOx                         = GPIOA;
	userSwitch.pinConfig.GPIO_PinNumber       = PIN_0;
	userSwitch.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	userSwitch.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	userSwitch.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
	userSwitch.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&userSwitch);

	/*Configuramos el pinB5*/
	userData.pGPIOx                         = GPIOB;
	userData.pinConfig.GPIO_PinNumber       = PIN_5;
	userData.pinConfig.GPIO_PinMode         = GPIO_MODE_IN;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&userData);

	/*Configuramos el pinB3  --> A10*/
	userSWenc.pGPIOx                         = GPIOA;
	userSWenc.pinConfig.GPIO_PinNumber       = PIN_10;
	userSWenc.pinConfig.GPIO_PinMode         = GPIO_MODE_IN;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&userSWenc);

	/*Configuramos el EXTI sw que será en la linea 3*/
	swExti.pGPIOHandler = &userSWenc;
	swExti.edgeType     = EXTERNAL_INTERRUPT_RISING_EDGE;

	//Cargamos la configuración de la interrupción externa (EXTI)
	exti_Config(&swExti);

	/*Configuramos el pinB13  --> B3*/
	userCKenc.pGPIOx                         = GPIOB;
	userCKenc.pinConfig.GPIO_PinNumber       = PIN_3;
	userCKenc.pinConfig.GPIO_PinMode         = GPIO_MODE_IN;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&userCKenc);

	/*Configuramos el EXTI sw que será en la linea 13*/
	ckExti.pGPIOHandler = &userCKenc;
	ckExti.edgeType     = EXTERNAL_INTERRUPT_RISING_EDGE;

	//Cargamos la configuración de la interrupción externa (EXTI)
	exti_Config(&ckExti);

	//Configuramos los timers

	/*Configuramos el timer del blink (TIM2)  --> TIM4*/
	blinkTimer.pTIMx                             = TIM4;
	blinkTimer.TIMx_Config.TIMx_Prescaler        = 16000;
	blinkTimer.TIMx_Config.TIMx_Period           = 250;
	blinkTimer.TIMx_Config.TIMx_mode             = TIMER_UP_COUNTER;
	blinkTimer.TIMx_Config.TIMx_InterruptEnable  = TIMER_INT_ENABLE;

	//Cargamos la configuración del timer
	timer_Config(&blinkTimer);

	//Encendemos el timer
	timer_SetState(&blinkTimer, TIMER_ON);

	//Encendemos el led que nos indica que se cargaron las configuraciones
	gpio_WritePin(&userLed, SET);
}

/*
 * Overwrite function for A5
 * */
void Timer4_Callback(void){
	gpio_TooglePin(&userLed);
}

void callback_ExtInt10(void){

}

void callback_ExtInt3(void){

	//Almacenamos la informacion recibida por los datos
	dir0 = gpio_ReadPin(&userCKenc);
	dir1 = gpio_ReadPin(&userData);
}

//Función que suma 1 a la variable que indiquemos como parámetro
void suma(uint8_t *conteo){
	*conteo += 1;
}

//Función que resta 1 a la variable que indiquemos como parámetro
void resta(uint8_t *conteo){
	*conteo -= 1;
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
