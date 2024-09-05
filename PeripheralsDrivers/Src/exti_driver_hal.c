/*
 * exti_driver_hal.c
 *
 *  Created on: 9/08/2024
 *      Author: laurasofia
 */

#include "exti_driver_hal.h"
#include "gpio_driver_hal.h"

/* === Headers for private functions === */
static void exti_enable_clock_peripheral(void);
static void exti_assign_channel(EXTI_Config_t *extiConfig);
static void exti_select_edge(EXTI_Config_t *extiConfig);
static void exti_config_interrupt(EXTI_Config_t *extiConfig);

/*
 * Funcion de configuracion del sistema EXTI.
 * Requiere que un pinX ya se encuentre configurado como
 * entrada digital
 * */
void exti_Config(EXTI_Config_t *extiConfig){

	/* 1.0 Se carga la configuración, que debe ser el PINx como entrada "simple" */
	gpio_Config(extiConfig->pGPIOHandler);

	/* 2.0 Activamos el acceso al SYSCFG */
	exti_enable_clock_peripheral();

	/* 3.0 Seleccion de canal */
	exti_assign_channel(extiConfig);

	/* 4.0 Seleccionamos el tipo de flanco */
	exti_select_edge(extiConfig);

	/* 5.0 Desactivo primero las interrupciones globales */
	__disable_irq();

	/* 6. 0 Manejo de Interrupciones */
	exti_config_interrupt(extiConfig);

	/* 7.0 Volvemos a activar las interrupciones globales */
	__enable_irq();
}

/*
 * No requiere el periferico, ya que solo es necesario activar
 * al SYCFG
 * */
static void exti_enable_clock_peripheral(void){

	/* 2.0 Activamos el acceso al SYSCFG */
	/*Asignamos un SET en la posición correspondiente al SYSCFG dentro de la opción enable clock peripheral*/
	RCC->APB2ENR |= (RCC_APB2ENR_SYSCFGEN) ;
}

/*
 * Funcion que configura los MUX para asignar el pinX del puerto Y
 * a la entrada EXTI correspondiente.
 * */
static void exti_assign_channel(EXTI_Config_t *extiConfig){

	/*Asignamos el canal EXTI que corresponde al PIN_y del puerto GPIO_X
		 * Debemos activar la línea PIN_Xy (Y = A, B, C... y x = 0, 1, 2, 3...)
		 * en el módulo EXTI */
		switch (extiConfig->pGPIOHandler->pinConfig.GPIO_PinNumber) {

		/* Configurando para todos los pines GPIOX_0*/
		case 0: {

			/* SYSCFG_EXTICR1 */
			// Limpiamos primero la posición que deseamos configurar
			SYSCFG->EXTICR[0] &= ~(0xF << SYSCFG_EXTICR1_EXTI0_Pos);

			// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			// del puerto que vamos a utilizar: GPIOA_0, ó GPIOB_0, ó GPIOC_0, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PA);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PB);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PC);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PD);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PE);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PH);

			} else {
				__NOP();
			}

			break;
		}

		/* Configurando para el todos los pines GPIOX_1*/
		case 1: {

			/* SYSCFG_EXTICR1 */
			// Limpiamos primero la posición que deseamos configurar
			SYSCFG->EXTICR[0] &= ~(0xF << SYSCFG_EXTICR1_EXTI1_Pos);

			// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			// del puerto que vamos a utilizar: GPIOA_0, ó GPIOB_0, ó GPIOC_0, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI1_PA);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI1_PB);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI1_PC);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI1_PD);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI1_PE);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI1_PH);

			} else {
				__NOP();
			}

			break;
		}

	    /* Configurando para el todos los pines GPIOX_2*/
		case 2: {

			/* SYSCFG_EXTICR1 */
			// Limpiamos primero la posición que deseamos configurar
			SYSCFG->EXTICR[0] &= ~(0xF << SYSCFG_EXTICR1_EXTI2_Pos);

			// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			// del puerto que vamos a utilizar: GPIOA_0, ó GPIOB_0, ó GPIOC_0, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PA);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PB);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PC);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PD);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PE);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PH);

			} else {
				__NOP();
			}

			break;
		}

		/* Configurando para el todos los pines GPIOX_3*/
		case 3: {

			/* SYSCFG_EXTICR1 */
			// Limpiamos primero la posición que deseamos configurar
			SYSCFG->EXTICR[0] &= ~(0xF << SYSCFG_EXTICR1_EXTI3_Pos);

			// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			// del puerto que vamos a utilizar: GPIOA_0, ó GPIOB_0, ó GPIOC_0, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PA);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PB);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PC);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PD);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PE);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PH);

			} else {
				__NOP();
			}

			break;
		}

		/* Configurando para el todos los pines GPIOX_4*/
		case 4: {

			/* SYSCFG_EXTICR2 */
			// Limpiamos primero la posición que deseamos configurar
			SYSCFG->EXTICR[1] &= ~(0xF << SYSCFG_EXTICR2_EXTI4_Pos);

			// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			// del puerto que vamos a utilizar: GPIOA_0, ó GPIOB_0, ó GPIOC_0, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI4_PA);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI4_PB);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI4_PC);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI4_PD);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI4_PE);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI4_PH);

			} else {
				__NOP();
			}

			break;
		}

		/* Configurando para el todos los pines GPIOX_5*/
		case 5: {

			/* SYSCFG_EXTICR2 */
			// Limpiamos primero la posición que deseamos configurar
			SYSCFG->EXTICR[1] &= ~(0xF << SYSCFG_EXTICR2_EXTI5_Pos);

			// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			// del puerto que vamos a utilizar: GPIOA_0, ó GPIOB_0, ó GPIOC_0, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PA);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PB);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PC);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PD);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PE);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PH);

			} else {
				__NOP();
			}

			break;
		}

		/* Configurando para el todos los pines GPIOX_6*/
		case 6: {

			/* SYSCFG_EXTICR2 */
			// Limpiamos primero la posición que deseamos configurar
			SYSCFG->EXTICR[1] &= ~(0xF << SYSCFG_EXTICR2_EXTI6_Pos);

			// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			// del puerto que vamos a utilizar: GPIOA_0, ó GPIOB_0, ó GPIOC_0, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI6_PA);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI6_PB);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI6_PC);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI6_PD);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI6_PE);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI6_PH);

			} else {
				__NOP();
			}

			break;
		}

		/* Configurando para el todos los pines GPIOX_7*/
		case 7: {

			/* SYSCFG_EXTICR2 */
			// Limpiamos primero la posición que deseamos configurar
			SYSCFG->EXTICR[1] &= ~(0xF << SYSCFG_EXTICR2_EXTI7_Pos);

			// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			// del puerto que vamos a utilizar: GPIOA_0, ó GPIOB_0, ó GPIOC_0, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI7_PA);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI7_PB);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI7_PC);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI7_PD);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI7_PE);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
				SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI7_PH);

			} else {
				__NOP();
			}

			break;
		}

		/* Configurando para el todos los pines GPIOX_8*/
		case 8: {

			/* SYSCFG_EXTICR3 */
			// Limpiamos primero la posición que deseamos configurar
			SYSCFG->EXTICR[2] &= ~(0xF << SYSCFG_EXTICR3_EXTI8_Pos);

			// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			// del puerto que vamos a utilizar: GPIOA_0, ó GPIOB_0, ó GPIOC_0, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PA);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PB);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PC);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PD);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PE);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PH);

			} else {
				__NOP();
			}

			break;
		}

		/* Configurando para el todos los pines GPIOX_9*/
		case 9: {

			/* SYSCFG_EXTICR3 */
			// Limpiamos primero la posición que deseamos configurar
			SYSCFG->EXTICR[2] &= ~(0xF << SYSCFG_EXTICR3_EXTI9_Pos);

			// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			// del puerto que vamos a utilizar: GPIOA_0, ó GPIOB_0, ó GPIOC_0, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PA);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PB);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PC);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PD);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PE);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PH);

			} else {
				__NOP();
			}

			break;
		}

		/* Configurando para el todos los pines GPIOX_10*/
		case 10: {

			/* SYSCFG_EXTICR3 */
			// Limpiamos primero la posición que deseamos configurar
			SYSCFG->EXTICR[2] &= ~(0xF << SYSCFG_EXTICR3_EXTI10_Pos);

			// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			// del puerto que vamos a utilizar: GPIOA_0, ó GPIOB_0, ó GPIOC_0, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI10_PA);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI10_PB);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI10_PC);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI10_PD);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI10_PE);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI10_PH);

			} else {
				__NOP();
			}

			break;
		}

		/* Configurando para el todos los pines GPIOX_11*/
		case 11: {

			/* SYSCFG_EXTICR3 */
			// Limpiamos primero la posición que deseamos configurar
			SYSCFG->EXTICR[2] &= ~(0xF << SYSCFG_EXTICR3_EXTI11_Pos);

			// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			// del puerto que vamos a utilizar: GPIOA_0, ó GPIOB_0, ó GPIOC_0, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PA);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PB);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PC);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PD);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PE);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
				SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PH);

			} else {
				__NOP();
			}

			break;
		}

		/* Configurando para el todos los pines GPIOX_12 */
		case 12: {

			/* SYSCFG_EXTICR4 */
		    // Limpiamos primero la posición que deseamos configurar
		    SYSCFG->EXTICR[3] &= ~(0xF << SYSCFG_EXTICR4_EXTI12_Pos);

		    // Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		    // del puerto que vamos a utilizar: GPIOA_0, ó GPIOB_0, ó GPIOC_0, etc
		    if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
		    	SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PA);

		    } else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			    SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PB);

	    	} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			    SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PC);

		    } else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			    SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PD);

		    } else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			    SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PE);

		    } else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			    SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PH);

		    } else {
			    __NOP();
		    }

		    break;
		}

		/* Configurando para el todos los pines GPIOX_13 */
		case 13: {

			/* SYSCFG_EXTICR4 */
		    // Limpiamos primero la posición que deseamos configurar
		    SYSCFG->EXTICR[3] &= ~(0xF << SYSCFG_EXTICR4_EXTI13_Pos);

		    // Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		    // del puerto que vamos a utilizar: GPIOA_0, ó GPIOB_0, ó GPIOC_0, etc
		    if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
		    	SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PA);

		    } else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			    SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PB);

	    	} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			    SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PC);

		    } else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			    SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PD);

		    } else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			    SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PE);

		    } else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			    SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PH);

		    } else {
			    __NOP();
		    }

		    break;
		}

		/* Configurando para el todos los pines GPIOX_14 */
		case 14: {

			/* SYSCFG_EXTICR4 */
		    // Limpiamos primero la posición que deseamos configurar
		    SYSCFG->EXTICR[3] &= ~(0xF << SYSCFG_EXTICR4_EXTI14_Pos);

		    // Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		    // del puerto que vamos a utilizar: GPIOA_0, ó GPIOB_0, ó GPIOC_0, etc
		    if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
		    	SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI14_PA);

		    } else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			    SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI14_PB);

	    	} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			    SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI14_PC);

		    } else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			    SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI14_PD);

		    } else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			    SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI14_PE);

		    } else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			    SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI14_PH);

		    } else {
			    __NOP();
		    }

		    break;
		}

		/* Configurando para el todos los pines GPIOX_15 */
		case 15: {

			/* SYSCFG_EXTICR4 */
		    // Limpiamos primero la posición que deseamos configurar
		    SYSCFG->EXTICR[3] &= ~(0xF << SYSCFG_EXTICR4_EXTI15_Pos);

		    // Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		    // del puerto que vamos a utilizar: GPIOA_0, ó GPIOB_0, ó GPIOC_0, etc
		    if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
		    	SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI15_PA);

		    } else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			    SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI15_PB);

	    	} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			    SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI15_PC);

		    } else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			    SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI15_PD);

		    } else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			    SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI15_PE);

		    } else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			    SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI15_PH);

		    } else {
			    __NOP();
		    }

		    break;
		}

		default: {
			__NOP();

			break;
		}

		}// Fin del switch-case

}

/*
 * Funcion para seleccionar adecuadamente el flanco que lanza la interrupcion
 * en el canal EXTI especifico.
 * */
static void exti_select_edge(EXTI_Config_t *extiConfig){

	if(extiConfig->edgeType == EXTERNAL_INTERRUPT_FALLING_EDGE){
		/* Falling Trigger selection register*/

		// Select TRx for falling Trigger event configuration bit of line x
		// Se está interpretanto el número del pin x en uso cómo la relación con el "bit of line x"
		switch (extiConfig->pGPIOHandler->pinConfig.GPIO_PinNumber) {

		//Configuramos para el pin 0
		case 0: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->FTSR &= ~(EXTI_FTSR_TR0);

			//Falling trigger enabled for input line.
			EXTI->FTSR |= (EXTI_FTSR_TR0);

			break;
		}

		//Configuramos para el pin 1
		case 1: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->FTSR &= ~(EXTI_FTSR_TR1);

			//Falling trigger enabled for input line.
			EXTI->FTSR |= (EXTI_FTSR_TR1);

			break;
		}

		//Configuramos para el pin 2
		case 2: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->FTSR &= ~(EXTI_FTSR_TR2);

			//Falling trigger enabled for input line.
			EXTI->FTSR |= (EXTI_FTSR_TR2);

			break;
		}

		//Configuramos para el pin 3
		case 3: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->FTSR &= ~(EXTI_FTSR_TR3);

			//Falling trigger enabled for input line.
			EXTI->FTSR |= (EXTI_FTSR_TR3);

			break;
		}

		//Configuramos para el pin 4
		case 4: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->FTSR &= ~(EXTI_FTSR_TR4);

			//Falling trigger enabled for input line.
			EXTI->FTSR |= (EXTI_FTSR_TR4);

			break;
		}

		//Configuramos para el pin 5
		case 5: {
			// Limpiamos primero la posición que deseamos configurar
			EXTI->FTSR &= ~(EXTI_FTSR_TR5);

			//Falling trigger enabled for input line.
			EXTI->FTSR |= (EXTI_FTSR_TR5);

			break;
		}

		//Configuramos para el pin 6
		case 6: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->FTSR &= ~(EXTI_FTSR_TR6);

			//Falling trigger enabled for input line.
			EXTI->FTSR |= (EXTI_FTSR_TR6);

			break;
		}

		//Configuramos para el pin 7
		case 7: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->FTSR &= ~(EXTI_FTSR_TR7);

			//Falling trigger enabled for input line.
			EXTI->FTSR |= (EXTI_FTSR_TR7);

			break;
		}

		//Configuramos para el pin 8
		case 8: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->FTSR &= ~(EXTI_FTSR_TR8);

			//Falling trigger enabled for input line.
			EXTI->FTSR |= (EXTI_FTSR_TR8);

			break;
		}

		//Configuramos para el pin 9
		case 9: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->FTSR &= ~(EXTI_FTSR_TR9);

			//Falling trigger enabled for input line.
			EXTI->FTSR |= (EXTI_FTSR_TR9);

			break;
		}

		//Configuramos para el pin 10
		case 10: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->FTSR &= ~(EXTI_FTSR_TR10);

			//Falling trigger enabled for input line.
			EXTI->FTSR |= (EXTI_FTSR_TR10);

			break;
		}

		//Configuramos para el pin 11
		case 11: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->FTSR &= ~(EXTI_FTSR_TR11);

			//Falling trigger enabled for input line.
			EXTI->FTSR |= (EXTI_FTSR_TR11);

			break;
		}

		//Configuramos para el pin 12
		case 12: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->FTSR &= ~(EXTI_FTSR_TR12);

			//Falling trigger enabled for input line.
			EXTI->FTSR |= (EXTI_FTSR_TR12);

			break;
		}

		//Configuramos para el pin 13
		case 13: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->FTSR &= ~(EXTI_FTSR_TR13);

			//Falling trigger enabled for input line.
			EXTI->FTSR |= (EXTI_FTSR_TR13);

			break;
		}

		//Configuramos para el pin 14
		case 14: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->FTSR &= ~(EXTI_FTSR_TR14);

			//Falling trigger enabled for input line.
			EXTI->FTSR |= (EXTI_FTSR_TR14);

			break;
		}

		//Configuramos para el pin 15
		case 15: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->FTSR &= ~(EXTI_FTSR_TR15);

			//Falling trigger enabled for input line.
			EXTI->FTSR |= (EXTI_FTSR_TR15);

			break;
		}
		default:{
			break;
		}

		}
	}
	else if(extiConfig->edgeType == EXTERNAL_INTERRUPT_RISING_EDGE){
		/* Rising Trigger selection register*/

		// Select TRx for rising Trigger event configuration bit of line x
		// Se está interpretanto el número del pin x en uso cómo la relación con el "bit of line x"
		switch (extiConfig->pGPIOHandler->pinConfig.GPIO_PinNumber) {

		//Configuramos para el pin 0
		case 0: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->RTSR &= ~(EXTI_RTSR_TR0);

			//Rising trigger enabled for input line.
			EXTI->RTSR |= (EXTI_RTSR_TR0);

			break;
		}

		//Configuramos para el pin 1
		case 1: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->RTSR &= ~(EXTI_RTSR_TR1);

			//Rising trigger enabled for input line.
			EXTI->RTSR |= (EXTI_RTSR_TR1);

			break;
		}

		//Configuramos para el pin 2
		case 2: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->RTSR &= ~(EXTI_RTSR_TR2);

			//Rising trigger enabled for input line.
			EXTI->RTSR |= (EXTI_RTSR_TR2);

			break;
		}

		//Configuramos para el pin 3
		case 3: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->RTSR &= ~(EXTI_RTSR_TR3);

			//Rising trigger enabled for input line.
			EXTI->RTSR |= (EXTI_RTSR_TR3);

			break;
		}

		//Configuramos para el pin 4
		case 4: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->RTSR &= ~(EXTI_RTSR_TR4);

			//Rising trigger enabled for input line.
			EXTI->RTSR |= (EXTI_RTSR_TR4);

			break;
		}

		//Configuramos para el pin 5
		case 5: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->RTSR &= ~(EXTI_RTSR_TR5);

			//Rising trigger enabled for input line.
			EXTI->RTSR |= (EXTI_RTSR_TR5);

			break;
		}

		//Configuramos para el pin 6
		case 6: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->RTSR &= ~(EXTI_RTSR_TR6);

			//Rising trigger enabled for input line.
			EXTI->RTSR |= (EXTI_RTSR_TR6);

			break;
		}

		//Configuramos para el pin 7
		case 7: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->RTSR &= ~(EXTI_RTSR_TR7);

			//Rising trigger enabled for input line.
			EXTI->RTSR |= (EXTI_RTSR_TR7);

			break;
		}

		//Configuramos para el pin 8
		case 8: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->RTSR &= ~(EXTI_RTSR_TR8);

			//Rising trigger enabled for input line.
			EXTI->RTSR |= (EXTI_RTSR_TR8);

			break;
		}

		//Configuramos para el pin 9
		case 9: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->RTSR &= ~(EXTI_RTSR_TR9);

			//Rising trigger enabled for input line.
			EXTI->RTSR |= (EXTI_RTSR_TR9);

			break;
		}

		//Configuramos para el pin 10
		case 10: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->RTSR &= ~(EXTI_RTSR_TR10);

			//Rising trigger enabled for input line.
			EXTI->RTSR |= (EXTI_RTSR_TR10);

			break;
		}

		//Configuramos para el pin 11
		case 11: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->RTSR &= ~(EXTI_RTSR_TR11);

			//Rising trigger enabled for input line.
			EXTI->RTSR |= (EXTI_RTSR_TR11);

			break;
		}

		//Configuramos para el pin 12
		case 12: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->RTSR &= ~(EXTI_RTSR_TR12);

			//Rising trigger enabled for input line.
			EXTI->RTSR |= (EXTI_RTSR_TR12);

			break;
		}

		//Configuramos para el pin 13
		case 13: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->RTSR &= ~(EXTI_RTSR_TR13);

			//Rising trigger enabled for input line.
			EXTI->RTSR |= (EXTI_RTSR_TR13);

			break;
		}

		//Configuramos para el pin 14
		case 14: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->RTSR &= ~(EXTI_RTSR_TR14);

			//Rising trigger enabled for input line.
			EXTI->RTSR |= (EXTI_RTSR_TR14);

			break;
		}

		//Configuramos para el pin 15
		case 15: {

			// Limpiamos primero la posición que deseamos configurar
			EXTI->RTSR &= ~(EXTI_RTSR_TR15);

			//Rising trigger enabled for input line.
			EXTI->RTSR |= (EXTI_RTSR_TR15);

			break;
		}

		default:{
			break;
		}

		}
	}
	//REVISAR!!!
	else if(extiConfig->edgeType == EXTERNAL_INTERRUPT_RISING_AND_FALLING_EDGE){
			/* Rising and falling Trigger selection register*/

			// Select TRx for rising and falling Trigger event configuration bit of line x
			// Se está interpretanto el número del pin x en uso cómo la relación con el "bit of line x"
			switch (extiConfig->pGPIOHandler->pinConfig.GPIO_PinNumber) {

			//Configuramos para el pin 0
			case 0: {

				// Limpiamos primero las posiciones que deseamos configurar
				EXTI->RTSR &= ~(EXTI_RTSR_TR0);
				EXTI->FTSR &= ~(EXTI_FTSR_TR0);

				//Rising and falling trigger enabled for input line.
				EXTI->RTSR |= (EXTI_RTSR_TR0);
				EXTI->FTSR |= (EXTI_FTSR_TR0);

				break;
			}

			//Configuramos para el pin 1
			case 1: {

				// Limpiamos primero las posiciones que deseamos configurar
				EXTI->RTSR &= ~(EXTI_RTSR_TR1);
				EXTI->FTSR &= ~(EXTI_FTSR_TR1);

				//Rising and falling trigger enabled for input line.
				EXTI->RTSR |= (EXTI_RTSR_TR1);
				EXTI->FTSR |= (EXTI_FTSR_TR1);

				break;
			}

			//Configuramos para el pin 2
			case 2: {

				// Limpiamos primero las posiciones que deseamos configurar
				EXTI->RTSR &= ~(EXTI_RTSR_TR2);
				EXTI->FTSR &= ~(EXTI_FTSR_TR2);

				//Rising and falling trigger enabled for input line.
				EXTI->RTSR |= (EXTI_RTSR_TR2);
				EXTI->FTSR |= (EXTI_FTSR_TR2);

				break;
			}

			//Configuramos para el pin 3
			case 3: {

				// Limpiamos primero las posiciones que deseamos configurar
				EXTI->RTSR &= ~(EXTI_RTSR_TR3);
				EXTI->FTSR &= ~(EXTI_FTSR_TR3);

				//Rising and falling trigger enabled for input line.
				EXTI->RTSR |= (EXTI_RTSR_TR3);
				EXTI->FTSR |= (EXTI_FTSR_TR3);

				break;
			}

			//Configuramos para el pin 4
			case 4: {

				// Limpiamos primero las posiciones que deseamos configurar
				EXTI->RTSR &= ~(EXTI_RTSR_TR4);
				EXTI->FTSR &= ~(EXTI_FTSR_TR4);

				//Rising and falling trigger enabled for input line.
				EXTI->RTSR |= (EXTI_RTSR_TR4);
				EXTI->FTSR |= (EXTI_FTSR_TR4);

				break;
			}

			//Configuramos para el pin 5
			case 5: {

				// Limpiamos primero las posiciones que deseamos configurar
				EXTI->RTSR &= ~(EXTI_RTSR_TR5);
				EXTI->FTSR &= ~(EXTI_FTSR_TR5);

				//Rising and falling trigger enabled for input line.
				EXTI->RTSR |= (EXTI_RTSR_TR5);
				EXTI->FTSR |= (EXTI_FTSR_TR5);

				break;
			}

			//Configuramos para el pin 6
			case 6: {

				// Limpiamos primero las posiciones que deseamos configurar
				EXTI->RTSR &= ~(EXTI_RTSR_TR6);
				EXTI->FTSR &= ~(EXTI_FTSR_TR6);

				//Rising and falling trigger enabled for input line.
				EXTI->RTSR |= (EXTI_RTSR_TR6);
				EXTI->FTSR |= (EXTI_FTSR_TR6);

				break;
			}

			//Configuramos para el pin 7
			case 7: {

				// Limpiamos primero las posiciones que deseamos configurar
				EXTI->RTSR &= ~(EXTI_RTSR_TR7);
				EXTI->FTSR &= ~(EXTI_FTSR_TR7);

				//Rising and falling trigger enabled for input line.
				EXTI->RTSR |= (EXTI_RTSR_TR7);
				EXTI->FTSR |= (EXTI_FTSR_TR7);

				break;
			}

			//Configuramos para el pin 8
			case 8: {

				// Limpiamos primero las posiciones que deseamos configurar
				EXTI->RTSR &= ~(EXTI_RTSR_TR8);
				EXTI->FTSR &= ~(EXTI_FTSR_TR8);

				//Rising and falling trigger enabled for input line.
				EXTI->RTSR |= (EXTI_RTSR_TR8);
				EXTI->FTSR |= (EXTI_FTSR_TR8);

				break;
			}

			//Configuramos para el pin 9
			case 9: {

				// Limpiamos primero las posiciones que deseamos configurar
				EXTI->RTSR &= ~(EXTI_RTSR_TR9);
				EXTI->FTSR &= ~(EXTI_FTSR_TR9);

				//Rising and falling trigger enabled for input line.
				EXTI->RTSR |= (EXTI_RTSR_TR9);
				EXTI->FTSR |= (EXTI_FTSR_TR9);

				break;
			}

			//Configuramos para el pin 10
			case 10: {

				// Limpiamos primero las posiciones que deseamos configurar
				EXTI->RTSR &= ~(EXTI_RTSR_TR10);
				EXTI->FTSR &= ~(EXTI_FTSR_TR10);

				//Rising and falling trigger enabled for input line.
				EXTI->RTSR |= (EXTI_RTSR_TR10);
				EXTI->FTSR |= (EXTI_FTSR_TR10);

				break;
			}

			//Configuramos para el pin 11
			case 11: {

				// Limpiamos primero las posiciones que deseamos configurar
				EXTI->RTSR &= ~(EXTI_RTSR_TR11);
				EXTI->FTSR &= ~(EXTI_FTSR_TR11);

				//Rising and falling trigger enabled for input line.
				EXTI->RTSR |= (EXTI_RTSR_TR11);
				EXTI->FTSR |= (EXTI_FTSR_TR11);

				break;
			}

			//Configuramos para el pin 12
			case 12: {

				// Limpiamos primero las posiciones que deseamos configurar
				EXTI->RTSR &= ~(EXTI_RTSR_TR12);
				EXTI->FTSR &= ~(EXTI_FTSR_TR12);

				//Rising and falling trigger enabled for input line.
				EXTI->RTSR |= (EXTI_RTSR_TR12);
				EXTI->FTSR |= (EXTI_FTSR_TR12);

				break;
			}

			//Configuramos para el pin 13
			case 13: {

				// Limpiamos primero las posiciones que deseamos configurar
				EXTI->RTSR &= ~(EXTI_RTSR_TR13);
				EXTI->FTSR &= ~(EXTI_FTSR_TR13);

				//Rising and falling trigger enabled for input line.
				EXTI->RTSR |= (EXTI_RTSR_TR13);
				EXTI->FTSR |= (EXTI_FTSR_TR13);

				break;
			}

			//Configuramos para el pin 14
			case 14: {

				// Limpiamos primero las posiciones que deseamos configurar
				EXTI->RTSR &= ~(EXTI_RTSR_TR14);
				EXTI->FTSR &= ~(EXTI_FTSR_TR14);

				//Rising and falling trigger enabled for input line.
				EXTI->RTSR |= (EXTI_RTSR_TR14);
				EXTI->FTSR |= (EXTI_FTSR_TR14);

				break;
			}

			//Configuramos para el pin 15
			case 15: {

				// Limpiamos primero las posiciones que deseamos configurar
				EXTI->RTSR &= ~(EXTI_RTSR_TR15);
				EXTI->FTSR &= ~(EXTI_FTSR_TR15);

				//Rising and falling trigger enabled for input line.
				EXTI->RTSR |= (EXTI_RTSR_TR15);
				EXTI->FTSR |= (EXTI_FTSR_TR15);

				break;
			}

			default:{
				break;
			}

			}
		}
}

/*
 * Funcion que configura las mascaras de interrupciones (registro de mascaras) y
 * ademas matricula cada una de las posibles interrupciones en el NVIC
 * */
static void exti_config_interrupt(EXTI_Config_t *extiConfig){
	/* 6.0 Activamos la interrupción del canal que estamos configurando */

		// Interrupt Mask register
	    //Activamos la Interrupt mask register para el canal específico en uso
	    switch (extiConfig->pGPIOHandler->pinConfig.GPIO_PinNumber) {

	    //Configuramos para el pin 0
		case 0: {
			//Interrupt mask on input line
			EXTI->IMR |= (EXTI_IMR_MR0);

			break;

		}

		//Configuramos para el pin 1
		case 1: {
			//Interrupt mask on input line
			EXTI->IMR |= (EXTI_IMR_MR1);

			break;
		}

		//Configuramos para el pin 2
		case 2: {
			//Interrupt mask on input line
			EXTI->IMR |= (EXTI_IMR_MR2);

			break;
		}

		//Configuramos para el pin 3
		case 3: {
			//Interrupt mask on input line
			EXTI->IMR |= (EXTI_IMR_MR3);

			break;
		}

		//Configuramos para el pin 4
		case 4: {
			//Interrupt mask on input line
			EXTI->IMR |= (EXTI_IMR_MR4);

			break;
		}

		//Configuramos para el pin 5
		case 5: {
			//Interrupt mask on input line
			EXTI->IMR |= (EXTI_IMR_MR5);

			break;
		}

		//Configuramos para el pin 6
		case 6: {
			//Interrupt mask on input line
			EXTI->IMR |= (EXTI_IMR_MR6);

			break;
		}

		//Configuramos para el pin 7
		case 7: {
			//Interrupt mask on input line
			EXTI->IMR |= (EXTI_IMR_MR7);

			break;
		}

		//Configuramos para el pin 8
		case 8: {
			//Interrupt mask on input line
			EXTI->IMR |= (EXTI_IMR_MR8);

			break;
		}

		//Configuramos para el pin 9
		case 9: {
			//Interrupt mask on input line
			EXTI->IMR |= (EXTI_IMR_MR9);

			break;
		}

		//Configuramos para el pin 10
		case 10: {
			//Interrupt mask on input line
			EXTI->IMR |= (EXTI_IMR_MR10);

			break;
		}

		//Configuramos para el pin 11
		case 11: {
			//Interrupt mask on input line
			EXTI->IMR |= (EXTI_IMR_MR11);

			break;
		}

		//Configuramos para el pin 12
		case 12: {
			//Interrupt mask on input line
			EXTI->IMR |= (EXTI_IMR_MR12);

			break;
		}

		//Configuramos para el pin 13
		case 13: {
			//Interrupt mask on input line
			EXTI->IMR |= (EXTI_IMR_MR13);

			break;
		}

		//Configuramos para el pin 14
		case 14: {
			//Interrupt mask on input line
			EXTI->IMR |= (EXTI_IMR_MR14);

			break;
		}

		//Configuramos para el pin 15
		case 15: {
			//Interrupt mask on input line
			EXTI->IMR |= (EXTI_IMR_MR15);

			break;
		}
		default:{
			break;
		}

		}

		/* 6.1 Matriculamos la interrupción en el NVIC para el canal correspondiente,
		 * donde el canal 0 corresponde al EXTI_0, canal 1 al EXTI_1, etc.
		 *
		 * NOTA: Observar que algunos canales EXTI comparten un mismo vector de interrupción
		 * */
		switch (extiConfig->pGPIOHandler->pinConfig.GPIO_PinNumber) {
		case 0: {
			__NVIC_EnableIRQ(EXTI0_IRQn);
			break;
		}

		case 1: {
			__NVIC_EnableIRQ(EXTI1_IRQn);
			break;
		}
		case 2: {
			__NVIC_EnableIRQ(EXTI2_IRQn);
			break;
		}
		case 3: {
			__NVIC_EnableIRQ(EXTI3_IRQn);
			break;
		}
		case 4: {
			__NVIC_EnableIRQ(EXTI4_IRQn);
			break;
		}
		case 5: {
			__NVIC_EnableIRQ(EXTI9_5_IRQn);
			break;
		}
		case 6: {
			__NVIC_EnableIRQ(EXTI9_5_IRQn);
			break;
		}
		case 7: {
			__NVIC_EnableIRQ(EXTI9_5_IRQn);
			break;
		}
		case 8: {
			__NVIC_EnableIRQ(EXTI9_5_IRQn);
			break;
		}
		case 9: {
			__NVIC_EnableIRQ(EXTI9_5_IRQn);
			break;
		}
		case 10: {
			__NVIC_EnableIRQ(EXTI15_10_IRQn);
			break;
		}
		case 11: {
			__NVIC_EnableIRQ(EXTI15_10_IRQn);
			break;
		}
		case 12: {
			__NVIC_EnableIRQ(EXTI15_10_IRQn);
			break;
		}
		case 13: {
			__NVIC_EnableIRQ(EXTI15_10_IRQn);
			break;
		}
		case 14: {
			__NVIC_EnableIRQ(EXTI15_10_IRQn);
			break;
		}
		case 15: {
			__NVIC_EnableIRQ(EXTI15_10_IRQn);
			break;
		}
		default: {
			break;
		}
	}
}

/**/
__attribute__ ((weak)) void callback_ExtInt0(void){
	__NOP();
}
/**/
__attribute__ ((weak)) void callback_ExtInt1(void){
	__NOP();
}
/**/
__attribute__ ((weak)) void callback_ExtInt2(void){
	__NOP();
}
/**/
__attribute__ ((weak)) void callback_ExtInt3(void){
	__NOP();
}
/**/
__attribute__ ((weak)) void callback_ExtInt4(void){
	__NOP();
}
/**/
__attribute__ ((weak)) void callback_ExtInt5(void){
	__NOP();
}
/**/
__attribute__ ((weak)) void callback_ExtInt6(void){
	__NOP();
}
/**/
__attribute__ ((weak)) void callback_ExtInt7(void){
	__NOP();
}
/**/
__attribute__ ((weak)) void callback_ExtInt8(void){
	__NOP();
}
/**/
__attribute__ ((weak)) void callback_ExtInt9(void){
	__NOP();
}
/**/
__attribute__ ((weak)) void callback_ExtInt10(void){
	__NOP();
}
/**/
__attribute__ ((weak)) void callback_ExtInt11(void){
	__NOP();
}
/**/
__attribute__ ((weak)) void callback_ExtInt12(void){
	__NOP();
}
/**/
__attribute__ ((weak)) void callback_ExtInt13(void){
	__NOP();
}
/**/
__attribute__ ((weak)) void callback_ExtInt14(void){
	__NOP();
}
/**/
__attribute__ ((weak)) void callback_ExtInt15(void){
	__NOP();
}


/* ISR de la interrupción canal 0*/
void EXTI0_IRQHandler(void){
	// Evaluamos si la interrupción que se lanzo corresponde al PIN_0 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR0){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR0;

		// llamamos al callback
		callback_ExtInt0();
	}
}

/* ISR de la interrupción canal 1*/
void EXTI1_IRQHandler(void){
	// Evaluamos si la interrupción que se lanzo corresponde al PIN_1 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR1){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR1;

		// llamamos al callback
		callback_ExtInt1();
	}
}

/* ISR de la interrupción canal 2*/
void EXTI2_IRQHandler(void){
	// Evaluamos si la interrupción que se lanzo corresponde al PIN_2 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR2){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR2;

		// llamamos al callback
		callback_ExtInt2();
	}
}

/* ISR de la interrupción canal 3*/
void EXTI3_IRQHandler(void){
	// Evaluamos si la interrupción que se lanzo corresponde al PIN_3 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR3){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR3;

		// llamamos al callback
		callback_ExtInt3();
	}
}

/* ISR de la interrupción canal 4*/
void EXTI4_IRQHandler(void){
	// Evaluamos si la interrupción que se lanzo corresponde al PIN_4 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR4){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR4;

		// llamamos al callback
		callback_ExtInt4();
	}
}

/* ISR de la interrupción canales 9_5
 * Observe que debe agregar totos los posibles casos, los cuales
 * son identificados por un bloque if() y el analisis de la bandera
 * (pending register -> EXTI_PR)
 */
void EXTI9_5_IRQHandler(void){
	// Evaluamos si la interrupción que se lanzo corresponde al PIN_5 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR5){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR5;

		// llamamos al callback
		callback_ExtInt5();
	}

	// Evaluamos si la interrupción que se lanzo corresponde al PIN_6 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR6){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR6;

		// llamamos al callback
		callback_ExtInt6();
	}

	// Evaluamos si la interrupción que se lanzo corresponde al PIN_7 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR7){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR7;

		// llamamos al callback
		callback_ExtInt7();
	}

	// Evaluamos si la interrupción que se lanzo corresponde al PIN_8 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR8){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR8;

		// llamamos al callback
		callback_ExtInt8();
	}

	// Evaluamos si la interrupción que se lanzo corresponde al PIN_9 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR9){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR9;

		// llamamos al callback
		callback_ExtInt9();
	}

}

/* ISR de la interrupción canales 15_10
 * Observe que debe agregar totos los posibles casos, los cuales
 * son identificados por un bloque if() y el analisis de la bandera
 * (pending register -> EXTI_PR)
 */
void EXTI15_10_IRQHandler(void){
	// Evaluamos si la interrupción que se lanzo corresponde al PIN_10 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR10){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR10;

		// llamamos al callback
		callback_ExtInt10();
	}

	// Evaluamos si la interrupción que se lanzo corresponde al PIN_11 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR11){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR11;

		// llamamos al callback
		callback_ExtInt11();
	}

	// Evaluamos si la interrupción que se lanzo corresponde al PIN_12 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR12){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR12;

		// llamamos al callback
		callback_ExtInt12();
	}

	// Evaluamos si la interrupción que se lanzo corresponde al PIN_13 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR13){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR13;

		// llamamos al callback
		callback_ExtInt13();
	}

	// Evaluamos si la interrupción que se lanzo corresponde al PIN_14 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR14){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR14;

		// llamamos al callback
		callback_ExtInt14();
	}

	// Evaluamos si la interrupción que se lanzo corresponde al PIN_15 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR15){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR15;

		// llamamos al callback
		callback_ExtInt15();
	}

}
