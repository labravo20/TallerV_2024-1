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

//Definición de pines a utilizar para S2 y S3 sensor --> Seleccionan qué color se va a analizar
/*NOTA: Los pines asociados a S0 y S1 siempre mantendrán los valores de HIGH and LOW, respectivamente
 * para asegurar que siempre se trabajará con un escalamiento del 20% de la frecuencia.*/
GPIO_Handler_t sensorPinS2        = {0}; //Pin B5
GPIO_Handler_t sensorPinS3        = {0}; //PIn A10

//Definimos estructura para programar casos de análisis R,G o B
enum{
	FILTRO_RED    = 0,
	FILTRO_GREEN,
	FILTRO_BLUE
};

//Definimos pines a utilizar para EXTI
GPIO_Handler_t userOutputSensor   = {0};//Pin B2  //EXTI output sensor RGB --> interrupción

//Definimos pines a utilizar para USART
GPIO_Handler_t   userUsart2Tx     = {0};//Pin A2 //USART pin de transmisón
GPIO_Handler_t   userUsart2Rx     = {0};//Pin A3 //USART pin de recepción

//Definimos timers a utilizar
Timer_Handler_t blinkTimer        = {0}; // Timer para el blinking
Timer_Handler_t controlTimer      = {0}; // Timer asociado al control del tiempo entre mediciones del sensor --> Generamos delay de 200ms
Timer_Handler_t pulseTimer        = {0}; // Timer asociado a contador del ancho de pulso de la señal PWM

//Definición lineas EXTI que vamos a utilizar
EXTI_Config_t outputSensorExti    = {0}; //EXTI linea 2 para el output del sensor RGB

//Definimos USART a usar
USART_Handler_t   usart2          = {0};

//Definimos el caracter para ejecución del USART
char bufferMsg[128]               = {0};

//Definimos variable para recibir el valor del received char en análisis USART
uint8_t   receivedChar            = 0;

// Definimos variable para contar rising edges en señal PWM output sensor
uint16_t counterOutputSensorR      = 0;
uint16_t counterOutputSensorG      = 0;
uint16_t counterOutputSensorB      = 0;

// Definimos variable para contar tiempo en señal PWM output sensor
uint16_t counterPeriodR            = 0;
uint16_t counterPeriodG            = 0;
uint16_t counterPeriodB            = 0;

//Definimos variable para garantizar medida de ancho pulso en cada color
uint8_t counterMeasureR            = 0;
uint8_t counterMeasureG            = 0;
uint8_t counterMeasureB            = 0;

//Definición variable para delay
uint16_t counterDelay             = 0;

//Definimos variables para asignar el estado de la bandera correspondiente a cada interrupción
uint8_t banderaControlTimer       = 0;
uint8_t banderaOutputSensorExti   = 0;
uint8_t banderaPulseTimer         = 0;
uint8_t banderaUSARTTx            = 0;
uint8_t banderaUSARTRx            = 0;

// Definimos variables herramienta para determinar el ancho de pulso del output del sensor RGB
uint16_t pulseOutputSensorR        = 0;
uint16_t pulseOutputSensorG        = 0;
uint16_t pulseOutputSensorB        = 0;
uint16_t pulseWidth               = 0;
uint16_t pulseWidthRed            = 0;
uint16_t pulseWidthGreen          = 0;
uint16_t pulseWidthBlue           = 0;

//Definición función para configuración inicial
void initialConfig(void);

//Definición función para generar DELAY
void delay(void);

//Definición función para definir filtros de color a utilizar
void sensorConfig(uint8_t filtroColor);

//Definición función para contar rising edges de output sensor RGB
void counterOutputSensorConfigR(void);
void counterOutputSensorConfigG(void);
void counterOutputSensorConfigB(void);

//Definición de función para counter timer
void counterTimerPulseR(void);
void counterTimerPulseG(void);
void counterTimerPulseB(void);

//Definición de función para comunicación de datos en usart
void msgUsart(void);

//Definición función para determinar el periodo de la señal PWM
uint16_t pulseOutputSensorConfigR(void);
uint16_t pulseOutputSensorConfigG(void);
uint16_t pulseOutputSensorConfigB(void);

//Definición función para obtener el ancho del pulso del colo RGB en estudio
void getPulseWidthRed(void);
void getPulseWidthGreen(void);
void getPulseWidthBlue(void);

/*  Main function  */
int main(void)
{
	//Llamamos función para realizar configuración inicial
	initialConfig();

    /* Loop forever */
	while(1){

		//Llamamos a función encargada de obtener el valor del ancho de pulso del colo ROJO
		getPulseWidthRed();

		//Generamos delay entre medición
		//delay();

		//Llamamos a función encargada de obtener el valor del ancho de pulso del colo ROJO
		getPulseWidthGreen();

		//Generamos delay entre medición
		//delay();

		//Llamamos a función encargada de obtener el valor del ancho de pulso del colo ROJO
		getPulseWidthBlue();

		//Generamos delay entre medición
		//delay();

		//Llamamos a la función encargada de representación en USART
		//msgUsart();


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
		blinkTimer.TIMx_Config.TIMx_Period           = 1000;    //Periodo asociado a 1s
		blinkTimer.TIMx_Config.TIMx_mode             = TIMER_UP_COUNTER;
		blinkTimer.TIMx_Config.TIMx_InterruptEnable  = TIMER_INT_ENABLE;

		/* Configuramos el Timer */
		timer_Config(&blinkTimer);

		//Encendemos el Timer
		timer_SetState(&blinkTimer, TIMER_ON);

		//A continuación se está realizando la configuración de los timers a usar

		//Configuración Timer5 --> control del tiempo (DELAY entre mediciones de color)
		controlTimer.pTIMx                             = TIM5;
		controlTimer.TIMx_Config.TIMx_Prescaler        = 16000;  //Genera incrementos de 0.1 s
		controlTimer.TIMx_Config.TIMx_Period           = 1000;    //Periodo asociado a 1s
		controlTimer.TIMx_Config.TIMx_mode             = TIMER_UP_COUNTER;
		controlTimer.TIMx_Config.TIMx_InterruptEnable  = TIMER_INT_ENABLE;

		/* Configuramos el Timer */
		timer_Config(&controlTimer);

		//Encendemos el Timer
		timer_SetState(&controlTimer, TIMER_ON);

		//Configuración Timer3 --> control del tiempo
		pulseTimer.pTIMx                             = TIM3;
		pulseTimer.TIMx_Config.TIMx_Prescaler        = 160;  //Genera incrementos de 0.1 s
		pulseTimer.TIMx_Config.TIMx_Period           = 100;    //Periodo asociado a 0.001s
		pulseTimer.TIMx_Config.TIMx_mode             = TIMER_UP_COUNTER;
		pulseTimer.TIMx_Config.TIMx_InterruptEnable  = TIMER_INT_ENABLE;

		/* Configuramos el Timer */
		timer_Config(&pulseTimer);

		//Encendemos el Timer
		timer_SetState(&pulseTimer, TIMER_ON);

		//A continuación se configuran los pines asociados a la selección de color a analizar

		/* Configuramos el pin S2 */
		sensorPinS2.pGPIOx                         = GPIOB;
		sensorPinS2.pinConfig.GPIO_PinNumber       = PIN_5;
		sensorPinS2.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		sensorPinS2.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		sensorPinS2.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		sensorPinS2.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&sensorPinS2);

		/* Configuramos el pin S3 */
		sensorPinS3.pGPIOx                         = GPIOA;
		sensorPinS3.pinConfig.GPIO_PinNumber       = PIN_10;
		sensorPinS3.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		sensorPinS3.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
		sensorPinS3.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
		sensorPinS3.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

		//Cargamos la configuración en los registros que gobiernan el puerto
		gpio_Config(&sensorPinS3);

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

//Función para definir filtros de color a utilizar
void sensorConfig(uint8_t filtroColor){

	//Evaluamos cuál es la configuración de los filtros para el color a analizar
	switch(filtroColor){

	case FILTRO_RED:{

		//Configuración de pines S2 y S3 para analizar color ROJO
		gpio_WritePin(&sensorPinS2, RESET);
		gpio_WritePin(&sensorPinS3, RESET);

		break;
	}

	case FILTRO_GREEN:{

		//Configuración de pines S2 y S3 para analizar color VERDE
		gpio_WritePin(&sensorPinS2, SET);
		gpio_WritePin(&sensorPinS3, SET);

		break;
	}

	case FILTRO_BLUE:{

		//Configuración de pines S2 y S3 para analizar color AZUL
		gpio_WritePin(&sensorPinS2, RESET);
		gpio_WritePin(&sensorPinS3, SET);

		break;
	}

	default:{

		break;
	}
	}//Fin sel switch case

}//Fin de la función

//Función para contar rising edges de output sensor RGB
void counterOutputSensorConfigR(void){

	//Evaluamos si la bandera asociada a la interrupción está activa
	if(banderaOutputSensorExti){

		//Sumamos el valor de un contador que se irá sumado cada vez que la señal PWM tenga un rising edge
		counterOutputSensorR++;

		//Definimos límite de la suma para reiniciar contador
		if(counterOutputSensorR == 4){

			//En caso superarse el límite el contador se reinicia
			counterOutputSensorR = 0;

		}//Fin del condicional

		//Bajamos la bandera
		banderaOutputSensorExti = 0;

	} //Fin del condicional

}//Fin de la función
/**/
//Función para contar rising edges de output sensor RGB
void counterOutputSensorConfigG(void){

	//Evaluamos si la bandera asociada a la interrupción está activa
	if(banderaOutputSensorExti){

		//Sumamos el valor de un contador que se irá sumado cada vez que la señal PWM tenga un rising edge
		counterOutputSensorG++;

		//Definimos límite de la suma para reiniciar contador
		if(counterOutputSensorG == 4){

			//En caso superarse el límite el contador se reinicia
			counterOutputSensorG = 0;

		}//Fin del condicional

		//Bajamos la bandera
		banderaOutputSensorExti = 0;

	} //Fin del condicional

}//Fin de la función
/**/
//Función para contar rising edges de output sensor RGB
void counterOutputSensorConfigB(void){

	//Evaluamos si la bandera asociada a la interrupción está activa
	if(banderaOutputSensorExti){

		//Sumamos el valor de un contador que se irá sumado cada vez que la señal PWM tenga un rising edge
		counterOutputSensorB++;

		//Definimos límite de la suma para reiniciar contador
		if(counterOutputSensorB == 4){

			//En caso superarse el límite el contador se reinicia
			counterOutputSensorB = 0;

		}//Fin del condicional

		//Bajamos la bandera
		banderaOutputSensorExti = 0;

	} //Fin del condicional

}//Fin de la función

//Función para counter timer
void counterTimerPulseR(void){

	//Verificamos si la bandera del timer está activa
	if(banderaPulseTimer){

		//Empezamos cuenta del tiempo
		/*El valor correspondiente se interpreta planteando que 1 UNIDAD en counter Period equivale a 1ms*/
		counterPeriodR++;

		//Bajamos la bandera
		banderaPulseTimer = 0;
	}
}
/**/
//Función para counter timer
void counterTimerPulseG(void){

	//Verificamos si la bandera del timer está activa
	if(banderaPulseTimer){

		//Empezamos cuenta del tiempo
		/*El valor correspondiente se interpreta planteando que 1 UNIDAD en counter Period equivale a 1ms*/
		counterPeriodG++;

		//Bajamos la bandera
		banderaPulseTimer = 0;
	}
}
/**/
//Función para counter timer
void counterTimerPulseB(void){

	//Verificamos si la bandera del timer está activa
	if(banderaPulseTimer){

		//Empezamos cuenta del tiempo
		/*El valor correspondiente se interpreta planteando que 1 UNIDAD en counter Period equivale a 1ms*/
		counterPeriodB++;

		//Bajamos la bandera
		banderaPulseTimer = 0;
	}
}


//Definición de función para comunicación de datos en usart
void msgUsart(void){

	//Evaluamos si la bandera asociada a la transmisión por USART está activa
	if(banderaUSARTTx){

		//Escribimos mensaje con los datos de anchos de pulso de las señales
		sprintf(bufferMsg,"Valores de ancho de pulso de cada color: R = %d ms, G = %d ms, B = %d ms \n\r",pulseWidthRed,pulseWidthGreen,pulseWidthBlue);
		usart_writeMsg(&usart2, bufferMsg);

		//Bajamos la bandera
		banderaUSARTTx = 0;

	}
}

//Función para determinar el periodo de la señal PWM
uint16_t pulseOutputSensorConfigR(void){

	//Evaluamos inicio de contador del periodo cuando se mide el segundo rising edge
	if(counterOutputSensorR == 2){

		//Se inicia función para counter timer para determinar el periodo
		counterTimerPulseR();
	}
	else if(counterOutputSensorR == 3){

		//Asignamos el valor del periodo a una variable que retornará la función
		pulseOutputSensorR = counterPeriodR;
	}
	else{

		//Reiniciamos el counter del timer para no sobrepasar el valor correspondiente al periodo
		counterPeriodR = 0;
	}

	//Retornamos variable asociada al ancho del pulso de la señal PWM en unidad de ms
	return pulseOutputSensorR;

}//Fin de la función
/**/
//Función para determinar el periodo de la señal PWM
uint16_t pulseOutputSensorConfigG(void){

	//Evaluamos inicio de contador del periodo cuando se mide el segundo rising edge
	if(counterOutputSensorG == 2){

		//Se inicia función para counter timer para determinar el periodo
		counterTimerPulseG();
	}
	else if(counterOutputSensorG == 3){

		//Asignamos el valor del periodo a una variable que retornará la función
		pulseOutputSensorG = counterPeriodG;
	}
	else{

		//Reiniciamos el counter del timer para no sobrepasar el valor correspondiente al periodo
		counterPeriodG = 0;
	}

	//Retornamos variable asociada al ancho del pulso de la señal PWM en unidad de ms
	return pulseOutputSensorG;

}//Fin de la función
/**/
//Función para determinar el periodo de la señal PWM
uint16_t pulseOutputSensorConfigB(void){

	//Evaluamos inicio de contador del periodo cuando se mide el segundo rising edge
	if(counterOutputSensorB == 2){

		//Se inicia función para counter timer para determinar el periodo
		counterTimerPulseB();
	}
	else if(counterOutputSensorB == 3){

		//Asignamos el valor del periodo a una variable que retornará la función
		pulseOutputSensorB = counterPeriodB;
	}
	else{

		//Reiniciamos el counter del timer para no sobrepasar el valor correspondiente al periodo
		counterPeriodB = 0;
	}

	//Retornamos variable asociada al ancho del pulso de la señal PWM en unidad de ms
	return pulseOutputSensorB;

}//Fin de la función

//Función para obtener el ancho del pulso del colo RGB en estudio
void getPulseWidthRed(void){

//	//Configuramos filtro rojo para análisis del sensor RGB
	sensorConfig(FILTRO_RED);

	counterMeasureR = 1;

	while(counterMeasureR != 0){

		//Inicializamos función para empezar a contar los rising edges de la señal en estudio
		counterOutputSensorConfigR();

		//Llamamos función para establecer el periodo de la señal PWM del color en análisis
		//Realizamos conversión para encontrar el valor del ancho del pulso (en ms)
		/* RECORDAR--> Duty de la señal es siempre del 50% */


		pulseWidthRed = (pulseOutputSensorConfigR())/2;

		if(counterOutputSensorR == 3){

			counterMeasureR = 0;
		}
	}

}
/**/
//Función para obtener el ancho del pulso del colo RGB en estudio
void getPulseWidthGreen(void){

//	//Configuramos filtro rojo para análisis del sensor RGB
	sensorConfig(FILTRO_GREEN);

	counterMeasureG = 1;

	while(counterMeasureG != 0){

		//Inicializamos función para empezar a contar los rising edges de la señal en estudio
		counterOutputSensorConfigG();

		//Llamamos función para establecer el periodo de la señal PWM del color en análisis
		//Realizamos conversión para encontrar el valor del ancho del pulso (en ms)
		/* RECORDAR--> Duty de la señal es siempre del 50% */


		pulseWidthGreen = (pulseOutputSensorConfigG())/2;

		if(counterOutputSensorG == 3){

			counterMeasureG = 0;
		}
	}

}
/**/
//Función para obtener el ancho del pulso del colo RGB en estudio
void getPulseWidthBlue(void){

//	//Configuramos filtro rojo para análisis del sensor RGB
	sensorConfig(FILTRO_BLUE);

	counterMeasureB = 1;

	while(counterMeasureB != 0){

		//Inicializamos función para empezar a contar los rising edges de la señal en estudio
		counterOutputSensorConfigB();

		//Llamamos función para establecer el periodo de la señal PWM del color en análisis
		//Realizamos conversión para encontrar el valor del ancho del pulso (en ms)
		/* RECORDAR--> Duty de la señal es siempre del 50% */


		pulseWidthBlue = (pulseOutputSensorConfigB())/2;

		if(counterOutputSensorB == 3){

			counterMeasureB = 0;
		}
	}

}

//Definición función para generar DELAY
void delay(void){

	//Verificamos si la bandera del timer asociado al delay (control timer) está encendido
	if(banderaControlTimer){

		while(counterDelay < 11){

			counterDelay++;
		}

		//Reiniciamos contador del delay
		counterDelay = 0;

		//Bajamos la bandera
		banderaControlTimer = 0;
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
 * Overwrite function for pulse timer
 * */
void Timer3_Callback(void){

	//Activamos bandera correspondiente a interrupción para pulse timer
	banderaPulseTimer = 1;
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
