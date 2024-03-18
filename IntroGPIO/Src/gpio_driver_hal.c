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






