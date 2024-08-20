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
GPIO_Handler_t verificationLed    = {0}; //PinA5 (Led para verificación de correcto funcionamiento)
GPIO_Handler_t stateLed           = {0}; //PinH1 (Led de estado)
GPIO_Handler_t segmentoLed_a      = {0}; //PinC12 (led "a")
GPIO_Handler_t segmentoLed_b      = {0}; //PinA12 (led "b")
GPIO_Handler_t segmentoLed_c      = {0}; //PinC6  (led "c")
GPIO_Handler_t segmentoLed_d      = {0}; //PinB13 (led "d")
GPIO_Handler_t segmentoLed_e      = {0}; //PinB10 (led "e")
GPIO_Handler_t segmentoLed_f      = {0}; //PinC10 (led "f")
GPIO_Handler_t segmentoLed_g      = {0}; //PinB7  (led "g")
GPIO_Handler_t vcc_decena         = {0}; //PinB12
GPIO_Handler_t vcc_unidad         = {0}; //PinA0

//Definimos timers a utilizar
Timer_Handler_t blinkTimer   = {0}; // Timer para el blinking
Timer_Handler_t displayTimer = {0}; // Timer asociado al display del siete segmentos
Timer_Handler_t controlTimer = {0}; // Timer asociado al control del tiempo

// Definimos variable para activar contador
uint8_t counter_i = 0;

//Definiendo funciones a usar
uint32_t counter_a(uint8_t counterSietea);
uint32_t counter_b(uint8_t counterSieteb);
uint32_t counter_c(uint8_t counterSietec);
uint32_t counter_d(uint8_t counterSieted);
uint32_t counter_e(uint8_t counterSietee);
uint32_t counter_f(uint8_t counterSietef);
uint32_t counter_g(uint8_t counterSieteg);

//Definimos varible para cargar valor de la unidad y la decena del numero a representar
uint8_t unidad = 0;
uint8_t decena = 0;

//Definimos variable para activar vcc de unidad o vcc de decena
uint8_t posicion = 0;

//Definimos máscara para alternar la posicion unidad o decena
uint8_t maskChangeDisplay     = 1;

//Definimos variables para asignar el estado de la bandera correspondiente a cada interrupción
uint8_t banderaDisplayTimer   = 0;
uint8_t banderaControlTimer   = 0;

//Definición función para configuración inicial
void initialConfig();

/*  Main function  */
int main(void)
{
	//Llamamos función para realizar configuración inicial
	initialConfig();

    /* Loop forever */
	while(1){

		//Pasamos a verficiar si el valor del counter requiere la representación de los dos dígitos o no
			if((counter_i) < 10){

				// Generamos condicional para representar numero en posición unidad o decena del siete segmentos

				//Condición para representar decena:
				if(posicion == 1){

					//Activamos vcc del siete segmentos para tener en cuenta decenas
					// == NOTA importante: Debido a que el siete segmentos a utilizar es de ánodo común
					// == entonces necesitamos generar conexión a tierra, en lugar de alimentación, para
					// == lograr la activación de los mismos pines, es decir que en este caso
					// == ponemos SET para desactivar y RESET para activar
					gpio_WritePin(&vcc_unidad, SET);
					gpio_WritePin(&vcc_decena, RESET);

					//Ejecutamos la configuración de los pines para la DECENA
					gpio_WritePin(&segmentoLed_a, counter_a(0));
					gpio_WritePin(&segmentoLed_b, counter_b(0));
					gpio_WritePin(&segmentoLed_c, counter_c(0));
					gpio_WritePin(&segmentoLed_d, counter_d(0));
					gpio_WritePin(&segmentoLed_e, counter_e(0));
					gpio_WritePin(&segmentoLed_f, counter_f(0));
					gpio_WritePin(&segmentoLed_g, counter_g(0));

				} else{

					//Activamos vcc del siete segmentos para tener en cuenta unidades
					// == NOTA importante: Debido a que el siete segmentos a utilizar es de ánodo común
					// == entonces necesitamos generar conexión a tierra, en lugar de alimentación, para
					// == lograr la activación de los mismos pines, es decir que en este caso
					// == ponemos SET para desactivar y RESET para activar
					gpio_WritePin(&vcc_decena, SET);
					gpio_WritePin(&vcc_unidad, RESET);

					//Ejecutamos la configuración de los pines para la UNIDAD
					gpio_WritePin(&segmentoLed_a, counter_a(counter_i));
					gpio_WritePin(&segmentoLed_b, counter_b(counter_i));
					gpio_WritePin(&segmentoLed_c, counter_c(counter_i));
					gpio_WritePin(&segmentoLed_d, counter_d(counter_i));
					gpio_WritePin(&segmentoLed_e, counter_e(counter_i));
					gpio_WritePin(&segmentoLed_f, counter_f(counter_i));
					gpio_WritePin(&segmentoLed_g, counter_g(counter_i));
				}

			} else if((counter_i) >= 10){

				// Construimos relación para identificar el valor de la unidad del número
				unidad = counter_i%10;
				// Construimos relación para identificar el valor de la decena del número
				decena = (counter_i) - unidad;

				// Generamos condicional para representar numero en posición unidad o decena del siete segmentos

				// Condición para representar decena:
				if(posicion == 1){

					//Activamos vcc del siete segmentos para tener en cuenta decenas
					// == NOTA importante: Debido a que el siete segmentos a utilizar es de ánodo común
					// == entonces necesitamos generar conexión a tierra, en lugar de alimentación, para
					// == lograr la activación de los mismos pines, es decir que en este caso
					// == ponemos SET para desactivar y RESET para activar
					gpio_WritePin(&vcc_unidad, SET);
					gpio_WritePin(&vcc_decena, RESET);

					//Ejecutamos la configuración de los pines para la DECENA
					gpio_WritePin(&segmentoLed_a, counter_a(decena/10));
					gpio_WritePin(&segmentoLed_b, counter_b(decena/10));
					gpio_WritePin(&segmentoLed_c, counter_c(decena/10));
					gpio_WritePin(&segmentoLed_d, counter_d(decena/10));
					gpio_WritePin(&segmentoLed_e, counter_e(decena/10));
					gpio_WritePin(&segmentoLed_f, counter_f(decena/10));
					gpio_WritePin(&segmentoLed_g, counter_g(decena/10));

				} else{

					//Activamos vcc del siete segmentos para tener en cuenta unidades
					// == NOTA importante: Debido a que el siete segmentos a utilizar es de ánodo común
					// == entonces necesitamos generar conexión a tierra, en lugar de alimentación, para
					// == lograr la activación de los mismos pines, es decir que en este caso
					// == ponemos SET para desactivar y RESET para activar
					gpio_WritePin(&vcc_decena, SET);
					gpio_WritePin(&vcc_unidad, RESET);

					//Ejecutamos la configuración de los pines para la UNIDAD
					gpio_WritePin(&segmentoLed_a, counter_a(unidad));
					gpio_WritePin(&segmentoLed_b, counter_b(unidad));
					gpio_WritePin(&segmentoLed_c, counter_c(unidad));
					gpio_WritePin(&segmentoLed_d, counter_d(unidad));
					gpio_WritePin(&segmentoLed_e, counter_e(unidad));
					gpio_WritePin(&segmentoLed_f, counter_f(unidad));
					gpio_WritePin(&segmentoLed_g, counter_g(unidad));
				}

			}

			//Evaluamos si la bandera de la interrupción responsable del control del tiempo
			//está levantada
			if(banderaControlTimer == 1){

				//Bajamos la bandera de la interrupción de Control Timer
				banderaControlTimer = 0;

				//Sumamos el valor del counter para garantizar la cuenta ascentente
				counter_i = counter_i +1;

				//Delimitamos que el número máximo hasta el cual se contará es 59
				if(counter_i == 60){
					//Reiniciamos el contador para repetir el ciclo de cuenta
					counter_i = 0;
				}
			}

	}

}

//Definimos función para realizar todas las configuraciones iniciales
void initialConfig(){

	    //VERIFICACIÓN DE FUNCIONAMIENTO CONFIGURACIÓN DE DRIVERS
		/* Configuramos el pin A5*/
		verificationLed.pGPIOx                         = GPIOA;
		verificationLed.pinConfig.GPIO_PinNumber       = PIN_5;
		verificationLed.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		verificationLed.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		verificationLed.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		verificationLed.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&verificationLed);

		//Ejecutamos la configuración realizada en A5
		//gpio_WritePin(&verificationLed, SET);

		/* ========== SOLUCIÓN TAREA 2 ========== */

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
		segmentoLed_a.pGPIOx                         = GPIOC;
		segmentoLed_a.pinConfig.GPIO_PinNumber       = PIN_12;
		segmentoLed_a.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		segmentoLed_a.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		segmentoLed_a.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		segmentoLed_a.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&segmentoLed_a);

		//A continuación se está probando el correcto funcionamiento del pin C12
		//gpio_WritePin(&segmentoLed_a, SET);

		/* Configuramos el pin A12 --> LED b*/
		segmentoLed_b.pGPIOx                         = GPIOA;
		segmentoLed_b.pinConfig.GPIO_PinNumber       = PIN_12;
		segmentoLed_b.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		segmentoLed_b.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		segmentoLed_b.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		segmentoLed_b.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&segmentoLed_b);

		//A continuación se está probando el correcto funcionamiento del pin A12
		//gpio_WritePin(&segmentoLed_b, SET);

		/* Configuramos el pin C6 --> LED c*/
		segmentoLed_c.pGPIOx                         = GPIOC;
		segmentoLed_c.pinConfig.GPIO_PinNumber       = PIN_6;
		segmentoLed_c.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		segmentoLed_c.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		segmentoLed_c.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		segmentoLed_c.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&segmentoLed_c);

		//A continuación se está probando el correcto funcionamiento del pin C6
		//gpio_WritePin(&segmentoLed_c, SET);

		/* Configuramos el pin B13 --> LED d*/
		segmentoLed_d.pGPIOx                         = GPIOB;
		segmentoLed_d.pinConfig.GPIO_PinNumber       = PIN_13;
		segmentoLed_d.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		segmentoLed_d.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		segmentoLed_d.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		segmentoLed_d.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&segmentoLed_d);

		//A continuación se está probando el correcto funcionamiento del pin B13
		//gpio_WritePin(&segmentoLed_d, SET);

		/* Configuramos el pin B10 --> LED e*/
		segmentoLed_e.pGPIOx                         = GPIOB;
		segmentoLed_e.pinConfig.GPIO_PinNumber       = PIN_10;
		segmentoLed_e.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		segmentoLed_e.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		segmentoLed_e.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		segmentoLed_e.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&segmentoLed_e);

		//A continuación se está probando el correcto funcionamiento del pin B10
		//gpio_WritePin(&segmentoLed_e, SET);

		/* Configuramos el pin C10 --> LED f*/
		segmentoLed_f.pGPIOx                         = GPIOC;
		segmentoLed_f.pinConfig.GPIO_PinNumber       = PIN_10;
		segmentoLed_f.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		segmentoLed_f.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		segmentoLed_f.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		segmentoLed_f.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&segmentoLed_f);

		//A continuación se está probando el correcto funcionamiento del pin B7
		//gpio_WritePin(&segmentoLed_f, SET);

		/* Configuramos el pin B7 --> LED g*/
		segmentoLed_g.pGPIOx                         = GPIOB;
		segmentoLed_g.pinConfig.GPIO_PinNumber       = PIN_7;
		segmentoLed_g.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		segmentoLed_g.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		segmentoLed_g.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		segmentoLed_g.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&segmentoLed_g);

		//A continuación se está probando el correcto funcionamiento del pin C10
		//gpio_WritePin(&segmentoLed_g, SET);

		//Cargamos ahora la configuración respectiva para los pines de alimentación de los vcc
		//de los transistores que componen el circuito del siete segmentos.

		/* Configuramos el pin A0 --> vcc unidad*/
		vcc_unidad.pGPIOx                         = GPIOA;
		vcc_unidad.pinConfig.GPIO_PinNumber       = PIN_0;
		vcc_unidad.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		vcc_unidad.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		vcc_unidad.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		vcc_unidad.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&vcc_unidad);

		/* Configuramos el pin B12 --> vcc decimal*/
		vcc_decena.pGPIOx                         = GPIOB;
		vcc_decena.pinConfig.GPIO_PinNumber       = PIN_12;
		vcc_decena.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		vcc_decena.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		vcc_decena.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		vcc_decena.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&vcc_decena);

		//A continuación se está realizando la configuración de los timers a usar

		//Configuración Timer3 --> display del siete segmentos
		displayTimer.pTIMx                             = TIM3;
		displayTimer.TIMx_Config.TIMx_Prescaler        = 16000;
		displayTimer.TIMx_Config.TIMx_Period           = 10;
		displayTimer.TIMx_Config.TIMx_mode             = TIMER_UP_COUNTER;
		displayTimer.TIMx_Config.TIMx_InterruptEnable  = TIMER_INT_ENABLE;

		/* Configuramos el Timer */
		timer_Config(&displayTimer);

		//Encendemos el Timer
		timer_SetState(&displayTimer, TIMER_ON);

		//Configuración Timer5 --> control del tiempo
		controlTimer.pTIMx                             = TIM5;
		controlTimer.TIMx_Config.TIMx_Prescaler        = 16000;  //Genera incrementos de 1 s
		controlTimer.TIMx_Config.TIMx_Period           = 1000;     //De la mano con el prescaler...
		controlTimer.TIMx_Config.TIMx_mode             = TIMER_UP_COUNTER;
		controlTimer.TIMx_Config.TIMx_InterruptEnable  = TIMER_INT_ENABLE;

		/* Configuramos el Timer */
		timer_Config(&controlTimer);

		//Encendemos el Timer
		timer_SetState(&controlTimer, TIMER_ON);
}

//Definimos función para modo set o reset de los pines con respecto al número (0 a 9)
// == NOTA importante: Debido a que el siete segmentos a utilizar es de ánodo común
// == entonces necesitamos generar conexión a tierra, en lugar de alimentación, para
// == lograr la activación de los mismos pines, es decir que en este caso SET = 0 y RESET = 1
// ==== Se van a construir varias funciones cada una estudiando el estado del pin individual (7 pines)

//Definiendo función para el Led "a"
uint32_t counter_a(uint8_t counterSietea){

	//Definimos variable para cargar estado de pin activo o NO activo dependiendo del número específico
	uint8_t pinLed_a = 0;

	// Definimos a continuación todos los numeros en los cuales se debe ACTIVAR o DESACTIVAR al led "a"
	if(counterSietea == 1){

		//Desactivamos al led
		pinLed_a = 1;
	} else if (counterSietea == 4){

		//Desactivamos al led
		pinLed_a = 1;
	} else {

		//Activamos al led
		pinLed_a = 0;
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
		pinLed_b = 1;
	} else if (counterSieteb == 6){

		//Desactivamos al led
		pinLed_b = 1;
	} else {

		//Activamos al led
		pinLed_b = 0;
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
		pinLed_c = 1;
	} else {

		//Activamos al led
		pinLed_c = 0;
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
		pinLed_d = 1;
	} else if (counterSieted == 4){

		//Desactivamos al led
		pinLed_d = 1;
	} else if (counterSieted == 7){

		//Desactivamos al led
		pinLed_d = 1;
	} else {

		//Activamos al led
		pinLed_d = 0;
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
		pinLed_e = 0;
	} else if (counterSietee == 2){

		//activamos al led
		pinLed_e = 0;
	} else if (counterSietee == 6){

		//activamos al led
		pinLed_e = 0;
	} else if (counterSietee == 8){

		//activamos al led
		pinLed_e = 0;
	} else {

		//Desactivamos al led
		pinLed_e = 1;
	}

	//Retornamos el valor que será usado en la función writePin (activa (1) o desactiva (0) el pin)
	//dentro del counter
	return pinLed_e;
}

//NOTA IMPORTANTE: En referencia usada de siete segmentos g <--> f se intercambian (respecto a los convencionales mostrados en la guia)
//Definiendo función para el Led "f"
uint32_t counter_g(uint8_t counterSieteg){

	//Definimos variable para cargar estado de pin activo o NO activo dependiendo del número específico
	uint8_t pinLed_g = 0;

	// Definimos a continuación todos los numeros en los cuales se debe ACTIVAR o DESACTIVAR al led "f"
	if(counterSieteg == 1){

		//Desactivamos al led
		pinLed_g = 1;
	} else if (counterSieteg == 2){

		//Desactivamos al led
		pinLed_g = 1;
	} else if (counterSieteg == 3){

		//Desactivamos al led
		pinLed_g = 1;
	} else if (counterSieteg == 7){

		//Desactivamos al led
		pinLed_g = 1;
	} else {

		//Activamos al led
		pinLed_g = 0;
	}

	//Retornamos el valor que será usado en la función writePin (activa (1) o desactiva (0) el pin)
	//dentro del counter
	return pinLed_g;
}

//Definiendo función para el Led "g"
uint32_t counter_f(uint8_t counterSietef){

	//Definimos variable para cargar estado de pin activo o NO activo dependiendo del número específico
	uint8_t pinLed_f = 0;

	// Definimos a continuación todos los numeros en los cuales se debe ACTIVAR o DESACTIVAR al led "g"
	if(counterSietef == 0){

		//Desactivamos al led
		pinLed_f = 1;
	} else if (counterSietef == 1){

		//Desactivamos al led
		pinLed_f = 1;
	} else if (counterSietef == 7){

		//Desactivamos al led
		pinLed_f = 1;
	} else {

		//Activamos al led
		pinLed_f = 0;
	}

	//Retornamos el valor que será usado en la función writePin (activa (1) o desactiva (0) el pin)
	//dentro del counter
	return pinLed_f;
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
}
/*
 * Overwrite function for display del siete segmentos
 * */
void Timer3_Callback(void){

	// Cambiamos el valor de la posicion para representar todas las posiciones del numero
	// === Hacemos uso de la compuerta XOR para garantizar el cambio (0[pos] 1--> 1  and 1[pos] 1-->0 )
	posicion = posicion^maskChangeDisplay;
}
/*
 * Overwrite function for control del tiempo
 * */
void Timer5_Callback(void){

	//Subimos la bandera de la interrupción de Control Timer
	banderaControlTimer = 1;
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
