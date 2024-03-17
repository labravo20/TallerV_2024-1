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

