/*
 * pwm_driver_hal.c
 *
 *  Created on: 5/09/2024
 *      Author: laurasofia
 */

#include "pwm_driver_hal.h"
#include "timer_driver_hal.h"

/**/
void pwm_Config(PWM_Handler_t *ptrPwmHandler){

	/* 0. Activar la señal de reloh del periférico requerido */
	if(ptrPwmHandler->ptrTIMx == TIM2){
		//Agregue su código acá
	}
	else if(ptrPwmHandler->ptrTIMx == TIM3){
		//Agregue su código acá
	}
	else if(ptrPwmHandler->ptrTIMx == TIM4){
		//Agregue su código acá
	}
	else if (ptrPwmHandler->ptrTIMx == TIM5){
		//Agregue su código acá
	}
	else{
		__NOP();
	}

	/* 1. Cargamos la frecuencia deseada */
	pwm_Set_Frequency(ptrPwmHandler);

	/* 2. Cargamos el valor del dutty cycle */
	pwm_Set_DuttyCycle(ptrPwmHandler);

	/* 2a. Estamos en UP_Mode, el límite se carga en ARR y se comienza en 0 */
	ptrPwmHandler->ptrTIMx->CR1  &= ~TIM_CR1_DIR;

	/* 3. Configuramos los bits CCxS del registro TIMy_CCMR1, de forma que sea modo salida
	 * (para cada canal hay un conjunto CCxS
	 *
	 * 4. Además, en el mismo "case" podemos configurar el modo del PWM, su polaridad...
	 *
	 * 5. Y adicionalmente activamos el preload bit para que cada vez que exista u update-event
	 * el valor cargado en el CCRx será recargado en el registro "shadow" del PWM*/
	switch(ptrPwmHandler->config.channel){
	case PWM_CHANNEL_1:{
		//Seleccionamos como salida el canal
		ptrPwmHandler->ptrTIMx->CCMR1 &= ~(TIM_CCMR1_CC1S);

		//Configuramos el canal como PWM
		//Agregue su código acá

		//Activamos la funcionalidad de pre-load
		//Agregue su código acá

		break;
	}
	case PWM_CHANNEL_2:{
		//Seleccionamos como salida el canal
		//Agregue su código acá

		//Configuramos el canal como PWM
		//Agregue su código acá

		//Activamos la funcionalidad de pre-load
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC2PE;

		break;
	}
	case PWM_CHANNEL_3:{
		//Seleccionamos como salida el canal
		//Agregue su código acá

		//Configuramos el canal como PWM
		ptrPwmHandler->ptrTIMx->CCMR2 |= (0b110 << TIM_CCMR2_OC3M_Pos);

		//Activamos la funcionalidad de pre-load
		//Agregue su código acá

		break;
	}
	case PWM_CHANNEL_4:{
		//Seleccionamos como salida el canal
		//Agregue su código acá

		//Configuramos el canal como PWM
		//Agregue su código acá

		//Activamos la funcionalidad de pre-load
		//Agregue su código acá

		break;
	}
	default:{
		break;
	}

	/* 6. Activamos la salida seleccionada */
	pwm_Enable_Output(ptrPwmHandler);

	}//Fin del Switch case
}

/**/
void pwm_Start_Signal(PWM_Handler_t *ptrPwmHandler){
	//Agregue su código acá
}

/**/
void pwm_Stop_Signal(PWM_Handler_t *ptrPwmHandler){
	//Agregue su código acá
}


void pwm_Change_OutputPolarity(PWM_Handler_t *ptrPwmHandler){
	switch(ptrPwmHandler->config.channel){
	case PWM_CHANNEL_1:{
		//Polaridad para la salida del canal 1
		if(ptrPwmHandler->config.polarity == PWM_ACTIVE_HIGH){
			//Agregué su código acá
		}else{
			//Agregué su códig acá
		}

		break;
	}
	case PWM_CHANNEL_2:{
		//Polaridad para la salida del canal 2
		if(ptrPwmHandler->config.polarity == PWM_ACTIVE_HIGH){
			//Agregué su código acá
		}else{
			//Agregué su código acá
		}

		break;
	}
	case PWM_CHANNEL_3:{
		//Polaridad para la salida del canal 3
		if(ptrPwmHandler->config.polarity == PWM_ACTIVE_HIGH){
			ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC3P;
		}else{
			//Agregué su código acá
		}

		break;
	}
	case PWM_CHANNEL_4:{
		//Polaridad para la salida del canal 4
		if(ptrPwmHandler->config.polarity == PWM_ACTIVE_HIGH){
			//Agregué su código acá
		}else{
			ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC4P;
		}

		break;
	}
	default:{
		break;
	}
	}
}

/**/
void pwm_Enable_Output(PWM_Handler_t *ptrPwmHandler){
	switch(ptrPwmHandler->config.channel){
	case PWM_CHANNEL_1:{
		//Activamos la salida del canal 1
		//Agregue su código acá
		break;
	}
	case PWM_CHANNEL_2:{
		//Activamos la salida del canal 2
		//Agregue su código acá
		break;
	}
	case PWM_CHANNEL_3:{
		//Activamos la salida del canal 3
		//Agregue su código acá
		break;
	}
	case PWM_CHANNEL_4:{
		//Activamos la salida del canal 4
		//Agregue su código acá
		break;
	}
	default:{
		break;
	}
	}
}

/**/
void pwm_Disable_Output(PWM_Handler_t *ptrPwmHandler){
	switch(ptrPwmHandler->config.channel){
	case PWM_CHANNEL_1:{
		//Desactivamos la salida del canal 1
		//Agregue su código acá
		break;
	}
	case PWM_CHANNEL_2:{
		//Desactivamos la salida del canal 2
		//Agregue su código acá
		break;
	}
	case PWM_CHANNEL_3:{
		//Desactivamos la salida del canal 3
		//Agregue su código acá
		break;
	}
	case PWM_CHANNEL_4:{
		//Desactivamos la salida del canal 4
		//Agregue su código acá
		break;
	}
	default:{
		break;
	}
	}
}

/**/
void pwm_Enable_Event(PWM_Handler_t *ptrPwmHandler){
	switch(ptrPwmHandler->config.channel){
	case PWM_CHANNEL_1:{
		//Activamos el evento del canal 1
		//Agregue su código acá
		break;
	}
	case PWM_CHANNEL_2:{
		//Activamos el evento del canal 2
		//Agregue su código acá
		break;
	}
	case PWM_CHANNEL_3:{
		//Activamos el evento del canal 3
		//Agregue su código acá
		break;
	}
	case PWM_CHANNEL_4:{
		//Activamos el evento del canal 4
		//Agregue su código acá
		break;
	}
	default:{
		break;
	}
	}
}

/**/
void pwm_Disable_Event(PWM_Handler_t *ptrPwmHandler){
	switch(ptrPwmHandler->config.channel){
	case PWM_CHANNEL_1:{
		//Desactivamos el evento del canal 1
		//Agregue su código acá
		break;
	}
	case PWM_CHANNEL_2:{
		//Desactivamos el evento del canal 2
		//Agregue su código acá
		break;
	}
	case PWM_CHANNEL_3:{
		//Desactivamos el evento del canal 3
		//Agregue su código acá
		break;
	}
	case PWM_CHANNEL_4:{
		//Desactivamos el evento del canal 4
		//Agregue su código acá
		break;
	}
	default:{
		break;
	}
	}
}

/*La frecuencia es definida por el conjunto formado por el prescaler (PSC)
 * y el valor límite al que llega el timer (ARR), con estos dos se establece
 * la frecuencia
 * */
void pwm_Set_Frequency(PWM_Handler_t *ptrPwmHandler){
	//Cargamos el valor del prescaler, nos define la velocidad (en ns) a la cual
	//se incrementa el timer
	//Agregue su código acá

	//Cargamos el valor del ARR, el cual es el límite de incrementos del Timer
	//antes de hacer un opdate y reload
	//Agregue su código acá
}

/**/
void pwm_Update_Frequency(PWM_Handler_t *ptrPwmHandler, uint16_t newFreq){
	ptrPwmHandler->config.periodo = newFreq;

	pwm_Set_Frequency(ptrPwmHandler);
}

/*El valor del dutty debe estar dado en valores de %, entre 0% y 100%*/
void pwm_Set_DuttyCycle(PWM_Handler_t *ptrPwmHandler){

//	uint32_t auxDutty = 0;
//
//	//Verificamos que los valores estan en los limites adecuados
//	if(ptrPwmHandler->config.duttyCicle > PWM_DUTTY_100_PERCENT){
//		ptrPwmHandler->config.duttyCicle = PWM_DUTTY_100_PERCENT;
//	}
//	else if(ptrPwmHandler->config.duttyCicle < PWM_DUTTY_0_PERCENT){
//		ptrPwmHandler->config.duttyCicle = PWM_DUTTY_0_PERCENT;
//	}
//
//	auxDutty = (ptrPwmHandler->config.periodo *ptrPwmHandler->config.duttyCicle) / PWM_DUTTY_100_PERCENT;
//
	//Seleccionamos el canal para configurar su dutty
	switch(ptrPwmHandler->config.channel){
	case PWM_CHANNEL_1:{
		ptrPwmHandler->ptrTIMx->CCR1 = ptrPwmHandler->config.duttyCicle;

		break;
	}
	case PWM_CHANNEL_2:{
		//Agregue su código acá

		break;
	}
	case PWM_CHANNEL_3:{
		//Agregue su código acá

		break;
	}
	case PWM_CHANNEL_4:{
		//Agregue su código acá

		break;
	}
	default:{
		break;
	}
	}//Fin del switch case
}

/**/
void pwm_Update_DuttyCycle(PWM_Handler_t *ptrPwmHandler, uint16_t newDutty){
	ptrPwmHandler->config.duttyCicle = newDutty;

	pwm_Set_DuttyCycle(ptrPwmHandler);
}








