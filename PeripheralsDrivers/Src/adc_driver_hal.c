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

	//Activamos la señal de reloj
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
}


/*
 * Configures the resolution for the conversion
 * */
static void adc_set_resolution(ADC_Config_t *adcConfig){

	switch(adcConfig->resolution){

	case RESOLUTION_12_BIT:{

		// Debemos cargar el valor 0b00 en los 12 bit de resolución
		ADC1->CR1 &= ~ADC_CR1_RES_0;
		ADC1->CR1 &= ~ADC_CR1_RES_1;
		break;

	}
	case RESOLUTION_10_BIT:{

		// Debemos cargar el valor 0b01 en los 10 bit de resolución
		ADC1->CR1 |= ADC_CR1_RES_0;
		ADC1->CR1 &= ~ADC_CR1_RES_1;
		break;

	}
	case RESOLUTION_8_BIT:{

		// Debemos cargar el valor 0b10 en los 8 bit de resolución
		ADC1->CR1 &= ~ADC_CR1_RES_0;
		ADC1->CR1 |= ADC_CR1_RES_1;
		break;

	}
	case RESOLUTION_6_BIT:{

		// Debemos cargar el valor 0b11 en los 6 bit de resolución
		ADC1->CR1 |= ADC_CR1_RES_0;
		ADC1->CR1 |= ADC_CR1_RES_1;
		break;

	}
	default: {

		// En caso por defecto ebemos cargar el valor 0b00 en los 12 bit de resolución
		ADC1->CR1 &= ~ADC_CR1_RES_0;
		ADC1->CR1 &= ~ADC_CR1_RES_1;
		break;
	}
	}
}


/*
 * Set the number alignment_ left or right
 * */
static void adc_set_alignment(ADC_Config_t *adcConfig){

	if(adcConfig->dataAlignment == ALIGNMENT_RIGHT){
		ADC1->CR2 &= ~ADC_CR2_ALIGN;
	} else if(adcConfig->dataAlignment == ALIGNMENT_LEFT){
		ADC1->CR2 |= ADC_CR2_ALIGN;
	} else{
		ADC1->CR2 &= ~ADC_CR2_ALIGN;
	}
}


/*
 * Relacionando con el valor del tiempo de carga del capacitor HOLD
 * */
static void adc_set_sampling_and_hold(ADC_Config_t *adcConfig){

	switch(adcConfig->samplingPeriod){

	case SAMPLING_PERIOD_3_CYCLES: {
		if(adcConfig->channel == CHANNEL_0){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP0_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP0_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP0_2;

		}else if(adcConfig->channel == CHANNEL_1){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP1_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP1_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP1_2;

		}else if(adcConfig->channel == CHANNEL_2){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP2_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP2_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP2_2;

		}else if(adcConfig->channel == CHANNEL_3){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP3_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP3_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP3_2;

		}else if(adcConfig->channel == CHANNEL_4){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP4_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP4_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP4_2;

		}else if(adcConfig->channel == CHANNEL_5){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP5_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP5_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP5_2;

		}else if(adcConfig->channel == CHANNEL_6){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP6_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP6_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP6_2;

		}else if(adcConfig->channel == CHANNEL_7){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP7_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP7_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP7_2;

		}else if(adcConfig->channel == CHANNEL_8){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP8_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP8_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP8_2;

		}else if(adcConfig->channel == CHANNEL_9){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP9_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP9_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP9_2;

		}else if(adcConfig->channel == CHANNEL_10){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP10_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP10_1;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP10_2;

		}else if(adcConfig->channel == CHANNEL_11){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP11_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP11_1;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP11_2;

		}else if(adcConfig->channel == CHANNEL_12){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP12_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP12_1;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP12_2;

		}else if(adcConfig->channel == CHANNEL_13){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP13_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP13_1;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP13_2;

		}else if(adcConfig->channel == CHANNEL_14){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP14_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP14_1;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP14_2;

		}else if(adcConfig->channel == CHANNEL_15){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP15_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP15_1;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP15_2;
		}
		break;
	}
	case SAMPLING_PERIOD_15_CYCLES: {
		if(adcConfig->channel == CHANNEL_0){
			ADC1->SMPR2 |= ADC_SMPR2_SMP0_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP0_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP0_2;

		}else if(adcConfig->channel == CHANNEL_1){

			ADC1->SMPR2 |= ADC_SMPR2_SMP1_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP1_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP1_2;

		}else if(adcConfig->channel == CHANNEL_2){
			ADC1->SMPR2 |= ADC_SMPR2_SMP2_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP2_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP2_2;

		}else if(adcConfig->channel == CHANNEL_3){
			ADC1->SMPR2 |= ADC_SMPR2_SMP3_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP3_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP3_2;

		}else if(adcConfig->channel == CHANNEL_4){
			ADC1->SMPR2 |= ADC_SMPR2_SMP4_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP4_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP4_2;

		}else if(adcConfig->channel == CHANNEL_5){
			ADC1->SMPR2 |= ADC_SMPR2_SMP5_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP5_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP5_2;

		}else if(adcConfig->channel == CHANNEL_6){
			ADC1->SMPR2 |= ADC_SMPR2_SMP6_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP6_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP6_2;

		}else if(adcConfig->channel == CHANNEL_7){
			ADC1->SMPR2 |= ADC_SMPR2_SMP7_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP7_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP7_2;

		}else if(adcConfig->channel == CHANNEL_8){
			ADC1->SMPR2 |= ADC_SMPR2_SMP8_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP8_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP8_2;

		}else if(adcConfig->channel == CHANNEL_9){
			ADC1->SMPR2 |= ADC_SMPR2_SMP9_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP9_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP9_2;

		}else if(adcConfig->channel == CHANNEL_10){
			ADC1->SMPR1 |= ADC_SMPR1_SMP10_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP10_1;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP10_2;

		}else if(adcConfig->channel == CHANNEL_11){
			ADC1->SMPR1 |= ADC_SMPR1_SMP11_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP11_1;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP11_2;

		}else if(adcConfig->channel == CHANNEL_12){
			ADC1->SMPR1 |= ADC_SMPR1_SMP12_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP12_1;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP12_2;

		}else if(adcConfig->channel == CHANNEL_13){
			ADC1->SMPR1 |= ADC_SMPR1_SMP13_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP13_1;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP13_2;

		}else if(adcConfig->channel == CHANNEL_14){
			ADC1->SMPR1 |= ADC_SMPR1_SMP14_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP14_1;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP14_2;

		}else if(adcConfig->channel == CHANNEL_15){
			ADC1->SMPR1 |= ADC_SMPR1_SMP15_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP15_1;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP15_2;
		}
		break;
	}
	case SAMPLING_PERIOD_28_CYCLES: {
		if(adcConfig->channel == CHANNEL_0){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP0_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP0_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP0_2;

		}else if(adcConfig->channel == CHANNEL_1){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP1_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP1_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP1_2;

		}else if(adcConfig->channel == CHANNEL_2){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP2_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP2_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP2_2;

		}else if(adcConfig->channel == CHANNEL_3){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP3_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP3_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP3_2;

		}else if(adcConfig->channel == CHANNEL_4){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP4_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP4_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP4_2;

		}else if(adcConfig->channel == CHANNEL_5){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP5_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP5_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP5_2;

		}else if(adcConfig->channel == CHANNEL_6){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP6_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP6_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP6_2;

		}else if(adcConfig->channel == CHANNEL_7){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP7_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP7_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP7_2;

		}else if(adcConfig->channel == CHANNEL_8){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP8_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP8_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP8_2;

		}else if(adcConfig->channel == CHANNEL_9){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP9_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP9_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP9_2;

		}else if(adcConfig->channel == CHANNEL_10){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP10_0;
			ADC1->SMPR1 |= ADC_SMPR1_SMP10_1;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP10_2;

		}else if(adcConfig->channel == CHANNEL_11){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP11_0;
			ADC1->SMPR1 |= ADC_SMPR1_SMP11_1;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP11_2;

		}else if(adcConfig->channel == CHANNEL_12){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP12_0;
			ADC1->SMPR1 |= ADC_SMPR1_SMP12_1;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP12_2;

		}else if(adcConfig->channel == CHANNEL_13){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP13_0;
			ADC1->SMPR1 |= ADC_SMPR1_SMP13_1;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP13_2;

		}else if(adcConfig->channel == CHANNEL_14){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP14_0;
			ADC1->SMPR1 |= ADC_SMPR1_SMP14_1;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP14_2;

		}else if(adcConfig->channel == CHANNEL_15){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP15_0;
			ADC1->SMPR1 |= ADC_SMPR1_SMP15_1;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP15_2;
		}
		break;
	}
	case SAMPLING_PERIOD_56_CYCLES: {
		if(adcConfig->channel == CHANNEL_0){
			ADC1->SMPR2 |= ADC_SMPR2_SMP0_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP0_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP0_2;

		}else if(adcConfig->channel == CHANNEL_1){
			ADC1->SMPR2 |= ADC_SMPR2_SMP1_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP1_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP1_2;

		}else if(adcConfig->channel == CHANNEL_2){
			ADC1->SMPR2 |= ADC_SMPR2_SMP2_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP2_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP2_2;

		}else if(adcConfig->channel == CHANNEL_3){
			ADC1->SMPR2 |= ADC_SMPR2_SMP3_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP3_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP3_2;

		}else if(adcConfig->channel == CHANNEL_4){
			ADC1->SMPR2 |= ADC_SMPR2_SMP4_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP4_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP4_2;

		}else if(adcConfig->channel == CHANNEL_5){
			ADC1->SMPR2 |= ADC_SMPR2_SMP5_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP5_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP5_2;

		}else if(adcConfig->channel == CHANNEL_6){
			ADC1->SMPR2 |= ADC_SMPR2_SMP6_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP6_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP6_2;

		}else if(adcConfig->channel == CHANNEL_7){
			ADC1->SMPR2 |= ADC_SMPR2_SMP7_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP7_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP7_2;

		}else if(adcConfig->channel == CHANNEL_8){
			ADC1->SMPR2 |= ADC_SMPR2_SMP8_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP8_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP8_2;

		}else if(adcConfig->channel == CHANNEL_9){
			ADC1->SMPR2 |= ADC_SMPR2_SMP9_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP9_1;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP9_2;

		}else if(adcConfig->channel == CHANNEL_10){
			ADC1->SMPR1 |= ADC_SMPR1_SMP10_0;
			ADC1->SMPR1 |= ADC_SMPR1_SMP10_1;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP10_2;

		}else if(adcConfig->channel == CHANNEL_11){
			ADC1->SMPR1 |= ADC_SMPR1_SMP11_0;
			ADC1->SMPR1 |= ADC_SMPR1_SMP11_1;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP11_2;

		}else if(adcConfig->channel == CHANNEL_12){
			ADC1->SMPR1 |= ADC_SMPR1_SMP12_0;
			ADC1->SMPR1 |= ADC_SMPR1_SMP12_1;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP12_2;

		}else if(adcConfig->channel == CHANNEL_13){
			ADC1->SMPR1 |= ADC_SMPR1_SMP13_0;
			ADC1->SMPR1 |= ADC_SMPR1_SMP13_1;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP13_2;

		}else if(adcConfig->channel == CHANNEL_14){
			ADC1->SMPR1 |= ADC_SMPR1_SMP14_0;
			ADC1->SMPR1 |= ADC_SMPR1_SMP14_1;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP14_2;

		}else if(adcConfig->channel == CHANNEL_15){
			ADC1->SMPR1 |= ADC_SMPR1_SMP15_0;
			ADC1->SMPR1 |= ADC_SMPR1_SMP15_1;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP15_2;
		}
		break;
	}
	case SAMPLING_PERIOD_84_CYCLES: {
		if(adcConfig->channel == CHANNEL_0){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP0_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP0_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP0_2;

		}else if(adcConfig->channel == CHANNEL_1){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP1_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP1_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP1_2;

		}else if(adcConfig->channel == CHANNEL_2){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP2_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP2_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP2_2;

		}else if(adcConfig->channel == CHANNEL_3){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP3_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP3_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP3_2;

		}else if(adcConfig->channel == CHANNEL_4){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP4_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP4_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP4_2;

		}else if(adcConfig->channel == CHANNEL_5){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP5_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP5_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP5_2;

		}else if(adcConfig->channel == CHANNEL_6){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP6_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP6_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP6_2;

		}else if(adcConfig->channel == CHANNEL_7){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP7_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP7_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP7_2;

		}else if(adcConfig->channel == CHANNEL_8){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP8_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP8_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP8_2;

		}else if(adcConfig->channel == CHANNEL_9){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP9_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP9_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP9_2;

		}else if(adcConfig->channel == CHANNEL_10){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP10_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP10_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP10_2;

		}else if(adcConfig->channel == CHANNEL_11){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP11_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP11_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP11_2;

		}else if(adcConfig->channel == CHANNEL_12){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP12_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP12_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP12_2;

		}else if(adcConfig->channel == CHANNEL_13){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP13_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP13_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP13_2;

		}else if(adcConfig->channel == CHANNEL_14){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP14_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP14_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP14_2;

		}else if(adcConfig->channel == CHANNEL_15){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP15_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP15_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP15_2;
		}
		break;
	}
	case SAMPLING_PERIOD_112_CYCLES: {
		if(adcConfig->channel == CHANNEL_0){
			ADC1->SMPR2 |= ADC_SMPR2_SMP0_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP0_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP0_2;

		}else if(adcConfig->channel == CHANNEL_1){
			ADC1->SMPR2 |= ADC_SMPR2_SMP1_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP1_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP1_2;

		}else if(adcConfig->channel == CHANNEL_2){
			ADC1->SMPR2 |= ADC_SMPR2_SMP2_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP2_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP2_2;

		}else if(adcConfig->channel == CHANNEL_3){
			ADC1->SMPR2 |= ADC_SMPR2_SMP3_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP3_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP3_2;

		}else if(adcConfig->channel == CHANNEL_4){
			ADC1->SMPR2 |= ADC_SMPR2_SMP4_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP4_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP4_2;

		}else if(adcConfig->channel == CHANNEL_5){
			ADC1->SMPR2 |= ADC_SMPR2_SMP5_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP5_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP5_2;

		}else if(adcConfig->channel == CHANNEL_6){
			ADC1->SMPR2 |= ADC_SMPR2_SMP6_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP6_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP6_2;

		}else if(adcConfig->channel == CHANNEL_7){
			ADC1->SMPR2 |= ADC_SMPR2_SMP7_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP7_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP7_2;

		}else if(adcConfig->channel == CHANNEL_8){
			ADC1->SMPR2 |= ADC_SMPR2_SMP8_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP8_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP8_2;

		}else if(adcConfig->channel == CHANNEL_9){
			ADC1->SMPR2 |= ADC_SMPR2_SMP9_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP9_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP9_2;

		}else if(adcConfig->channel == CHANNEL_10){
			ADC1->SMPR1 |= ADC_SMPR1_SMP10_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP10_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP10_2;

		}else if(adcConfig->channel == CHANNEL_11){
			ADC1->SMPR1 |= ADC_SMPR1_SMP11_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP11_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP11_2;

		}else if(adcConfig->channel == CHANNEL_12){
			ADC1->SMPR1 |= ADC_SMPR1_SMP12_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP12_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP12_2;

		}else if(adcConfig->channel == CHANNEL_13){
			ADC1->SMPR1 |= ADC_SMPR1_SMP13_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP13_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP13_2;

		}else if(adcConfig->channel == CHANNEL_14){
			ADC1->SMPR1 |= ADC_SMPR1_SMP14_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP14_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP14_2;

		}else if(adcConfig->channel == CHANNEL_15){
			ADC1->SMPR1 |= ADC_SMPR1_SMP15_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP15_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP15_2;
		}
		break;
	}
	case SAMPLING_PERIOD_144_CYCLES: {
		if(adcConfig->channel == CHANNEL_0){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP0_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP0_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP0_2;

		}else if(adcConfig->channel == CHANNEL_1){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP1_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP1_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP1_2;

		}else if(adcConfig->channel == CHANNEL_2){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP2_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP2_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP2_2;

		}else if(adcConfig->channel == CHANNEL_3){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP3_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP3_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP3_2;

		}else if(adcConfig->channel == CHANNEL_4){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP4_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP4_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP4_2;

		}else if(adcConfig->channel == CHANNEL_5){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP5_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP5_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP5_2;

		}else if(adcConfig->channel == CHANNEL_6){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP6_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP6_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP6_2;

		}else if(adcConfig->channel == CHANNEL_7){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP7_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP7_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP7_2;

		}else if(adcConfig->channel == CHANNEL_8){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP8_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP8_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP8_2;

		}else if(adcConfig->channel == CHANNEL_9){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP9_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP9_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP9_2;

		}else if(adcConfig->channel == CHANNEL_10){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP10_0;
			ADC1->SMPR1 |= ADC_SMPR1_SMP10_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP10_2;

		}else if(adcConfig->channel == CHANNEL_11){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP11_0;
			ADC1->SMPR1 |= ADC_SMPR1_SMP11_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP11_2;

		}else if(adcConfig->channel == CHANNEL_12){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP12_0;
			ADC1->SMPR1 |= ADC_SMPR1_SMP12_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP12_2;

		}else if(adcConfig->channel == CHANNEL_13){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP13_0;
			ADC1->SMPR1 |= ADC_SMPR1_SMP13_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP13_2;

		}else if(adcConfig->channel == CHANNEL_14){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP14_0;
			ADC1->SMPR1 |= ADC_SMPR1_SMP14_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP14_2;

		}else if(adcConfig->channel == CHANNEL_15){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP15_0;
			ADC1->SMPR1 |= ADC_SMPR1_SMP15_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP15_2;
		}
		break;
	}
	case SAMPLING_PERIOD_480_CYCLES: {
		if(adcConfig->channel == CHANNEL_0){
			ADC1->SMPR2 |= ADC_SMPR2_SMP0_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP0_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP0_2;

		}else if(adcConfig->channel == CHANNEL_1){
			ADC1->SMPR2 |= ADC_SMPR2_SMP1_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP1_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP1_2;

		}else if(adcConfig->channel == CHANNEL_2){
			ADC1->SMPR2 |= ADC_SMPR2_SMP2_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP2_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP2_2;

		}else if(adcConfig->channel == CHANNEL_3){
			ADC1->SMPR2 |= ADC_SMPR2_SMP3_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP3_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP3_2;

		}else if(adcConfig->channel == CHANNEL_4){
			ADC1->SMPR2 |= ADC_SMPR2_SMP4_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP4_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP4_2;

		}else if(adcConfig->channel == CHANNEL_5){
			ADC1->SMPR2 |= ADC_SMPR2_SMP5_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP5_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP5_2;

		}else if(adcConfig->channel == CHANNEL_6){
			ADC1->SMPR2 |= ADC_SMPR2_SMP6_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP6_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP6_2;

		}else if(adcConfig->channel == CHANNEL_7){
			ADC1->SMPR2 |= ADC_SMPR2_SMP7_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP7_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP7_2;

		}else if(adcConfig->channel == CHANNEL_8){
			ADC1->SMPR2 |= ADC_SMPR2_SMP8_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP8_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP8_2;

		}else if(adcConfig->channel == CHANNEL_9){
			ADC1->SMPR2 |= ADC_SMPR2_SMP9_0;
			ADC1->SMPR2 |= ADC_SMPR2_SMP9_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP9_2;

		}else if(adcConfig->channel == CHANNEL_10){
			ADC1->SMPR1 |= ADC_SMPR1_SMP10_0;
			ADC1->SMPR1 |= ADC_SMPR1_SMP10_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP10_2;

		}else if(adcConfig->channel == CHANNEL_11){
			ADC1->SMPR1 |= ADC_SMPR1_SMP11_0;
			ADC1->SMPR1 |= ADC_SMPR1_SMP11_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP11_2;

		}else if(adcConfig->channel == CHANNEL_12){
			ADC1->SMPR1 |= ADC_SMPR1_SMP12_0;
			ADC1->SMPR1 |= ADC_SMPR1_SMP12_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP12_2;

		}else if(adcConfig->channel == CHANNEL_13){
			ADC1->SMPR1 |= ADC_SMPR1_SMP13_0;
			ADC1->SMPR1 |= ADC_SMPR1_SMP13_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP13_2;

		}else if(adcConfig->channel == CHANNEL_14){
			ADC1->SMPR1 |= ADC_SMPR1_SMP14_0;
			ADC1->SMPR1 |= ADC_SMPR1_SMP14_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP14_2;

		}else if(adcConfig->channel == CHANNEL_15){
			ADC1->SMPR1 |= ADC_SMPR1_SMP15_0;
			ADC1->SMPR1 |= ADC_SMPR1_SMP15_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP15_2;
		}
		break;
	}
	default:{
		if(adcConfig->channel == CHANNEL_0){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP0_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP0_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP0_2;

		}else if(adcConfig->channel == CHANNEL_1){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP1_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP1_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP1_2;

		}else if(adcConfig->channel == CHANNEL_2){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP2_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP2_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP2_2;

		}else if(adcConfig->channel == CHANNEL_3){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP3_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP3_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP3_2;

		}else if(adcConfig->channel == CHANNEL_4){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP4_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP4_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP4_2;

		}else if(adcConfig->channel == CHANNEL_5){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP5_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP5_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP5_2;

		}else if(adcConfig->channel == CHANNEL_6){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP6_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP6_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP6_2;

		}else if(adcConfig->channel == CHANNEL_7){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP7_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP7_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP7_2;

		}else if(adcConfig->channel == CHANNEL_8){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP8_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP8_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP8_2;

		}else if(adcConfig->channel == CHANNEL_9){
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP9_0;
			ADC1->SMPR2 &= ~ADC_SMPR2_SMP9_1;
			ADC1->SMPR2 |= ADC_SMPR2_SMP9_2;

		}else if(adcConfig->channel == CHANNEL_10){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP10_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP10_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP10_2;

		}else if(adcConfig->channel == CHANNEL_11){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP11_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP11_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP11_2;

		}else if(adcConfig->channel == CHANNEL_12){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP12_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP12_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP12_2;

		}else if(adcConfig->channel == CHANNEL_13){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP13_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP13_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP13_2;

		}else if(adcConfig->channel == CHANNEL_14){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP14_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP14_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP14_2;

		}else if(adcConfig->channel == CHANNEL_15){
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP15_0;
			ADC1->SMPR1 &= ~ADC_SMPR1_SMP15_1;
			ADC1->SMPR1 |= ADC_SMPR1_SMP15_2;
		}
		break;
	}
	}
}


/*
 * Configura el numero de elementos en la secuencia (solo un elemento)
 * Configura cual es el canal que adquiere la señal ADC
 * */
static void adc_set_one_channel_sequence(ADC_Config_t *adcConfig){

	ADC1->SQR1 &= ~ADC_SQR1_L;
	switch(adcConfig->channel){

	case CHANNEL_0:{
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_0;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_1;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_2;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_3;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_4;
		break;
	}
	case CHANNEL_1:{
		ADC1->SQR3 |= ADC_SQR1_SQ13_0;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_1;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_2;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_3;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_4;
		break;
	}
	case CHANNEL_2:{
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_0;
		ADC1->SQR3 |= ADC_SQR1_SQ13_1;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_2;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_3;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_4;
		break;
	}
	case CHANNEL_3:{
		ADC1->SQR3 |= ADC_SQR1_SQ13_0;
		ADC1->SQR3 |= ADC_SQR1_SQ13_1;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_2;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_3;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_4;
		break;
	}
	case CHANNEL_4:{
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_0;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_1;
		ADC1->SQR3 |= ADC_SQR1_SQ13_2;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_3;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_4;
		break;
	}
	case CHANNEL_5:{
		ADC1->SQR3 |= ADC_SQR1_SQ13_0;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_1;
		ADC1->SQR3 |= ADC_SQR1_SQ13_2;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_3;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_4;
		break;
	}
	case CHANNEL_6:{
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_0;
		ADC1->SQR3 |= ADC_SQR1_SQ13_1;
		ADC1->SQR3 |= ADC_SQR1_SQ13_2;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_3;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_4;
		break;
	}
	case CHANNEL_7:{
		ADC1->SQR3 |= ADC_SQR1_SQ13_0;
		ADC1->SQR3 |= ADC_SQR1_SQ13_1;
		ADC1->SQR3 |= ADC_SQR1_SQ13_2;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_3;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_4;
		break;
	}
	case CHANNEL_8:{
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_0;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_1;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_2;
		ADC1->SQR3 |= ADC_SQR1_SQ13_3;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_4;
		break;
	}
	case CHANNEL_9:{
		ADC1->SQR3 |= ADC_SQR1_SQ13_0;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_1;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_2;
		ADC1->SQR3 |= ADC_SQR1_SQ13_3;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_4;
		break;
	}
	case CHANNEL_10:{
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_0;
		ADC1->SQR3 |= ADC_SQR1_SQ13_1;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_2;
		ADC1->SQR3 |= ADC_SQR1_SQ13_3;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_4;
		break;
	}
	case CHANNEL_11:{
		ADC1->SQR3 |= ADC_SQR1_SQ13_0;
		ADC1->SQR3 |= ADC_SQR1_SQ13_1;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_2;
		ADC1->SQR3 |= ADC_SQR1_SQ13_3;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_4;
		break;
	}
	case CHANNEL_12:{
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_0;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_1;
		ADC1->SQR3 |= ADC_SQR1_SQ13_2;
		ADC1->SQR3 |= ADC_SQR1_SQ13_3;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_4;
		break;
	}
	case CHANNEL_13:{
		ADC1->SQR3 |= ADC_SQR1_SQ13_0;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_1;
		ADC1->SQR3 |= ADC_SQR1_SQ13_2;
		ADC1->SQR3 |= ADC_SQR1_SQ13_3;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_4;
		break;
	}
	case CHANNEL_14:{
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_0;
		ADC1->SQR3 |= ADC_SQR1_SQ13_1;
		ADC1->SQR3 |= ADC_SQR1_SQ13_2;
		ADC1->SQR3 |= ADC_SQR1_SQ13_3;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_4;
		break;
	}
	case CHANNEL_15:{
		ADC1->SQR3 |= ADC_SQR1_SQ13_0;
		ADC1->SQR3 |= ADC_SQR1_SQ13_1;
		ADC1->SQR3 |= ADC_SQR1_SQ13_2;
		ADC1->SQR3 |= ADC_SQR1_SQ13_3;
		ADC1->SQR3 &= ~ADC_SQR1_SQ13_4;
		break;
	}
	default:{
		break;
	}
	}
}


/*
 * Configura el enable de la interrupción y la activación del NVIC
 * */
static void adc_config_interrupt(ADC_Config_t *adcConfig){

	if(adcConfig->interrupState == ADC_INT_ENABLE){

		//ACTIVAMOS LA INTERRUPCIÓN
		ADC1->CR1 |= ADC_CR1_EOCIE;

		/* Activamos el canal del sistema NVIC para que lea la interrupción*/
		NVIC_EnableIRQ(ADC_IRQn);

	} else{

		//DESACTIVAMOS LA INTERRUPCIÓN
		ADC1->CR1 &= ~ADC_CR1_EOCIE;

		/* Desactivamos el canal del sistema NVIC para que lea la interrupción*/
		NVIC_DisableIRQ(ADC_IRQn);
	}
}


/*
 * Controla la ctivación y desactivación del modulo ADC desde el registro
 * CR2 del ADC
 * */
void adc_peripheralOnOFF(uint8_t state){

	if(state == ADC_ON){

		ADC1->CR2 |= ADC_CR2_ADON;

	}else{

		ADC1->CR2 &= ~ADC_CR2_ADON;
	}
}


/*
 *Enables and disables the ScanMode
 *Funciona de la mano con la secuencia de carios canales
 *NO es necesario para el caso de solo un canal simple
 * */
void adc_ScanMode(uint8_t state){

	if(state == SCAN_ON){

		ADC1->CR1 |= ADC_CR1_SCAN;

	}else{

		ADC1->CR1 &= ~ADC_CR1_SCAN;

	}
}


/*
 *Función que comienza la conversión ADC simple
 * */
void adc_StartSingleConv(void){

	//Se inicializa la conversión ADC simple
	ADC1->CR2 &= ~ADC_CR2_CONT;

	//Se inicializa la conversión ADC
	ADC1->CR2 |= ADC_CR2_SWSTART;

}


/*
 * Funcion que comienza la conversión ADC continua
 * */
void adc_StartContinuousConv(void){

	//Se inicializa la conversión ADC continua
	ADC1->CR2 |= ADC_CR2_CONT;

	//Se inicializa la conversión ADC
	ADC1->CR2 |= ADC_CR2_SWSTART;

}


/*
 * Funcion que termina la conversión ADC continua
 * */
void adc_StopContinuousConv(void){

	//Se inicializa la conversión ADC continua
	ADC1->CR2 &= ~ADC_CR2_CONT;
}


/*
 * Funcion que retorna el último dato adquirido por la ADC
 * */
uint16_t adc_Get_Value(void){
	return adcRawData;
}


__attribute__ ((weak)) void adc_CompleteCallback(void){
	__NOP();
}

/*
 * Esta es la ISR de la interrupción por conversión ADC
 * */
void ADC_IRQHandler(void){

	if(ADC1->CR1 & ADC_CR1_EOCIE){
		//Bajamos la bandera leyendo el dato
		adcRawData = ADC1->DR;

		//Se llama al callback
		adc_CompleteCallback();
	}
}


/*
 * Con esta función configuramod qué pin deseamos que funcione como ADC
 * */
void adc_ConfigAnalogPin(uint8_t adcChannel){

	switch(adcChannel){

	case CHANNEL_0:{
		handlerADCPin.pGPIOx                    = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinMode    = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber  = PIN_0;
		gpio_Config(&handlerADCPin);
		break;
	}
	case CHANNEL_1:{
		handlerADCPin.pGPIOx                    = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinMode    = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber  = PIN_1;
		gpio_Config(&handlerADCPin);
		break;
	}
	case CHANNEL_2:{
		handlerADCPin.pGPIOx                    = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinMode    = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber  = PIN_2;
		gpio_Config(&handlerADCPin);
		break;
	}
	case CHANNEL_3:{
		handlerADCPin.pGPIOx                    = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinMode    = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber  = PIN_3;
		gpio_Config(&handlerADCPin);
		break;
	}
	case CHANNEL_4:{
		handlerADCPin.pGPIOx                    = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinMode    = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber  = PIN_4;
		gpio_Config(&handlerADCPin);
		break;
	}
	case CHANNEL_5:{
		handlerADCPin.pGPIOx                    = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinMode    = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber  = PIN_5;
		gpio_Config(&handlerADCPin);
		break;
	}
	case CHANNEL_6:{
		handlerADCPin.pGPIOx                    = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinMode    = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber  = PIN_6;
		gpio_Config(&handlerADCPin);
		break;
	}
	case CHANNEL_7:{
		handlerADCPin.pGPIOx                    = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinMode    = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber  = PIN_7;
		gpio_Config(&handlerADCPin);
		break;
	}
	case CHANNEL_8:{
		handlerADCPin.pGPIOx                    = GPIOB;
		handlerADCPin.pinConfig.GPIO_PinMode    = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber  = PIN_0;
		gpio_Config(&handlerADCPin);
		break;
	}
	case CHANNEL_9:{
		handlerADCPin.pGPIOx                    = GPIOB;
		handlerADCPin.pinConfig.GPIO_PinMode    = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber  = PIN_1;
		gpio_Config(&handlerADCPin);
		break;
	}
	case CHANNEL_10:{
		handlerADCPin.pGPIOx                    = GPIOC;
		handlerADCPin.pinConfig.GPIO_PinMode    = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber  = PIN_0;
		gpio_Config(&handlerADCPin);
		break;
	}
	case CHANNEL_11:{
		handlerADCPin.pGPIOx                    = GPIOC;
		handlerADCPin.pinConfig.GPIO_PinMode    = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber  = PIN_1;
		gpio_Config(&handlerADCPin);
		break;
	}
	case CHANNEL_12:{
		handlerADCPin.pGPIOx                    = GPIOC;
		handlerADCPin.pinConfig.GPIO_PinMode    = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber  = PIN_2;
		gpio_Config(&handlerADCPin);
		break;
	}
	case CHANNEL_13:{
		handlerADCPin.pGPIOx                    = GPIOC;
		handlerADCPin.pinConfig.GPIO_PinMode    = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber  = PIN_3;
		gpio_Config(&handlerADCPin);
		break;
	}
	case CHANNEL_14:{
		handlerADCPin.pGPIOx                    = GPIOC;
		handlerADCPin.pinConfig.GPIO_PinMode    = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber  = PIN_4;
		gpio_Config(&handlerADCPin);
		break;
	}
	case CHANNEL_15:{
		handlerADCPin.pGPIOx                    = GPIOC;
		handlerADCPin.pinConfig.GPIO_PinMode    = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber  = PIN_5;
		gpio_Config(&handlerADCPin);
		break;
	}
	default:{
		break;
	}
	}
}


/*Configuración para hacer conversiones en multiples canales y en un orden específico*/

/* Configuración para trigger externo */
