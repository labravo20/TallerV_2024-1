/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Laura Sofia Bravo Revelo (labravo)
 * @brief          : Solución Tarea 2
 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32_assert.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"

//Definimos pines a utilizar
GPIO_Handler_t userLed      = {0}; //PinA5 (Led para verificación de correcto funcionamiento)
GPIO_Handler_t userLed00    = {0}; //PinH1 (Led de estado)
GPIO_Handler_t userLed01    = {0}; //PinC12 (led "a")
GPIO_Handler_t userLed02    = {0}; //PinA12 (led "b")
GPIO_Handler_t userLed03    = {0}; //PinC6  (led "c")
GPIO_Handler_t userLed04    = {0}; //PinB13 (led "d")
GPIO_Handler_t userLed05    = {0}; //PinB10 (led "e")
GPIO_Handler_t userLed06    = {0}; //PinB7  (led "f")
GPIO_Handler_t userLed07    = {0}; //PinC10 (led "g")
GPIO_Handler_t vcc_uni      = {0}; //PinA0
GPIO_Handler_t vcc_dec      = {0}; //PinB12

//Definimos timers a utilizar
Timer_Handler_t blinkTimer   = {0}; // Timer para el blinking
Timer_Handler_t displayTimer = {0}; // Timer asociado al display del siete segmentos
Timer_Handler_t controlTimer = {0}; // Timer asociado al control del tiempo

// Definimos variable para activar contador
uint8_t counter_i = 0;

//Definición función para configuración inicial
void initialConfig();

/*  Main function  */
int main(void)
{
	//Llamamos función para realizar configuración inicial
	void initialConfig();

    /* Loop forever */
	while(1){

	}

}

//Definimos función para realizar todas las configuraciones iniciales
void initialConfig(){

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
		gpio_WritePin(&userLed00, SET);

		//Configuración Timer2 --> blinking
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

		//A continuación se está probando el correcto funcionamiento del pin C12
		//gpio_WritePin(&userLed01, SET);

		/* Configuramos el pin A12 --> LED b*/
		userLed02.pGPIOx                         = GPIOA;
		userLed02.pinConfig.GPIO_PinNumber       = PIN_12;
		userLed02.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		userLed02.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		userLed02.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		userLed02.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&userLed02);

		//A continuación se está probando el correcto funcionamiento del pin A12
		//gpio_WritePin(&userLed02, SET);

		/* Configuramos el pin C6 --> LED c*/
		userLed03.pGPIOx                         = GPIOC;
		userLed03.pinConfig.GPIO_PinNumber       = PIN_6;
		userLed03.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		userLed03.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		userLed03.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		userLed03.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&userLed03);

		//A continuación se está probando el correcto funcionamiento del pin C6
		//gpio_WritePin(&userLed03, SET);

		/* Configuramos el pin B13 --> LED d*/
		userLed04.pGPIOx                         = GPIOB;
		userLed04.pinConfig.GPIO_PinNumber       = PIN_13;
		userLed04.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		userLed04.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		userLed04.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		userLed04.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&userLed04);

		//A continuación se está probando el correcto funcionamiento del pin B13
		//gpio_WritePin(&userLed04, SET);

		/* Configuramos el pin B10 --> LED e*/
		userLed05.pGPIOx                         = GPIOB;
		userLed05.pinConfig.GPIO_PinNumber       = PIN_10;
		userLed05.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		userLed05.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		userLed05.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		userLed05.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&userLed05);

		//A continuación se está probando el correcto funcionamiento del pin B10
		//gpio_WritePin(&userLed05, SET);

		/* Configuramos el pin B7 --> LED f*/
		userLed06.pGPIOx                         = GPIOB;
		userLed06.pinConfig.GPIO_PinNumber       = PIN_7;
		userLed06.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		userLed06.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		userLed06.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		userLed06.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&userLed06);

		//A continuación se está probando el correcto funcionamiento del pin B7
		//gpio_WritePin(&userLed06, SET);

		/* Configuramos el pin C10 --> LED g*/
		userLed07.pGPIOx                         = GPIOC;
		userLed07.pinConfig.GPIO_PinNumber       = PIN_10;
		userLed07.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		userLed07.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		userLed07.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		userLed07.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&userLed07);

		//A continuación se está probando el correcto funcionamiento del pin C10
		//gpio_WritePin(&userLed07, SET);

		//Cargamos ahora la configuración respectiva para los pines de alimentación de los vcc
		//de los transistores que componen el circuito del siete segmentos.

		/* Configuramos el pin A0 --> vcc unidad*/
		vcc_uni.pGPIOx                         = GPIOA;
		vcc_uni.pinConfig.GPIO_PinNumber       = PIN_0;
		vcc_uni.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		vcc_uni.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		vcc_uni.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		vcc_uni.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&vcc_uni);

		/* Configuramos el pin B12 --> vcc decimal*/
		vcc_dec.pGPIOx                         = GPIOB;
		vcc_dec.pinConfig.GPIO_PinNumber       = PIN_12;
		vcc_dec.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		vcc_dec.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		vcc_dec.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		vcc_dec.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&vcc_dec);

		//Configuración Timer3 --> display del siete segmentos
		displayTimer.pTIMx                             = TIM3;
		displayTimer.TIMx_Config.TIMx_Prescaler        = 16000;  //Genera incrementos de 1 ms
		displayTimer.TIMx_Config.TIMx_Period           = 250;     //De la mano con el prescaler...
		displayTimer.TIMx_Config.TIMx_mode             = TIMER_UP_COUNTER;
		displayTimer.TIMx_Config.TIMx_InterruptEnable  = TIMER_INT_ENABLE;

		/* Configuramos el Timer */
		timer_Config(&displayTimer);

		//Encendemos el Timer
		timer_SetState(&displayTimer, TIMER_ON);

		//Configuración Timer5 --> control del tiempo
		controlTimer.pTIMx                             = TIM5;
		controlTimer.TIMx_Config.TIMx_Prescaler        = 16000;  //Genera incrementos de 1 ms
		controlTimer.TIMx_Config.TIMx_Period           = 500;     //De la mano con el prescaler...
		controlTimer.TIMx_Config.TIMx_mode             = TIMER_UP_COUNTER;
		controlTimer.TIMx_Config.TIMx_InterruptEnable  = TIMER_INT_ENABLE;

		/* Configuramos el Timer */
		timer_Config(&controlTimer);

		//Encendemos el Timer
		timer_SetState(&controlTimer, TIMER_ON);
}

//Definimos función para modo set o reset de los pines con respecto al número (0 a 9)
// === Se van a construir varias funciones cada una estudiando el estado del pin individual (7 pines)

//Definiendo función para el Led "a"
uint32_t counter_a(uint8_t counterSietea){

	//Definimos variable para cargar estado de pin activo o NO activo dependiendo del número específico
	uint8_t pinLed_a = 0;

	// Definimos a continuación todos los numeros en los cuales se debe ACTIVAR o DESACTIVAR al led "a"
	if(counterSietea == 1){

		//Desactivamos al led
		pinLed_a = 0;
	} else if (counterSietea == 4){

		//Desactivamos al led
		pinLed_a = 0;
	} else {

		//Activamos al led
		pinLed_a = 1;
	}

	//Retornamos el valor que será usado en la función writePin (activa (1) o desactiva (0) el pin)
	//dentro del counter
	return pinLed_a;
}

//Definiendo función para el Led "b"
uint32_t counter_b(uint8_t counterSieteb){

	//Definimos variable para cargar estado de pin activo o NO activo dependiendo del número específico
	uint8_t pinLed_b = 0;

	// Definimos a continuación todos los numeros en los cuales se debe ACTIVAR o DESACTIVAR al led "b"
	if(counterSieteb == 5){

		//Desactivamos al led
		pinLed_b = 0;
	} else if (counterSieteb == 6){

		//Desactivamos al led
		pinLed_b = 0;
	} else {

		//Activamos al led
		pinLed_b = 1;
	}

	//Retornamos el valor que será usado en la función writePin (activa (1) o desactiva (0) el pin)
	//dentro del counter
	return pinLed_b;
}

//Definiendo función para el Led "c"
uint32_t counter_c(uint8_t counterSietec){

	//Definimos variable para cargar estado de pin activo o NO activo dependiendo del número específico
	uint8_t pinLed_c = 0;

	// Definimos a continuación todos los numeros en los cuales se debe ACTIVAR o DESACTIVAR al led "c"
	if(counterSietec == 2){

		//Desactivamos al led
		pinLed_c = 0;
	} else {

		//Activamos al led
		pinLed_c = 1;
	}

	//Retornamos el valor que será usado en la función writePin (activa (1) o desactiva (0) el pin)
	//dentro del counter
	return pinLed_c;
}

//Definiendo función para el Led "d"
uint32_t counter_d(uint8_t counterSieted){

	//Definimos variable para cargar estado de pin activo o NO activo dependiendo del número específico
	uint8_t pinLed_d = 0;

	// Definimos a continuación todos los numeros en los cuales se debe ACTIVAR o DESACTIVAR al led "d"
	if(counterSieted == 1){

		//Desactivamos al led
		pinLed_d = 0;
	} else if (counterSieted == 4){

		//Desactivamos al led
		pinLed_d = 0;
	} else if (counterSieted == 7){

		//Desactivamos al led
		pinLed_d = 0;
	} else {

		//Activamos al led
		pinLed_d = 1;
	}

	//Retornamos el valor que será usado en la función writePin (activa (1) o desactiva (0) el pin)
	//dentro del counter
	return pinLed_d;
}

//Definiendo función para el Led "e"
uint32_t counter_e(uint8_t counterSietee){

	//Definimos variable para cargar estado de pin activo o NO activo dependiendo del número específico
	uint8_t pinLed_e = 0;

	// Definimos a continuación todos los numeros en los cuales se debe ACTIVAR o DESACTIVAR al led "e"
	if(counterSietee == 0){

		//activamos al led
		pinLed_e = 1;
	} else if (counterSietee == 2){

		//activamos al led
		pinLed_e = 1;
	} else if (counterSietee == 6){

		//activamos al led
		pinLed_e = 1;
	} else if (counterSietee == 8){

		//activamos al led
		pinLed_e = 1;
	} else {

		//Desactivamos al led
		pinLed_e = 0;
	}

	//Retornamos el valor que será usado en la función writePin (activa (1) o desactiva (0) el pin)
	//dentro del counter
	return pinLed_e;
}

//Definiendo función para el Led "f"
uint32_t counter_f(uint8_t counterSietef){

	//Definimos variable para cargar estado de pin activo o NO activo dependiendo del número específico
	uint8_t pinLed_f = 0;

	// Definimos a continuación todos los numeros en los cuales se debe ACTIVAR o DESACTIVAR al led "f"
	if(counterSietef == 1){

		//Desactivamos al led
		pinLed_f = 0;
	} else if (counterSietef == 2){

		//Desactivamos al led
		pinLed_f = 0;
	} else if (counterSietef == 3){

		//Desactivamos al led
		pinLed_f = 0;
	} else if (counterSietef == 7){

		//Desactivamos al led
		pinLed_f = 0;
	} else {

		//Activamos al led
		pinLed_f = 1;
	}

	//Retornamos el valor que será usado en la función writePin (activa (1) o desactiva (0) el pin)
	//dentro del counter
	return pinLed_f;
}

//Definiendo función para el Led "g"
uint32_t counter_g(uint8_t counterSieteg){

	//Definimos variable para cargar estado de pin activo o NO activo dependiendo del número específico
	uint8_t pinLed_g = 0;

	// Definimos a continuación todos los numeros en los cuales se debe ACTIVAR o DESACTIVAR al led "g"
	if(counterSieteg == 0){

		//Desactivamos al led
		pinLed_g = 0;
	} else if (counterSieteg == 1){

		//Desactivamos al led
		pinLed_g = 0;
	} else if (counterSieteg == 7){

		//Desactivamos al led
		pinLed_g = 0;
	} else {

		//Activamos al led
		pinLed_g = 1;
	}

	//Retornamos el valor que será usado en la función writePin (activa (1) o desactiva (0) el pin)
	//dentro del counter
	return pinLed_g;
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
 * Overwrite function for display del siete segmentos
 * */
void Timer3_Callback(void){

}
/*
 * Overwrite function for control del tiempo
 * */
void Timer5_Callback(void){

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
