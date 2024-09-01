/*
 * adc_driver_hal.c
 *
 *  Created on: 1/09/2024
 *      Author: laurasofia
 */

#include "adc_driver_hal.h"
#include "gpio_driver_hal.h"
#include "stm32f4xx.h"
#include "stm32_assert.h"

/* === Headers for private functions === */
static void adc_enable_clock_peripheral(void);
static void adc_set_resolution(ADC_Config_t *adcConfig);
static void adc_set_alignment(ADC_Config_t *adcConfig);
static void adc_set_sampling_and_hold(ADC_Config_t *adcConfig);
static void adc_set_one_channel_sequence(ADC_Config_t *adcConfig);
static void adc_config_interrupt(ADC_Config_t *adcConfig);

/* Variables y elementos que necesita internamente el driver para funcionar adecuadamente */
GPIO_Handler_t handlerADCPin   = {0};
uint16_t       adcRawData      = 0;

/*
 *
 * */

void adc_ConfigSingleChannel(ADC_Config_t *adcConfig){

	/* 1. Configuramos el PinX paea que se cumpla la función del canal análogo deseado*/
	adc_ConfigAnalogPin(adcConfig->channel);

	/* 2. Activamos la señal de reloj para el ADC */
	adc_enable_clock_peripheral();

	//Limpiamos los registros antes de comenzar a configurar
	ADC1->CR1 = 0;
	ADC1->CR2 = 0;

	/* Comenzamos la configuración de ADC1 */

	/* 3. Resolución del ADC */
	adc_set_resolution(adcConfig);

	/* 4. Configuramos el modo Scan como desactivado */
	adc_ScanMode(SCAN_OFF);

	/* 5. Configuramos la alineación de los datos (derecha o izquierda)*/
	adc_set_alignment(adcConfig);

	/* 6. Desactivamos el continuous mode */
	adc_StopContinuousConv();

	/* 7. Se configura el sampling */
	adc_set_sampling_and_hold(adcConfig);

	/* 8. Configuramos la secuencia y cuantos elementos hay en las secuencias */
	adc_set_one_channel_sequence(adcConfig);

	/* 9. Configuramos el prescaler del ADC en 2:1 (el mas rápido que se puede tener) */
	ADC->CCR &= ~ADC_CCR_ADCPRE;

	/* 10. Desactivamos las interrupciones globales */
	__disable_irq();

	/* 11. Configuramos la interrupción (si se encuentra activa), ademas de inscribir/remover
	 * la interrupción en el NVIC */
	adc_config_interrupt(adcConfig);

	/* 12. Activamos el modulo ADC */
	adc_peripheralOnOFF(ADC_ON);

	/* 13. Activamos las interrupciones globales */
	__enable_irq();

}

/*
 * Enable Clock signal for ADC peripheral
 * */
static void adc_enable_clock_peripheral(void){

}


/*
 * Configures the resolution for the conversion
 * */
static void adc_set_resolution(ADC_Config_t *adcConfig){

}


/*
 * Set the nu,ber alignment_ left or right
 * */
static void adc_set_alignment(ADC_Config_t *adcConfig){

}


/*
 * Relacionando con el valor del tiempo de carga del capacitor HOLD
 * */
static void adc_set_sampling_and_hold(ADC_Config_t *adcConfig){

}


/*
 * Configura el numero de elementos en la secuencia (solo un elemento)
 * Configura cual es el canal que adquiere la señal ADC
 * */
static void adc_set_one_channel_sequence(ADC_Config_t *adcConfig){

}


/*
 * Configura el enable de la interrupción y la activación del NVIC
 * */
static void adc_config_interrupt(ADC_Config_t *adcConfig){

}


/*
 * Controla la ctivación y desactivación del modulo ADC desde el registro
 * CR2 del ADC
 * */
void adc_peripheralOnOFF(uint8_t state){

}


/*
 *Enables and disables the ScanMode
 *Funciona de la mano con la secuencia de carios canales
 *NO es necesario para el caso de solo un canal simple
 * */
void adc_ScanMode(uint8_t state){

}


/*
 *Función que comienza la conversión ADC simple
 * */
void adc_StartSingleConv(void){

}


/*
 * Funcion que comienza la conversión ADC continua
 * */
void adc_StartContinuousConv(void){

}


/*
 * Funcion que termina la conversión ADC continua
 * */
void adc_StopContinuousConv(void){

}


/*
 * Funcion que retorna el último dato adquirido por la ADC
 * */
uint16_t adc_Get_Value(void){
	return adcRawData;
}

/*
 * Esta es la ISR de la interrupción por conversión ADC
 * */
void ADC_IRQHandler(void){

}


__attribute__ ((weak)) void adc_CompleteCallback(void){
	__NOP();
}


/*Configuración para hacer conversiones en multiples canales y en un orden específico*/

/* Configuración para trigger externo */
