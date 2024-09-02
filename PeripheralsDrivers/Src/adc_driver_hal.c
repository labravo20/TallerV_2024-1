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
	adc_ConfigAnalogPin(adcConfig->ADCx_Config.channel);

	/* 2. Activamos la señal de reloj para el ADC */
	adc_enable_clock_peripheral();

	//Limpiamos los registros antes de comenzar a configurar
	ADC1->CR1 = 0;
	ADC1->CR2 = 0;

	/* Comenzamos la configuración de ADC1 */

	/* 3. Resolución del ADC */
	adc_set_resolution(adcConfig);

	/* 4. Configuramos el modo Scan como desactivado */
	adc_ScanMode(adcConfig, SCAN_OFF);

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
	adc_peripheralOnOFF(adcConfig, ADC_ON);

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

	switch(adcConfig->ADCx_Config.resolution){

	case RESOLUTION_12_BIT:{

		// Debemos cargar el valor 0b00 en los 12 bit de resolución
		adcConfig->pADCx->CR1 &= ~ADC_CR1_RES_0;
		adcConfig->pADCx->CR1 &= ~ADC_CR1_RES_1;
		break;

	}
	case RESOLUTION_10_BIT:{

		// Debemos cargar el valor 0b01 en los 10 bit de resolución
		adcConfig->pADCx->CR1 |= ADC_CR1_RES_0;
		adcConfig->pADCx->CR1 &= ~ADC_CR1_RES_1;
		break;

	}
	case RESOLUTION_8_BIT:{

		// Debemos cargar el valor 0b10 en los 8 bit de resolución
		adcConfig->pADCx->CR1 &= ~ADC_CR1_RES_0;
		adcConfig->pADCx->CR1 |= ADC_CR1_RES_1;
		break;

	}
	case RESOLUTION_6_BIT:{

		// Debemos cargar el valor 0b11 en los 6 bit de resolución
		adcConfig->pADCx->CR1 |= ADC_CR1_RES_0;
		adcConfig->pADCx->CR1 |= ADC_CR1_RES_1;
		break;

	}
	default: {

		// En caso por defecto ebemos cargar el valor 0b00 en los 12 bit de resolución
		adcConfig->pADCx->CR1 &= ~ADC_CR1_RES_0;
		adcConfig->pADCx->CR1 &= ~ADC_CR1_RES_1;
		break;
	}
	}
}


/*
 * Set the number alignment_ left or right
 * */
static void adc_set_alignment(ADC_Config_t *adcConfig){

	if(adcConfig->ADCx_Config.dataAlignment == ALIGNMENT_RIGHT){
		adcConfig->pADCx->CR2 &= ~ADC_CR2_ALIGN;
	} else{
		adcConfig->pADCx->CR2 |= ADC_CR2_ALIGN;
	}
}


/*
 * Relacionando con el valor del tiempo de carga del capacitor HOLD
 * */
static void adc_set_sampling_and_hold(ADC_Config_t *adcConfig){

	switch(adcConfig->ADCx_Config.samplingPeriod){

	case SAMPLING_PERIOD_3_CYCLES: {
		if(adcConfig->ADCx_Config.channel == CHANNEL_0){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP0_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP0_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP0_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_1){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP1_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP1_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP1_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_2){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP2_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP2_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP2_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_3){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP3_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP3_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP3_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_4){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP4_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP4_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP4_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_5){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP5_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP5_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP5_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_6){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP6_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP6_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP6_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_7){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP7_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP7_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP7_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_8){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP8_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP8_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP8_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_9){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP9_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP9_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP9_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_10){
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP10_0;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP10_1;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP10_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_11){
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP11_0;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP11_1;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP11_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_12){
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP12_0;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP12_1;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP12_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_13){
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP13_0;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP13_1;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP13_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_14){
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP14_0;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP14_1;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP14_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_15){
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP15_0;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP15_1;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP15_2;
		}
	}
	case SAMPLING_PERIOD_15_CYCLES: {
		if(adcConfig->ADCx_Config.channel == CHANNEL_0){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP0_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP0_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP0_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_1){

			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP1_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP1_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP1_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_2){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP2_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP2_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP2_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_3){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP3_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP3_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP3_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_4){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP4_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP4_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP4_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_5){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP5_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP5_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP5_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_6){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP6_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP6_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP6_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_7){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP7_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP7_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP7_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_8){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP8_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP8_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP8_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_9){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP9_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP9_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP9_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_10){
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP10_0;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP10_1;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP10_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_11){
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP11_0;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP11_1;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP11_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_12){
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP12_0;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP12_1;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP12_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_13){
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP13_0;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP13_1;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP13_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_14){
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP14_0;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP14_1;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP14_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_15){
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP15_0;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP15_1;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP15_2;
		}
	}
	case SAMPLING_PERIOD_28_CYCLES: {
		if(adcConfig->ADCx_Config.channel == CHANNEL_0){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP0_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP0_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP0_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_1){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP1_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP1_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP1_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_2){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP2_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP2_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP2_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_3){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP3_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP3_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP3_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_4){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP4_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP4_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP4_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_5){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP5_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP5_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP5_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_6){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP6_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP6_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP6_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_7){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP7_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP7_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP7_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_8){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP8_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP8_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP8_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_9){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP9_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP9_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP9_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_10){
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP10_0;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP10_1;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP10_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_11){
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP11_0;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP11_1;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP11_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_12){
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP12_0;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP12_1;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP12_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_13){
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP13_0;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP13_1;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP13_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_14){
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP14_0;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP14_1;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP14_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_15){
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP15_0;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP15_1;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP15_2;
		}
	}
	case SAMPLING_PERIOD_56_CYCLES: {
		if(adcConfig->ADCx_Config.channel == CHANNEL_0){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP0_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP0_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP0_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_1){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP1_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP1_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP1_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_2){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP2_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP2_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP2_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_3){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP3_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP3_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP3_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_4){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP4_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP4_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP4_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_5){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP5_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP5_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP5_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_6){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP6_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP6_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP6_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_7){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP7_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP7_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP7_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_8){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP8_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP8_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP8_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_9){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP9_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP9_1;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP9_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_10){
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP10_0;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP10_1;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP10_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_11){
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP11_0;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP11_1;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP11_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_12){
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP12_0;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP12_1;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP12_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_13){
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP13_0;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP13_1;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP13_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_14){
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP14_0;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP14_1;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP14_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_15){
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP15_0;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP15_1;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP15_2;
		}
	}
	case SAMPLING_PERIOD_84_CYCLES: {
		if(adcConfig->ADCx_Config.channel == CHANNEL_0){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP0_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP0_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP0_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_1){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP1_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP1_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP1_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_2){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP2_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP2_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP2_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_3){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP3_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP3_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP3_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_4){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP4_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP4_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP4_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_5){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP5_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP5_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP5_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_6){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP6_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP6_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP6_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_7){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP7_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP7_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP7_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_8){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP8_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP8_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP8_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_9){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP9_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP9_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP9_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_10){
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP10_0;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP10_1;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP10_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_11){
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP11_0;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP11_1;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP11_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_12){
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP12_0;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP12_1;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP12_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_13){
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP13_0;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP13_1;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP13_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_14){
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP14_0;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP14_1;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP14_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_15){
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP15_0;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP15_1;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP15_2;
		}
	}
	case SAMPLING_PERIOD_112_CYCLES: {
		if(adcConfig->ADCx_Config.channel == CHANNEL_0){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP0_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP0_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP0_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_1){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP1_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP1_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP1_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_2){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP2_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP2_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP2_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_3){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP3_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP3_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP3_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_4){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP4_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP4_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP4_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_5){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP5_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP5_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP5_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_6){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP6_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP6_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP6_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_7){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP7_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP7_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP7_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_8){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP8_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP8_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP8_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_9){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP9_0;
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP9_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP9_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_10){
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP10_0;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP10_1;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP10_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_11){
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP11_0;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP11_1;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP11_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_12){
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP12_0;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP12_1;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP12_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_13){
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP13_0;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP13_1;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP13_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_14){
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP14_0;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP14_1;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP14_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_15){
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP15_0;
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP15_1;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP15_2;
		}
	}
	case SAMPLING_PERIOD_144_CYCLES: {
		if(adcConfig->ADCx_Config.channel == CHANNEL_0){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP0_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP0_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP0_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_1){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP1_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP1_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP1_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_2){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP2_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP2_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP2_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_3){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP3_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP3_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP3_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_4){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP4_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP4_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP4_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_5){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP5_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP5_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP5_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_6){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP6_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP6_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP6_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_7){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP7_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP7_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP7_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_8){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP8_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP8_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP8_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_9){
			adcConfig->pADCx->SMPR2 &= ~ADC_SMPR2_SMP9_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP9_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP9_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_10){
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP10_0;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP10_1;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP10_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_11){
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP11_0;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP11_1;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP11_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_12){
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP12_0;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP12_1;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP12_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_13){
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP13_0;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP13_1;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP13_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_14){
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP14_0;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP14_1;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP14_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_15){
			adcConfig->pADCx->SMPR1 &= ~ADC_SMPR1_SMP15_0;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP15_1;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP15_2;
		}
	}
	case SAMPLING_PERIOD_480_CYCLES: {
		if(adcConfig->ADCx_Config.channel == CHANNEL_0){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP0_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP0_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP0_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_1){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP1_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP1_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP1_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_2){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP2_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP2_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP2_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_3){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP3_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP3_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP3_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_4){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP4_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP4_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP4_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_5){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP5_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP5_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP5_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_6){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP6_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP6_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP6_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_7){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP7_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP7_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP7_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_8){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP8_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP8_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP8_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_9){
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP9_0;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP9_1;
			adcConfig->pADCx->SMPR2 |= ADC_SMPR2_SMP9_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_10){
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP10_0;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP10_1;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP10_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_11){
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP11_0;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP11_1;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP11_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_12){
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP12_0;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP12_1;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP12_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_13){
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP13_0;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP13_1;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP13_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_14){
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP14_0;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP14_1;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP14_2;

		}else if(adcConfig->ADCx_Config.channel == CHANNEL_15){
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP15_0;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP15_1;
			adcConfig->pADCx->SMPR1 |= ADC_SMPR1_SMP15_2;
		}
	}
	}
}


/*
 * Configura el numero de elementos en la secuencia (solo un elemento)
 * Configura cual es el canal que adquiere la señal ADC
 * */
static void adc_set_one_channel_sequence(ADC_Config_t *adcConfig){

	//Se realiza la selección para UN CANAL
	adcConfig->pADCx->CR1 &= ~ADC_CR1_DISCNUM_0;
	adcConfig->pADCx->CR1 &= ~ADC_CR1_DISCNUM_1;
	adcConfig->pADCx->CR1 &= ~ADC_CR1_DISCNUM_2;
}


/*
 * Configura el enable de la interrupción y la activación del NVIC
 * */
static void adc_config_interrupt(ADC_Config_t *adcConfig){

	if(adcConfig->ADCx_Config.interrupState == ADC_INT_ENABLE){

		//ACTIVAMOS LA INTERRUPCIÓN
		adcConfig->pADCx->CR1 |= ADC_CR1_EOCIE;

		/* Activamos el canal del sistema NVIC para que lea la interrupción*/
		NVIC_EnableIRQ(ADC_IRQn);

	} else{

		//DESACTIVAMOS LA INTERRUPCIÓN
		adcConfig->pADCx->CR1 &= ~ADC_CR1_EOCIE;

		/* Desactivamos el canal del sistema NVIC para que lea la interrupción*/
		NVIC_DisableIRQ(ADC_IRQn);
	}
}


/*
 * Controla la ctivación y desactivación del modulo ADC desde el registro
 * CR2 del ADC
 * */
void adc_peripheralOnOFF(ADC_Config_t *adcConfig, uint8_t state){

	if(state == ADC_ON){

		adcConfig->pADCx->CR2 |= ADC_CR2_ADON;

	}else{

		adcConfig->pADCx->CR2 &= ~ADC_CR2_ADON;
	}
}


/*
 *Enables and disables the ScanMode
 *Funciona de la mano con la secuencia de carios canales
 *NO es necesario para el caso de solo un canal simple
 * */
void adc_ScanMode(ADC_Config_t *adcConfig,uint8_t state){

	if(state == SCAN_ON){

		adcConfig->pADCx->CR1 |= ADC_CR1_SCAN;

	}else{

		adcConfig->pADCx->CR1 &= ~ADC_CR1_SCAN;

	}
}


/*
 *Función que comienza la conversión ADC simple
 * */
void adc_StartSingleConv(void){

	//Se inicializa la conversión ADC
	ADC1->CR2 |= ADC_CR2_SWSTART;

	//Se inicializa la conversión ADC simple
	ADC1->CR2 &= ~ADC_CR2_CONT;
}


/*
 * Funcion que comienza la conversión ADC continua
 * */
void adc_StartContinuousConv(void){

	//Se inicializa la conversión ADC
	ADC1->CR2 |= ADC_CR2_SWSTART;

	//Se inicializa la conversión ADC continua
	ADC1->CR2 |= ADC_CR2_CONT;
}


/*
 * Funcion que termina la conversión ADC continua
 * */
void adc_StopContinuousConv(void){

	//Conversion complete
	ADC1->SR |= ADC_SR_EOC;
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

	//Bajamos la bandera
	ADC1->SR &= ~ ADC_SR_STRT;

	//Se llama al callback
	adc_CompleteCallback();
}


/*Configuración para hacer conversiones en multiples canales y en un orden específico*/

/* Configuración para trigger externo */
