/*
 * pwm_driver_hal.h
 *
 *  Created on: 5/09/2024
 *      Author: laurasofia
 */

#ifndef PWM_DRIVER_HAL_H_
#define PWM_DRIVER_HAL_H_

#include "stm32f4xx.h"

enum{
	PWM_CHANNEL_1 = 0,
	PWM_CHANNEL_2,
	PWM_CHANNEL_3,
	PWM_CHANNEL_4
};

enum{
	PWM_DUTTY_0_PERCENT = 0,
	PWM_DUTTY_100_PERCENT
};

enum{
	PWM_ACTIVE_HIGH = 0,
	PWM_ACTIVE_LOW
};

/**/
typedef struct
{
	uint8_t    channel;        //Canal PWM relacionado con el TIMER
	uint32_t   prescaler;      //A qué velocidad se incrementa el timer
	uint16_t   periodo;        //Indica el número de veces que el timer se incrementa, el
	                           //periodo de la frecuencia viene dado por Time_Fosc * PSC *ARR
	uint16_t   duttyCicle;      //Valor en porcentaje del tiempo que la señal está en alto
	uint8_t    polarity;       //Polaridad de la señal de salida
} PWM_Config_t;


/**/
typedef struct
{
	TIM_TypeDef      *ptrTIMx;   //Timer al que está asociado el PWM
	PWM_Config_t     config;     //Configuración inicial PWM
}PWM_Handler_t;

/*Prototipos de las funciones*/
void pwm_Config(PWM_Handler_t *ptrPwmHandler);
void pwm_Set_Frequency(PWM_Handler_t *ptrPwmHandler);
void pwm_Update_Frequency(PWM_Handler_t *ptrPwmHandler, uint16_t newFreq);
void pwm_Set_DuttyCycle(PWM_Handler_t *ptrPwmHandler);
void pwm_Update_DuttyCycle(PWM_Handler_t *ptrPwmHandler, uint16_t newDutty);
void pwm_Enable_Output(PWM_Handler_t *ptrPwmHandler);
void pwm_Disable_Output(PWM_Handler_t *ptrPwmHandler);
void pwm_Start_Signal(PWM_Handler_t *ptrPwmHandler);
void pwm_Stop_Signal(PWM_Handler_t *ptrPwmHandler);
void pwm_Change_OutputPolarity(PWM_Handler_t *ptrPwmHandler);
void pwm_Enable_Event(PWM_Handler_t *ptrPwmHandler);
void pwm_Disable_Event(PWM_Handler_t *ptrPwmHandler);

#endif /* PWM_DRIVER_HAL_H_ */
