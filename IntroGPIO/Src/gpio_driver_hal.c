/*
 * gpio_driver_hal.c
 *
 *  Created on: 17/03/2024
 *      Author: laurasofia
 */

#include "gpio_driver_hal.h"
#include "stm32f4xx_hal.h"
#include "stm32_assert.h"

/* === Headers for private functions */
void gpio_enable_clock_peripherical (GPIO_Handler_t *pGPIOHandler);
void gpio_config_mode (GPIO_Handler_t *pGPIOHandler);
void gpio_config_output_type (GPIO_Handler_t *pGPIOHandler);
void gpio_config_output_speed (GPIO_Handler_t *pGPIOHandler);
void gpio_config_pullup_pulldown (GPIO_Handler_t *pGPIOHandler);
void gpio_config_alternate_function (GPIO_Handler_t *pGPIOHandler);

/*
 * Para cualquier periferico, hay varios pasos que siempre se deben seguir en un
 * orden estricto para poder que el sistema perimita configurar el periferico X.
 * Lo primero y mas importante es activar la señal de reloj principal hacia ese
 * elemento especifico (relacionado con el periferico RCC), a esto llamaremos
 * simplemente "activar el periferico o activar la señal de reloj del periferico"
 */

void gpio_Config (GPIO_Handler_t *pGPIOHandler){

	/* Verificamos que el pin seleccionado es correcto */
	assert_param(IS_GPIO_PIN(pGPIOHandler -> pinConfig.GPIO_PinNumber));

	// 1) Activar el periferico
	gpio_enable_clock_peripheral(pGPIOHandler);

	//Despues de activado, podemos comenzar a configurar.

	// 2) Configurando el registro GPIOx_MODER
	gpio_config_mode(pGPIOHandler);

	// 3) Configurando el registro GPIOx_OTYPER
	gpio_config_output_type(pGPIOHandler);

	// 4) Configurando la velocidad
	gpio_config_output_speed(pGPIOHandler);

	// 5) Configurando si se desea pull-up, pull-down o flotante
	gpio_config_pullup_pulldown(pGPIOHandler);

	// 6) Configurando funciones alternativas... se verá luego, mas adelante en el curso
	gpio_config_alternate_function(pGPIOHandler);

}//Fin del GPIO_config

/*
 * Enable clock signal for specific GPIOx port
 */
void gpio_enable_clock_peripheral(GPIO_Handler_t *pGPIOHandler){

	//Verificamos que el puerto configurado si es permitido
	assert_param(IS_GPIO_ALL_INSTANCE(pGPIOHandler -> pGPIOx));

	//Verificamos para GPIOA
	if(pGPIOHandler ->pGPIOx == GPIOA){
		//Escribimos 1 (SET) en la posicion correspondiente al GPIOA
		RCC ->AHB1ENR |= (1 << RCC_AHB1ENR_GPIOAEN);
	}
	//Verificamos para GPIOB
	else if(pGPIOHandler ->pGPIOx == GPIOB){
		//Escribimos 1 (SET) en la posicion correspondiente al GPIOB
		RCC ->AHB1ENR |= (1 << RCC_AHB1ENR_GPIOBEN);
	}
	//Verificamos para GPIOC
	else if(pGPIOHandler ->pGPIOx == GPIOC){
		//Escribimos 1 (SET) en la posicion correspondiente al GPIOC
		RCC ->AHB1ENR |= (1 << RCC_AHB1ENR_GPIOCEN);
	}
	//Verificamos para GPIOD
	else if(pGPIOHandler ->pGPIOx == GPIOD){
		//Escribimos 1 (SET) en la posicion correspondiente al GPIOD
		RCC ->AHB1ENR |= (1 << RCC_AHB1ENR_GPIODEN);
	}
	//Verificamos para GPIOE
	else if(pGPIOHandler ->pGPIOx == GPIOE){
		//Escribimos 1 (SET) en la posicion correspondiente al GPIOE
		RCC ->AHB1ENR |= (1 << RCC_AHB1ENR_GPIOEEN);
	}
	//Verificamos para GPIOH
	else if(pGPIOHandler ->pGPIOx == GPIOH){
		//Escribimos 1 (SET) en la posicion correspondiente al GPIOH
		RCC ->AHB1ENR |= (1 << RCC_AHB1ENR_GPIOHEN);
	}
}






