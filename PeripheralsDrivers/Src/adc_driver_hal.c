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
static void adc_set_sampling_and_holdd(ADC_Config_t *adcConfig);
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

}
