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
#include "adc_driver_hal.h"

//Definimos pines a utilizar para verificación correcto funcionamiento
GPIO_Handler_t verificationLed    = {0}; //PinA5 (Led para verificación de correcto funcionamiento)
GPIO_Handler_t stateLed           = {0}; //PinH1 (Led de estado)

//Definimos pines a utilizar para siete segmentos
GPIO_Handler_t segmentoLed_a      = {0}; //PinA11 (led "a")
GPIO_Handler_t segmentoLed_b      = {0}; //PinA12 (led "b")
GPIO_Handler_t segmentoLed_c      = {0}; //PinC12 (led "c")
GPIO_Handler_t segmentoLed_d      = {0}; //PinC11 (led "d")
GPIO_Handler_t segmentoLed_e      = {0}; //PinC10 (led "e")
GPIO_Handler_t segmentoLed_f      = {0}; //PinB12 (led "f")
GPIO_Handler_t segmentoLed_g      = {0}; //PinB7  (led "g")
GPIO_Handler_t vcc_unidad         = {0}; //PinC2
GPIO_Handler_t vcc_decena         = {0}; //PinB6
GPIO_Handler_t vcc_centena        = {0}; //PinC7
GPIO_Handler_t vcc_mil            = {0}; //PinA10

//Definimos pines a utilizar para EXTI
GPIO_Handler_t userCKenc     = {0};//Pin B2  //EXTI clock --> interrupción
GPIO_Handler_t userData      = {0};//Pin B15  //Data encoder (conociendo clock ya se conoce esta)
GPIO_Handler_t userSWenc     = {0};//Pin B1 //EXTI switch --> interrupción

//Definimos timers a utilizar
Timer_Handler_t blinkTimer   = {0}; // Timer para el blinking
Timer_Handler_t displayTimer = {0}; // Timer asociado al display del siete segmentos
Timer_Handler_t controlTimer = {0}; // Timer asociado al control del tiempo

//Definición lineas EXTI que vamos a utilizar
EXTI_Config_t swExti    = {0}; //EXTI linea 1 para el sw del encoder
EXTI_Config_t ckExti    = {0}; //EXTI linea 15 para el ck del encoder

// Definimos variable para activar representación numero en siete segmentos
uint16_t counter_i = 0;

//Definimos variable para generar cambios en el numero a representar en el siete segmentos
uint8_t numberSwitch = 0;

// Definimos variable para activar contador
uint16_t counter = 0;

// Definimos variable para contar vueltas encoder
int16_t counterEncoder = 0;

// Definimos variables para cargar características de las señales data y clock del encoder
uint8_t directionclk            = {0};
uint8_t directiondata           = {0};

// Definimos variable para tomar medida trimmer
uint16_t counterTrimmer = 2;

// Definimos variable para tomar medida Foto resistencia
uint16_t counterFotoResistencia = 3;

//Definiendo funciones a usar
uint32_t counter_a(uint8_t counterSietea);
uint32_t counter_b(uint8_t counterSieteb);
uint32_t counter_c(uint8_t counterSietec);
uint32_t counter_d(uint8_t counterSieted);
uint32_t counter_e(uint8_t counterSietee);
uint32_t counter_f(uint8_t counterSietef);
uint32_t counter_g(uint8_t counterSieteg);

//Definimos varible para cargar valor del numero a representar
uint16_t unidad  = 0;
uint16_t decena  = 0;
uint16_t centena = 0;
uint16_t mil     = 0;

//Definimos variable para activar vcc de unidad, vcc de decena, vcc de centena o vcc de mil
uint8_t posicion = 0;

//Definimos máscara para alternar la posicion unidad, decena, centena y mil
uint8_t maskChangeDisplay     = 1;

//Definimos variables para asignar el estado de la bandera correspondiente a cada interrupción
uint8_t banderaDisplayTimer   = 0;
uint8_t banderaControlTimer   = 0;
uint8_t banderaSwitchExti     = 0;
uint8_t banderaClockExti      = 0;

//Definición variable para generar apagado total de los cuatro dígitos del siete segmentos
uint8_t apagadoLed   = 1;

//Definición función para configuración inicial
void initialConfig(void);

//Definición función para configuración siete segmentos
void getDigitToShow(void);

//Definición función para configuración counter
void counterConfig(void);

//Definición función para configuración counter encoder
void counterEncoderConfig(void);

//Definición función para configuración switch
void switchConfig(void);

//Definición función para RESET de los leds
void apagadoTotalLeds(void);


/*  Main function  */
int main(void)
{
	//Llamamos función para realizar configuración inicial
	initialConfig();

    /* Loop forever */
	while(1){

		//Evaluamos si la bandera de la interrupción responsable del control del switch
		//está levantada
		if(banderaSwitchExti == 1){

			//Bajamos la bandera de la interrupción del Switch encoder
			banderaSwitchExti = 0;

			//Llamamos a la función encargada de configuración asociada al switch
			switchConfig();
		}

		//Evaluamos si la bandera de la interrupción responsable del control del display
		//está levantada
		if(banderaDisplayTimer == 1){

			//Bajamos la bandera de la interrupción de Display Timer
			banderaDisplayTimer = 0;

			//Llamamos a la función encargada de la representación del contador en el display
			getDigitToShow();
		}

		//Evaluamos si la bandera de la interrupción responsable del control del tiempo
		//está levantada
		if(banderaControlTimer == 1){

			//Bajamos la bandera de la interrupción de Control Timer
			banderaControlTimer = 0;

			//Llamamos a la función encargada del counter
			counterConfig();


			//Evaluamos si el estado del switch indica que si se debe representar el counter
			if(numberSwitch == 1){

				//Igualamos variable de counterConfig con la variable getDigitToShow
				counter_i = counter;
			}

		}

		//Evaluamos si la bandera de la interrupción responsable del counter encoder
		//está levantada
		if(banderaClockExti == 1){

			//Bajamos la bandera de la interrupción de Counter encoder
			banderaClockExti = 0;

			//Llamamos a la función encargada del counter encoder
			counterEncoderConfig();

			//Evaluamos si el estado del switch indica que si se debe representar el counter
			if(numberSwitch == 2){

				//Igualamos variable de counterConfig con la variable getDigitToShow
				counter_i = counterEncoder;
			}

		}

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
		blinkTimer.TIMx_Config.TIMx_Period           = 250;     //De la mano con el prescaler...
		blinkTimer.TIMx_Config.TIMx_mode             = TIMER_UP_COUNTER;
		blinkTimer.TIMx_Config.TIMx_InterruptEnable  = TIMER_INT_ENABLE;

		/* Configuramos el Timer */
		timer_Config(&blinkTimer);

		//Encendemos el Timer
		timer_SetState(&blinkTimer, TIMER_ON);

		/* A continuación se empieza con la configuración de los pines seleccionados para
		 * activar los LEDs de la cuenta respectiva en el 7 segmentos */

		/* Configuramos el pin A11 --> LED a*/
		segmentoLed_a.pGPIOx                         = GPIOA;
		segmentoLed_a.pinConfig.GPIO_PinNumber       = PIN_11;
		segmentoLed_a.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		segmentoLed_a.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		segmentoLed_a.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		segmentoLed_a.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&segmentoLed_a);

		//A continuación se está probando el correcto funcionamiento del pin A11
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

		/* Configuramos el pin C12 --> LED c*/
		segmentoLed_c.pGPIOx                         = GPIOC;
		segmentoLed_c.pinConfig.GPIO_PinNumber       = PIN_12;
		segmentoLed_c.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		segmentoLed_c.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		segmentoLed_c.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		segmentoLed_c.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&segmentoLed_c);

		//A continuación se está probando el correcto funcionamiento del pin C12
		//gpio_WritePin(&segmentoLed_c, SET);

		/* Configuramos el pin C11 --> LED d*/
		segmentoLed_d.pGPIOx                         = GPIOC;
		segmentoLed_d.pinConfig.GPIO_PinNumber       = PIN_11;
		segmentoLed_d.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		segmentoLed_d.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		segmentoLed_d.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		segmentoLed_d.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&segmentoLed_d);

		//A continuación se está probando el correcto funcionamiento del pin C11
		//gpio_WritePin(&segmentoLed_d, SET);

		/* Configuramos el pin C10 --> LED e*/
		segmentoLed_e.pGPIOx                         = GPIOC;
		segmentoLed_e.pinConfig.GPIO_PinNumber       = PIN_10;
		segmentoLed_e.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		segmentoLed_e.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		segmentoLed_e.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		segmentoLed_e.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&segmentoLed_e);

		//A continuación se está probando el correcto funcionamiento del pin C10
		//gpio_WritePin(&segmentoLed_e, SET);

		/* Configuramos el pin B12 --> LED f*/
		segmentoLed_f.pGPIOx                         = GPIOB;
		segmentoLed_f.pinConfig.GPIO_PinNumber       = PIN_12;
		segmentoLed_f.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		segmentoLed_f.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		segmentoLed_f.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		segmentoLed_f.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&segmentoLed_f);

		//A continuación se está probando el correcto funcionamiento del pin B12
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

		//A continuación se está probando el correcto funcionamiento del pin B7
		//gpio_WritePin(&segmentoLed_g, SET);

		//Cargamos ahora la configuración respectiva para los pines de alimentación de los vcc
		//de los transistores que componen el circuito del siete segmentos.

		/* Configuramos el pin C2 --> vcc unidad*/
		vcc_unidad.pGPIOx                         = GPIOC;
		vcc_unidad.pinConfig.GPIO_PinNumber       = PIN_2;
		vcc_unidad.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		vcc_unidad.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		vcc_unidad.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		vcc_unidad.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&vcc_unidad);

		/* Configuramos el pin B6 --> vcc decimal*/
		vcc_decena.pGPIOx                         = GPIOB;
		vcc_decena.pinConfig.GPIO_PinNumber       = PIN_6;
		vcc_decena.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		vcc_decena.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		vcc_decena.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		vcc_decena.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&vcc_decena);

		/* Configuramos el pin C7 --> vcc CENTENA*/
		vcc_centena.pGPIOx                         = GPIOC;
		vcc_centena.pinConfig.GPIO_PinNumber       = PIN_7;
		vcc_centena.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		vcc_centena.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		vcc_centena.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		vcc_centena.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&vcc_centena);

		/* Configuramos el pin A10 --> vcc MIL*/
		vcc_mil.pGPIOx                         = GPIOA;
		vcc_mil.pinConfig.GPIO_PinNumber       = PIN_10;
		vcc_mil.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		vcc_mil.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		vcc_mil.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		vcc_mil.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&vcc_mil);

		//A continuación se está realizando la configuración de los timers a usar

		//Configuración Timer3 --> display del siete segmentos
		displayTimer.pTIMx                             = TIM3;
		displayTimer.TIMx_Config.TIMx_Prescaler        = 16000;
		displayTimer.TIMx_Config.TIMx_Period           = 2.5; //Debido a que el código de la representación de los
		                                                    //numeros en unidad y decena SIEMPRE tiene una pausa intermedia
		                                                    //para apagar totalmente y asi evitar fantasmas... el código recorre
		                                                    //el análisis de representación de unidad, decena, centena y mil en UN SEGUNDO,
		                                                    //sin embargo, necesitamos entonces que la velocidad del timer
		                                                    //sea de 1/4 segundo para que el numero completo sea representado
		                                                    //en un segundo.
		displayTimer.TIMx_Config.TIMx_mode             = TIMER_UP_COUNTER;
		displayTimer.TIMx_Config.TIMx_InterruptEnable  = TIMER_INT_ENABLE;

		/* Configuramos el Timer */
		timer_Config(&displayTimer);

		//Encendemos el Timer
		timer_SetState(&displayTimer, TIMER_ON);

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

		/*Configuramos el pin B15 -> DATA ENCODER*/
		userData.pGPIOx                         = GPIOB;
		userData.pinConfig.GPIO_PinNumber       = PIN_15;
		userData.pinConfig.GPIO_PinMode         = GPIO_MODE_IN;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&userData);

		/*Configuramos el pin..  --> SWITCH ENCODER*/
		userSWenc.pGPIOx                         = GPIOB;
		userSWenc.pinConfig.GPIO_PinNumber       = PIN_1;
		userSWenc.pinConfig.GPIO_PinMode         = GPIO_MODE_IN;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&userSWenc);

		/*Configuramos el pin B15  --> CLOCK ENCODER*/
		userCKenc.pGPIOx                         = GPIOB;
		userCKenc.pinConfig.GPIO_PinNumber       = PIN_2;
		userCKenc.pinConfig.GPIO_PinMode         = GPIO_MODE_IN;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&userCKenc);

		//A continuación se está realizando la configuración de los EXTI a usar

		/*Configuramos el EXTI sw que será en la linea 1--> Switch*/
		swExti.pGPIOHandler = &userSWenc;
		swExti.edgeType     = EXTERNAL_INTERRUPT_RISING_EDGE;

		//Cargamos la configuración de la interrupción externa (EXTI)
		exti_Config(&swExti);

		/*Configuramos el EXTI ck que será en la linea 2 --> Clock*/
		ckExti.pGPIOHandler = &userCKenc;
		ckExti.edgeType     = EXTERNAL_INTERRUPT_RISING_EDGE;

		//Cargamos la configuración de la interrupción externa (EXTI)
		exti_Config(&ckExti);

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

//Definiendo función para el Led "f"
uint32_t counter_f(uint8_t counterSietef){

	//Definimos variable para cargar estado de pin activo o NO activo dependiendo del número específico
	uint8_t pinLed_f = 0;

	// Definimos a continuación todos los numeros en los cuales se debe ACTIVAR o DESACTIVAR al led "f"
	if(counterSietef == 1){

		//Desactivamos al led
		pinLed_f = 1;
	} else if (counterSietef == 2){

		//Desactivamos al led
		pinLed_f = 1;
	} else if (counterSietef == 3){

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

//Definiendo función para el Led "g"
uint32_t counter_g(uint8_t counterSieteg){

	//Definimos variable para cargar estado de pin activo o NO activo dependiendo del número específico
	uint8_t pinLed_g = 0;

	// Definimos a continuación todos los numeros en los cuales se debe ACTIVAR o DESACTIVAR al led "g"
	if(counterSieteg == 0){

		//Desactivamos al led
		pinLed_g = 1;
	} else if (counterSieteg == 1){

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

//Función para apagar los leds manualmente para garantizar NO aparezcan fantasmas
void apagadoTotalLeds(void){

	gpio_WritePin(&segmentoLed_a, SET);
	gpio_WritePin(&segmentoLed_b, SET);
	gpio_WritePin(&segmentoLed_c, SET);
	gpio_WritePin(&segmentoLed_d, SET);
	gpio_WritePin(&segmentoLed_e, SET);
	gpio_WritePin(&segmentoLed_f, SET);
	gpio_WritePin(&segmentoLed_g, SET);
}

//Función para configuración siete segmentos
void getDigitToShow(void){

	// Construimos relación para identificar el valor de mil del número
	mil = counter_i - (counter_i%1000);

	// Construimos relación para identificar el valor de centena del número
	centena = (counter_i%1000) - ((counter_i%1000)%100);

	// Construimos relación para identificar el valor de la decena del número
	decena = ((counter_i%1000)%100) - (((counter_i%1000)%100)%10);

	// Construimos relación para identificar el valor de la unidad del número
	unidad = (((counter_i%1000)%100)%10);

	// Generamos condicional para representar numero en posición unidad o decena del siete segmentos
	//La dinámica de funcionemiento hace uso del cambio de una variable al
	//hacer uso de un operador XOR, lo cual alternará entre cinco posibles valores
	// (siempre y cuando se ubiquen estrategicamente las aignaciones de cada variable)
	//que condicionarán los estados de representación para MIL, CENTENA, DECENA, UNIDAD y TOTAL APAGADO

	// Condición para representar decena:
	if(posicion == 2){

		//Desactivamos vcc del siete segmentos para mil, centena, decena y unidad
		// *****Esto para evitar aparición de fantasmas
		// == NOTA importante: Debido a que el siete segmentos a utilizar es de ánodo común
		// == entonces necesitamos generar conexión a tierra, en lugar de alimentación, para
		// == lograr la activación de los mismos pines, es decir que en este caso
		// == ponemos SET para desactivar y RESET para activar

		//DESACTIVACIÓN DE VCC:
		gpio_WritePin(&vcc_unidad, SET);
		gpio_WritePin(&vcc_decena, SET);
		gpio_WritePin(&vcc_centena, SET);
		gpio_WritePin(&vcc_mil, SET);

		//Ejecutamos la configuración de los pines para la DECENA
		gpio_WritePin(&segmentoLed_a, counter_a(decena/10));
		gpio_WritePin(&segmentoLed_b, counter_b(decena/10));
		gpio_WritePin(&segmentoLed_c, counter_c(decena/10));
		gpio_WritePin(&segmentoLed_d, counter_d(decena/10));
		gpio_WritePin(&segmentoLed_e, counter_e(decena/10));
		gpio_WritePin(&segmentoLed_f, counter_f(decena/10));
		gpio_WritePin(&segmentoLed_g, counter_g(decena/10));

		//Activamos vcc del siete segmentos correspondiente a las decenas
		gpio_WritePin(&vcc_decena, RESET);

		apagadoLed = 2; //Con este valor se garantiza que la posicion, despues de
		                //pasar por el apagado y el posterior cambio de valor con
		                //el uso de la máscara, será 3...y el código representará centenas

		posicion = 0;  //Con este valor se garantiza que al usar el cambio
		               //de valor con la máscara el código entre al condicional
		               //de apagado (posicion será igual a 1)

	} else if (posicion == 1){ //Condición para desactivar todos los LEDs

		//Apagamos TODOS los leds que puedan estar encendidos en el momento
		//**** Esto para evitar la aparición de fantasmas
		//DESACTIVACIÓN DE LOS LEDS DIRECTAMENTE:
		apagadoTotalLeds();

		posicion = apagadoLed; //La alternación de apagadoLed permitirá la entrada
		                       //del código a todos los dígitos del siete segmentos

	} else if (posicion == 3){

		//Desactivamos vcc del siete segmentos para mil, centena, decena y unidad.
		// *****Esto para evitar aparición de fantasmas
		// == NOTA importante: Debido a que el siete segmentos a utilizar es de ánodo común
		// == entonces necesitamos generar conexión a tierra, en lugar de alimentación, para
		// == lograr la activación de los mismos pines, es decir que en este caso
		// == ponemos SET para desactivar y RESET para activar

		//DESACTIVACIÓN DE VCC:
		gpio_WritePin(&vcc_unidad, SET);
		gpio_WritePin(&vcc_decena, SET);
		gpio_WritePin(&vcc_centena, SET);
		gpio_WritePin(&vcc_mil, SET);

		//Ejecutamos la configuración de los pines para la CENTENA
		gpio_WritePin(&segmentoLed_a, counter_a(centena/100));
		gpio_WritePin(&segmentoLed_b, counter_b(centena/100));
		gpio_WritePin(&segmentoLed_c, counter_c(centena/100));
		gpio_WritePin(&segmentoLed_d, counter_d(centena/100));
		gpio_WritePin(&segmentoLed_e, counter_e(centena/100));
		gpio_WritePin(&segmentoLed_f, counter_f(centena/100));
		gpio_WritePin(&segmentoLed_g, counter_g(centena/100));

		//Activamos vcc del siete segmentos correspondiente a las decenas
		gpio_WritePin(&vcc_centena, RESET);

		apagadoLed = 5; //Agregar esta condición ayuda en ajuste para entrada
                        //del código al mil
		posicion = 0; //Con este valor se garantiza que al usar el cambio
		              //de valor con la máscara el código entre al condicional
			          //de apagado (posicion será igual a 1)

	} else if (posicion == 4){

		//Desactivamos vcc del siete segmentos para mil, centena, decena y unidad.
		// *****Esto para evitar aparición de fantasmas
		// == NOTA importante: Debido a que el siete segmentos a utilizar es de ánodo común
		// == entonces necesitamos generar conexión a tierra, en lugar de alimentación, para
		// == lograr la activación de los mismos pines, es decir que en este caso
		// == ponemos SET para desactivar y RESET para activar

		//DESACTIVACIÓN DE VCC:
		gpio_WritePin(&vcc_unidad, SET);
		gpio_WritePin(&vcc_decena, SET);
		gpio_WritePin(&vcc_centena, SET);
		gpio_WritePin(&vcc_mil, SET);

		//Ejecutamos la configuración de los pines para la MIL
		gpio_WritePin(&segmentoLed_a, counter_a(mil/1000));
		gpio_WritePin(&segmentoLed_b, counter_b(mil/1000));
		gpio_WritePin(&segmentoLed_c, counter_c(mil/1000));
		gpio_WritePin(&segmentoLed_d, counter_d(mil/1000));
		gpio_WritePin(&segmentoLed_e, counter_e(mil/1000));
		gpio_WritePin(&segmentoLed_f, counter_f(mil/1000));
		gpio_WritePin(&segmentoLed_g, counter_g(mil/1000));

		//Activamos vcc del siete segmentos correspondiente a las decenas
		gpio_WritePin(&vcc_mil, RESET);

		apagadoLed = 1; //Agregar esta condición ayuda en ajuste para entrada
                       //del código a las unidades
		posicion = 0; //Con este valor se garantiza que al usar el cambio
                      //de valor con la máscara el código entre al condicional
                     //de apagado (posicion será igual a 1)

	} else if (posicion == 0){

		//Desactivamos vcc del siete segmentos para mil, centena, decena y unidad
		// *****Esto para evitar aparición de fantasmas
		// == NOTA importante: Debido a que el siete segmentos a utilizar es de ánodo común
		// == entonces necesitamos generar conexión a tierra, en lugar de alimentación, para
		// == lograr la activación de los mismos pines, es decir que en este caso
		// == ponemos SET para desactivar y RESET para activar

		//DESACTIVACIÓN DE VCC:
		gpio_WritePin(&vcc_unidad, SET);
		gpio_WritePin(&vcc_decena, SET);
		gpio_WritePin(&vcc_centena, SET);
		gpio_WritePin(&vcc_mil, SET);

		//Ejecutamos la configuración de los pines para la UNIDAD
		gpio_WritePin(&segmentoLed_a, counter_a(unidad));
		gpio_WritePin(&segmentoLed_b, counter_b(unidad));
		gpio_WritePin(&segmentoLed_c, counter_c(unidad));
		gpio_WritePin(&segmentoLed_d, counter_d(unidad));
		gpio_WritePin(&segmentoLed_e, counter_e(unidad));
		gpio_WritePin(&segmentoLed_f, counter_f(unidad));
		gpio_WritePin(&segmentoLed_g, counter_g(unidad));

		//Activamos vcc del siete segmentos correspondiente a las decenas
		gpio_WritePin(&vcc_unidad, RESET);

		apagadoLed = 3; //Agregar esta condición ayuda en ajuste para entrada
		               //del código a las decenas
	}

	// Cambiamos el valor de la posicion para representar todas las posiciones del numero
	// === Hacemos uso de la compuerta XOR para garantizar el cambio (0[pos] 1--> 1  and 1[pos] 1-->0 )
	posicion = posicion^maskChangeDisplay;
}

//Función para configuración counter
void counterConfig(void){
	//Sumamos el valor del counter para garantizar la cuenta ascentente
	counter = counter +1;

	//Delimitamos que el número máximo hasta el cual se contará es 59
	if(counter == 4096){
		//Reiniciamos el contador para repetir el ciclo de cuenta
		counter = 0;
	}
}

//Función para configuración counter encoder
void counterEncoderConfig(void){

	//Almacenamos la informacion recibida por los datos de la señal clock y la señal data
	directionclk = gpio_ReadPin(&userCKenc);
	directiondata = gpio_ReadPin(&userData);

	//Definimos condiciones para diferenciación entre suma y resta de la cantidad de vueltas
	if(directionclk == directiondata){ //Verificamos condición giro derecha

		//Sumamos al contador para empezar a sumar con cada vuelta
		counterEncoder++;

		//Verificamos si el valor sobrepasó el maximo de representación
		if(counterEncoder == 4096){
			//En caso de alcanzar el máximo se reinicia la cuenta desde cero
			counterEncoder = 0;
		}

	} else{ // Condición giro izquierda

		//Restamos al contador para empezar a restar con cada vuelta
		counterEncoder--;

		//Verificamos si el valor sobrepasó el minimo de representación
		if(counterEncoder == -1){
			//En caso de alcanzar el mÍNIMO se reinicia la cuenta desde máximo
			counterEncoder = 4095;
		}
	}
}

//Función para configuración switch
void switchConfig(void){

	//Aumentamos el valor de la variable con cada activación del switch
	//==== Cada numero asignado va a representar un caso especifico de representación en el siete segmentos
	numberSwitch ++;

	//Definimos límite del contador en 5 casos posibles
	/*
	 * 0. No realiza ninguna función
	 * 1. Contador de tiempo
	 * 2. Contador vueltas encoder
	 * 3. Medida trimmer
	 * 4. Medida Foto Resistencia
	 * */
	if(numberSwitch == 3){

		numberSwitch = 0;
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
}
/*
 * Overwrite function for display del siete segmentos
 * */
void Timer3_Callback(void){

	//Subimos la bandera de la interrupción de Display Timer
	banderaDisplayTimer = 1;
}
/*
 * Overwrite function for control del tiempo
 * */
void Timer5_Callback(void){

	//Subimos la bandera de la interrupción de Control Timer
	banderaControlTimer = 1;
}
/*
 * Overwrite function for switch
 * */
void callback_ExtInt1(void){

	//Activamos bandera de la interrupción
	banderaSwitchExti = 1;
}
/*
 * Overwrite function for clock
 * */
void callback_ExtInt2(void){

	//Activamos bandera de la interrupción
	banderaClockExti = 1;
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
