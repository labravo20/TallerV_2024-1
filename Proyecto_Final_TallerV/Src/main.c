/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Laura Sofia Bravo Revelo (labravo)
 * @brief          : Proyecto Final Taller V
 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32_assert.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"
#include "exti_driver_hal.h"
#include "usart_driver_hal.h"
#include "pwm_driver_hal.h"

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

//Definimos pines a utilizar para PWM
GPIO_Handler_t   pinPWMChannel   = {0};

//Definimos timers a utilizar
Timer_Handler_t blinkTimer        = {0}; // Timer para el blinking
Timer_Handler_t controlTimer      = {0}; // Timer asociado al control del tiempo entre mediciones del sensor --> Generamos delay de 200ms
Timer_Handler_t pulseTimer        = {0}; // Timer asociado a contador del ancho de pulso de la señal PWM

//Definición de canal PWM a usar
PWM_Handler_t    signalPWM       = {0};

//Definición lineas EXTI que vamos a utilizar
EXTI_Config_t outputSensorExti    = {0}; //EXTI linea 2 para el output del sensor RGB

//Definimos USART a usar
USART_Handler_t   usart2          = {0};

//Definimos el caracter para ejecución del USART
char bufferMsg[128]               = {0};

//Definimos variable para recibir el valor del received char en análisis USART
uint8_t   receivedChar            = 0;

// Definimos variables para contar rising edges en señal PWM output sensor
uint16_t counterOutputSensorR      = 0;
uint16_t counterOutputSensorG      = 0;
uint16_t counterOutputSensorB      = 0;

// Definimos variables para contar tiempo en señal PWM output sensor
uint16_t counterPeriodR            = 0;
uint16_t counterPeriodG            = 0;
uint16_t counterPeriodB            = 0;

//Definimos variables para garantizar medida de ancho pulso en cada color
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

// Definimos variables herramienta para determinar el periodo del output del sensor RGB
uint32_t pulseOutputSensorR        = 0;
uint32_t pulseOutputSensorG        = 0;
uint32_t pulseOutputSensorB        = 0;

// Definimos variables herramienta para determinar el ancho de pulso del output del sensor RGB
uint16_t pulseWidthRed            = 0;
uint16_t pulseWidthGreen          = 0;
uint16_t pulseWidthBlue           = 0;


//Definición de valores máximos y mínimos de medición de cada filtro de color

/*NOTA --> Terminar de optimizar la calibración para obtener mejor diferenciación entre colores*/
#define    MIN_APORTE_RED      16000  //Aporte de filtro RED cuando hay menor frecuencia (mayor ancho pulso) --> ANALIZANDO COLOR NEGRO
#define    MAX_APORTE_RED      2000    //Aporte de filtro RED cuando hay mayor frecuencia (menor ancho pulso) --> ANALIZANDO COLOR BLANCO
#define    MIN_APORTE_GREEN    14000   //Aporte de filtro GREEN cuando hay menor frecuencia (mayor ancho pulso) --> ANALIZANDO COLOR NEGRO
#define    MAX_APORTE_GREEN    3000   //Aporte de filtro GREEN cuando hay mayor frecuencia (menor ancho pulso) --> ANALIZANDO COLOR BLANCO
#define    MIN_APORTE_BLUE     16000 //Aporte de filtro BLUE cuando hay menor frecuencia (mayor ancho pulso) --> ANALIZANDO COLOR NEGRO
#define    MAX_APORTE_BLUE     3000   //Aporte de filtro BLUE cuando hay mayor frecuencia (menor ancho pulso) --> ANALIZANDO COLOR BLANCO

//Definición de variables para pendientes de escalamiento de medidas en cada filtro del sensor RGB
uint8_t pendienteRed    = 0;
uint8_t pendienteGreen  = 0;
uint8_t pendienteBlue   = 0;

//Definición de variables para cargar el valor medido escalado
uint16_t scaleValueRed   = 0;
uint16_t scaleValueGreen = 0;
uint16_t scaleValueBlue  = 0;

//Definición de variables para cargar el valor resultante del escalamiento y el ajuste del aporte
uint16_t aporteRed   = 0;
uint16_t aporteGreen = 0;
uint16_t aporteBlue  = 0;

//Definición de variable para cargar el aporte DEFINITIVO de los tres colores en cada medida
uint32_t aporteRGB   = 0;

//Definición de variable para pendiente de escalamiento para la frecuencia de entrada al buzzer
uint8_t pendienteFrec = 0;

//Definición de variable para cargar el valor medido escalado
uint16_t scaleValueFrec = 0;

//Definición de variable para asignar la frecuencia de la señal PWM que ingresa al buzzer
uint16_t  frecValue  = 0;

//Definición de variable para asignar el valor del dutty de la señal PWM que ingresa al buzzer
uint16_t duttyValue  = 0;

//Definición de variable para asignar el valor del periodo de la señal PWM que ingresa al buzzer
uint16_t periodValue = 0;

/*Rango de frecuencias a utilizar
 *
 * == Cuarta octava del piano de 262 Hz hasta 494 Hz aprox    ==
 * == Quinta octava del piano de 524 Hz hasta 988 Hz aprox    ==
 * == Sexta octava del piano de 1046 Hz hasta 1976 Hz aprox   ==
 * == Septima octava del piano de 2094 Hz hasta 3952 Hz aprox ==
 * INICIO --> 300  Hz aprox
 * FIN    --> 3952 Hz aprox
 *
 * */
#define  MAX_FREQUENCY  3000
#define  MIN_FREQUENCY  300

//Definición función para configuración inicial
void initialConfig(void);

//Definición función para generar DELAY
void delay(void);

//Definición función para definir filtros de color a utilizar
void sensorConfig(uint8_t filtroColor);

//Definición funciones para contar rising edges de output sensor RGB
void counterOutputSensorConfigR(void);
void counterOutputSensorConfigG(void);
void counterOutputSensorConfigB(void);

//Definición de funciones para counter timer
void counterTimerPulseR(void);
void counterTimerPulseG(void);
void counterTimerPulseB(void);

//Definición de función para comunicación de datos en usart
void msgUsart(void);

//Definición funciones para determinar el periodo de la señal PWM
uint32_t pulseOutputSensorConfigR(void);
uint32_t pulseOutputSensorConfigG(void);
uint32_t pulseOutputSensorConfigB(void);

//Definición funciones para obtener el ancho del pulso del colo RGB en estudio
void getPulseWidthRed(void);
void getPulseWidthGreen(void);
void getPulseWidthBlue(void);

//Definición función para escalamiento de resultados RGB
void getPulseScale(void);

//Definición función para respetar los límites del intervalo de escalamiento
uint16_t scaleLimit(uint16_t scaleValue);

//Definición función para respetar los límites del intervalo de escalamiento
uint16_t scaleLimitFrec(uint16_t scaleValueFrequency);

//Definición de función para escalamiento de resultados RGB a frecuencia
void getFrequency(void);

/*  Main function  */
int main(void)
{
	//Llamamos función para realizar configuración inicial
	initialConfig();

    /* Loop forever */
	while(1){

		//Llamamos a función encargada de obtener el valor del ancho de pulso del colo ROJO
		getPulseWidthRed();

		//Llamamos a función encargada de obtener el valor del ancho de pulso del colo ROJO
		getPulseWidthGreen();

		//Llamamos a función encargada de obtener el valor del ancho de pulso del colo ROJO
		getPulseWidthBlue();

		//Llamamos a la función encargada de representación en USART
		msgUsart();

		//Llamamos función para calcular el aporte de cada color RGB en la medición
		getPulseScale();

		//Llamamos a función para calcular valor de frecuencia a calcular
		getFrequency();

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
		controlTimer.pTIMx                             = TIM4;
		controlTimer.TIMx_Config.TIMx_Prescaler        = 16000;  //Genera incrementos de 0.1 s
		controlTimer.TIMx_Config.TIMx_Period           = 3000;    //Periodo asociado a 3s
		controlTimer.TIMx_Config.TIMx_mode             = TIMER_UP_COUNTER;
		controlTimer.TIMx_Config.TIMx_InterruptEnable  = TIMER_INT_ENABLE;

		/* Configuramos el Timer */
		timer_Config(&controlTimer);

		//Encendemos el Timer
		timer_SetState(&controlTimer, TIMER_ON);

		//Configuración Timer3 --> control del tiempo
		pulseTimer.pTIMx                             = TIM5;
		pulseTimer.TIMx_Config.TIMx_Prescaler        = 16;  //Genera incrementos de 0.1 s
		//pulseTimer.TIMx_Config.TIMx_Period           = 100;    //Periodo asociado a 0.0001s (100 us)
		/*Porqué al usar la siguiente configuración de periodo se obtienen valores negativos??? --> Cuando variables eran de 16 bit*/
		pulseTimer.TIMx_Config.TIMx_Period           = 10;    //Periodo asociado a 0.00001s (10 us)
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

		//A continuación se hace la configuración para los canales PWM a utilizar

		/*Configuración timer para generar señal pwm*/
		signalPWM.ptrTIMx                = TIM3;
		signalPWM.config.channel         = PWM_CHANNEL_1;
		signalPWM.config.duttyCicle      = duttyValue; //Se debe asegurar PWM siempre tendrá un dutty del 50%
		signalPWM.config.prescaler       = 160;
		signalPWM.config.periodo         = periodValue; //Periodo es de periodValue/100000

		pwm_Config(&signalPWM);
		pwm_Enable_Output(&signalPWM);

		/*Configuración PWM*/
		pinPWMChannel.pGPIOx                        = GPIOC;
		pinPWMChannel.pinConfig.GPIO_PinNumber      = PIN_6;
		pinPWMChannel.pinConfig.GPIO_PinMode        = GPIO_MODE_ALTFN;
		pinPWMChannel.pinConfig.GPIO_PinOutputType  = GPIO_OTYPE_PUSHPULL;
		pinPWMChannel.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		pinPWMChannel.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
		pinPWMChannel.pinConfig.GPIO_PinAltFunMode  = AF2;

		gpio_Config(&pinPWMChannel);

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

//Función para contar rising edges de output RED sensor RGB
void counterOutputSensorConfigR(void){

	//Evaluamos si la bandera asociada a la interrupción está activa
	if(banderaOutputSensorExti){

		//Sumamos el valor de un contador que se irá sumado cada vez que la señal PWM tenga un rising edge
		counterOutputSensorR++;

		//Definimos límite de la suma para reiniciar contador
		if(counterOutputSensorR == 3){

			//En caso superarse el límite el contador se reinicia
			counterOutputSensorR = 0;

		}//Fin del condicional

		//Bajamos la bandera
		banderaOutputSensorExti = 0;

	} //Fin del condicional

}//Fin de la función
/**/
//Función para contar rising edges de output GREEN sensor RGB
void counterOutputSensorConfigG(void){

	//Evaluamos si la bandera asociada a la interrupción está activa
	if(banderaOutputSensorExti){

		//Sumamos el valor de un contador que se irá sumado cada vez que la señal PWM tenga un rising edge
		counterOutputSensorG++;

		//Definimos límite de la suma para reiniciar contador
		if(counterOutputSensorG == 3){

			//En caso superarse el límite el contador se reinicia
			counterOutputSensorG = 0;

		}//Fin del condicional

		//Bajamos la bandera
		banderaOutputSensorExti = 0;

	} //Fin del condicional

}//Fin de la función
/**/
//Función para contar rising edges de output BLUE sensor RGB
void counterOutputSensorConfigB(void){

	//Evaluamos si la bandera asociada a la interrupción está activa
	if(banderaOutputSensorExti){

		//Sumamos el valor de un contador que se irá sumado cada vez que la señal PWM tenga un rising edge
		counterOutputSensorB++;

		//Definimos límite de la suma para reiniciar contador
		if(counterOutputSensorB == 3){

			//En caso superarse el límite el contador se reinicia
			counterOutputSensorB = 0;

		}//Fin del condicional

		//Bajamos la bandera
		banderaOutputSensorExti = 0;

	} //Fin del condicional

}//Fin de la función

//Función para counter timer RED
void counterTimerPulseR(void){

	//Verificamos si la bandera del timer está activa
	if(banderaPulseTimer){

		//Empezamos cuenta del tiempo
		/*El valor correspondiente se interpreta planteando que 1 UNIDAD en counter Period equivale a 0.1ms*/
		counterPeriodR++;

		//Bajamos la bandera
		banderaPulseTimer = 0;
	}
}
/**/
//Función para counter timer GREEN
void counterTimerPulseG(void){

	//Verificamos si la bandera del timer está activa
	if(banderaPulseTimer){

		//Empezamos cuenta del tiempo
		/*El valor correspondiente se interpreta planteando que 1 UNIDAD en counter Period equivale a 0.1ms*/
		counterPeriodG++;

		//Bajamos la bandera
		banderaPulseTimer = 0;
	}
}
/**/
//Función para counter timer BLUE
void counterTimerPulseB(void){

	//Verificamos si la bandera del timer está activa
	if(banderaPulseTimer){

		//Empezamos cuenta del tiempo
		/*El valor correspondiente se interpreta planteando que 1 UNIDAD en counter Period equivale a 0.1ms*/
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
//		sprintf(bufferMsg,"Valores de ancho de pulso de cada color: R = %d ms, G = %d ms, B = %d ms \n",pulseWidthRed,pulseWidthGreen,pulseWidthBlue);
//		usart_writeMsg(&usart2, bufferMsg);

		//Escribimos mensaje con los datos de aporte de cada color
		sprintf(bufferMsg,"Aporte PORCENTUAL de cada color RGB en la medida: R = %d , G = %d , B = %d  \n\r",(aporteRed/10),(aporteGreen/10),(aporteBlue/10));
		usart_writeMsg(&usart2, bufferMsg);

		//Bajamos la bandera
		banderaUSARTTx = 0;

	}
}

//Función para determinar el periodo de la señal RED PWM
uint32_t pulseOutputSensorConfigR(void){

	//Evaluamos inicio de contador del periodo cuando se mide el segundo rising edge
	if(counterOutputSensorR == 1){

		//Se inicia función para counter timer para determinar el periodo
		counterTimerPulseR();
	}
	else if(counterOutputSensorR == 2){

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
//Función para determinar el periodo de la señal GREEN PWM
uint32_t pulseOutputSensorConfigG(void){

	//Evaluamos inicio de contador del periodo cuando se mide el segundo rising edge
	if(counterOutputSensorG == 1){

		//Se inicia función para counter timer para determinar el periodo
		counterTimerPulseG();
	}
	else if(counterOutputSensorG == 2){

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
//Función para determinar el periodo de la señal BLUE PWM
uint32_t pulseOutputSensorConfigB(void){

	//Evaluamos inicio de contador del periodo cuando se mide el segundo rising edge
	if(counterOutputSensorB == 1){

		//Se inicia función para counter timer para determinar el periodo
		counterTimerPulseB();
	}
	else if(counterOutputSensorB == 2){

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

//Función para obtener el ancho del pulso del color RED
void getPulseWidthRed(void){

	//Configuramos filtro rojo para análisis del sensor RGB
	sensorConfig(FILTRO_RED);

	//Inicializamos variable para entrar al ciclo while
	counterMeasureR = 1;

	while(counterMeasureR != 0){

		//Inicializamos función para empezar a contar los rising edges de la señal en estudio
		counterOutputSensorConfigR();

		//Llamamos función para establecer el periodo de la señal PWM del color en análisis
		//Realizamos conversión para encontrar el valor del ancho del pulso (en ms)
		/* RECORDAR--> Duty de la señal es siempre del 50% */
		pulseWidthRed = (pulseOutputSensorConfigR())/2;

		//Definimos máximo del ciclo while en valor que permite contar y medir el ancho de pulso
		if(counterOutputSensorR == 2){

			//Reiniciamos variable para salir del ciclo
			counterMeasureR = 0;
		}
	}

}
/**/
//Función para obtener el ancho del pulso del color GREEN
void getPulseWidthGreen(void){

//	//Configuramos filtro rojo para análisis del sensor RGB
	sensorConfig(FILTRO_GREEN);

	//Inicializamos variable para entrar al ciclo while
	counterMeasureG = 1;

	while(counterMeasureG != 0){

		//Inicializamos función para empezar a contar los rising edges de la señal en estudio
		counterOutputSensorConfigG();

		//Llamamos función para establecer el periodo de la señal PWM del color en análisis
		//Realizamos conversión para encontrar el valor del ancho del pulso (en ms)
		/* RECORDAR--> Duty de la señal es siempre del 50% */
		pulseWidthGreen = (pulseOutputSensorConfigG())/2;

		//Definimos máximo del ciclo while en valor que permite contar y medir el ancho de pulso
		if(counterOutputSensorG == 2){

			//Reiniciamos variable para salir del ciclo
			counterMeasureG = 0;
		}
	}

}
/**/
//Función para obtener el ancho del pulso del color BLUE
void getPulseWidthBlue(void){

//	//Configuramos filtro rojo para análisis del sensor RGB
	sensorConfig(FILTRO_BLUE);

	//Inicializamos variable para entrar al ciclo while
	counterMeasureB = 1;

	while(counterMeasureB != 0){

		//Inicializamos función para empezar a contar los rising edges de la señal en estudio
		counterOutputSensorConfigB();

		//Llamamos función para establecer el periodo de la señal PWM del color en análisis
		//Realizamos conversión para encontrar el valor del ancho del pulso (en ms)
		/* RECORDAR--> Duty de la señal es siempre del 50% */
		pulseWidthBlue = (pulseOutputSensorConfigB())/2;

		//Definimos máximo del ciclo while en valor que permite contar y medir el ancho de pulso
		if(counterOutputSensorB == 2){

			//Reiniciamos variable para salir del ciclo
			counterMeasureB = 0;
		}
	}

}

//Función para respetar los límites del intervalo de escalamiento
uint16_t scaleLimit(uint16_t scaleValue){

	uint16_t scaleAdjust = 0;

	//Se evalua si el valor cumple las condiciones del intervalo
	if(scaleValue > 1000){

		//Se establece el valor justo en el máximo en caso de el original sobrepasarlo
		scaleAdjust = 1000;
	}
	else if(scaleValue < 1){

		//Se establece el valor justo en el mínimo en caso de el original sobrepasarlo
		scaleAdjust = 0;
	}
	else{
		scaleAdjust = scaleValue;
	}

	return scaleAdjust;

}

//Función para escalar el ancho de pulso de cada señal
void getPulseScale(void){

	/* Para poder lograr una mejor interpretación de los datos de aporte de cada uno de los colores
	 * respecto a la medida definitiva del sensor se propone un escalamiento en el rango de 0 a 1000 (LINEA RECTA)
	 * para cada uno de los colores -> Para poder cumplir con este objetivo se analizó cuales fueron
	 * los valores máximos y mínimos medidos por cada filtro del sensor (analizando el color negro y blanco)
	 * */
	//ESCALA: 1000 -> Max contribución del color en el análisis === 0 -> Min contribución del color en el análisis

	/*1. Determinamos el valor de la pendiente de escalamiento para cada uno de los filtros*/
	//NOTA: Dado que NO queremos trabajar con datos de punto flotante se aproxima a considerar hasta el orden del tercer decimal
	//para lograrlo se multiplica por 1000 el valor real de cada pendiente

	//Calculamos pendiente escalamiento filtro RED
	pendienteRed = ((1000000)/(MIN_APORTE_RED-MAX_APORTE_RED));

	//Calculamos pendiente escalamiento filtro GREEN
	pendienteGreen = ((1000000)/(MIN_APORTE_GREEN-MAX_APORTE_GREEN));

	//Calculamos pendiente escalamiento filtro BLUE
	pendienteBlue = ((1000000)/(MIN_APORTE_BLUE-MAX_APORTE_BLUE));

	/*2. Aplicamos multiplicación de la pendiente por el respectivo valor medido.. para conocer el valor escalado resultante*/
	//NOTA: Se divide el resultado entre 1000 para volver a valor real del escalamiento

	//Calulamos el valor escalado RED
	scaleValueRed = (pendienteRed*pulseWidthRed)/1000;

	//Calulamos el valor escalado GREEN
	scaleValueGreen = (pendienteGreen*pulseWidthGreen)/1000;

	//Calulamos el valor escalado BLUE
	scaleValueBlue = (pendienteBlue*pulseWidthBlue)/1000;

	/*2.1 Es necesario NO sobrepasar los límites del intervalo de escalamiento, por lo tanto se ejecuta una función
	 * para garantizar el cumplimiento de esta condición en los tres filtros de color R,G y B  --> POR ESTO SE LLAMA A scaleLimit()
	 * en el siguiente item*/

	/*3. Reajustamos resultado para garantizar que entre mayor el valor escalado, mayor el aporte del color respectivo en la medición*/

	//Reajustando aporte respectivo escalado de RED
	aporteRed = 1000 - scaleLimit(scaleValueRed);

	//Reajustando aporte respectivo escalado de GREEN
	aporteGreen = 1000 - scaleLimit(scaleValueGreen);

	//Reajustando aporte respectivo escalado de BLUE
	aporteBlue = 1000 - scaleLimit(scaleValueBlue);

	//Calculamos el valor del aporte TOTAL de los tres filtros de color
	/* Esta valor va a ser de utilidad para asignar el respectivo valor de frecuencia para reproducir.
	 * Cada medida, considerando el aporte RGB, va a tener un valor dentro del rango [0-111000] (*Este valor
	 * pensando en lograr asignar a diferentes colores diferentes tonos, sin importar que el porcentaje de
	 * aporte RGB total pueda ser el mismo entre dos colores distintos, importará también el aporte específico
	 * de cada filtro de color para determinar la tonalidad específica*), el porcentaje asociado a este
	 * valor/111000 va a escalar el valor de frecuencia a reproducir*/

	aporteRGB = (aporteBlue*100) + (aporteGreen*10) + (aporteRed);
}

//Función para respetar los límites del intervalo de escalamiento
uint16_t scaleLimitFrec(uint16_t scaleValueFrequency){

	uint16_t scaleAdjustFrec = 0;

	//Se evalua si el valor cumple las condiciones del intervalo
	if(scaleValueFrequency > 3000){

		//Se establece el valor justo en el máximo en caso de el original sobrepasarlo
		scaleAdjustFrec = 3000;
	}
	else if(scaleValueFrequency < 300){

		//Se establece el valor justo en el mínimo en caso de el original sobrepasarlo
		scaleAdjustFrec = 300;
	}
	else{
		scaleAdjustFrec = scaleValueFrequency;
	}

	return scaleAdjustFrec;

}

//Función para escalamiento de resultados RGB a frecuencia
void getFrequency(void){

	/*Para poder asignar un resultado de frecuencia relacionado con los valores de aporte de cada uno de los filtros RGB se propone
	 * un escalamiento (LINEA RECTA) que irá dentro del rango aprox [300 Hz,3950 Hz] que corresponde a gran parte de las notas presentes en los
	 * espacios de la cuarta a séptima octava de un piano.*/

	/*1. Determinamos el valor de la pendiente de escalamiento*/
	//NOTA: Dado que NO queremos trabajar con datos de punto flotante se aproxima a considerar hasta el orden del tercer decimal
	//para lograrlo se multiplica por 1000 el valor real de cada pendiente

	pendienteFrec = (1000*(MAX_FREQUENCY-MIN_FREQUENCY))/(111000);

	/*2. Aplicamos multiplicación de la pendiente por el respectivo valor medido.. para conocer el valor escalado resultante*/
	//NOTA: Se divide el resultado entre 1000 para volver a valor real del escalamiento

	scaleValueFrec = (pendienteFrec*aporteRGB)/1000;

	/*2.1 Es necesario NO sobrepasar los límites del intervalo de escalamiento, por lo tanto se ejecuta una función
	 * para garantizar el cumplimiento de esta condición */

	frecValue = scaleLimitFrec(scaleValueFrec);

	/*3. Determinamos los valores del Dutty y del periodo que deben ir en la configuración del PWM*/

	//Determinamos el valor del "periodo" que se asigna en la configuración teniendo en cuenta velocidad micro 16MHz y prescaler en 1600Hz
	periodValue = (100000/frecValue);

	//Evaluamos si el numero es par o impar para poder mejorar el cálculo del Dutty correctamente
	if((periodValue % 2) != 0){

		periodValue = periodValue + 1;
	}
	else{

		periodValue = periodValue;
	}

	//Determinamos el valor de dutty en el 50% constante para todos los casos de frecuencia
	duttyValue = (periodValue/2) ;

	//Se actualiza el valor del periodo y el dutty en configuración del PWM
	pwm_Update_Frequency(&signalPWM, periodValue);
	pwm_Update_DuttyCycle(&signalPWM, duttyValue);

	//Inicializando la señal PWM
	pwm_Start_Signal(&signalPWM);

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
void Timer5_Callback(void){

	//Activamos bandera correspondiente a interrupción para pulse timer
	banderaPulseTimer = 1;
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
