/**
 ******************************************************************************
 * @file           : main.c
 * @author         : laurasofia
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "stm32_assert.h"
#include "gpio_driver_hal.h"

//Headers definition
int add(int x, int y);

//Definimos Pines a utilizar
GPIO_Handler_t userLed    = {0}; //PinA5
GPIO_Handler_t userButton = {0}; //PinC13
GPIO_Handler_t userLed_00 = {0}; //PinA0
GPIO_Handler_t userLed_01 = {0}; //PinA6
GPIO_Handler_t userLed_02 = {0}; //PinB9
GPIO_Handler_t userLed_03 = {0}; //PinC6
GPIO_Handler_t userLed_04 = {0}; //PinC7
GPIO_Handler_t userLed_05 = {0}; //PinB6
GPIO_Handler_t userLed_06 = {0}; //PinA9

/*
 * The main function, where everything happens
 */
int main(void)
{
    /* Configuramos el pin */
	//NOTA: p en pGPIO indica un counter
	userLed.pGPIOx                        = GPIOA; //Determinando el puerto a utilizar
	userLed.pinConfig.GPIO_PinNumber      = PIN_5;
	userLed.pinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	userLed.pinConfig.GPIO_PinOutputType  = GPIO_OTYPE_PUSHPULL;
	userLed.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
	userLed.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	/* Cargamos la configuracion en los registros que gobiernan el puerto */
	gpio_Config(&userLed);
	//gpio_WritePin(&userLed,SET);

	/* ++++====== PRIMER PUNTO ======++++ */

	/* a) Errores encontrados en planteamiento inicial:
	 *
	 * 1. Al momento de asignar a la variable pinValue el valor correspondiente al registro
	 *  IDR desplazado un "pinNumber" de veces hacia la izquierda (usando la operacción
	 *  shift "<<") no se obtiene un resultado que permita identificar si la posición
	 *  correspondiente al pin de análisis está o no activa (pues el resultado obtenido
	 *  corresponde a un  overflow"). La anterior conclusión se
	 *  argumenta también con el hecho de que el valor que retorna la función es exactamente
	 *  el resultado del registro IDR modificado de la forma antes mencionada (no se
	 *  procede a interpretar este resultado para encontrar algún camino que garantice la
	 *  respuesta solicitada), lo cual NO brinda la solución específica que se plantea
	 *  debe cumplir la función.
	 *
	 *  2. La asignación "pinValue = pinValue" al final de la función es innecesaria en
	 *  la estructura de la misma, pues estamos asignando un valor ya guardado en la
	 *  variable anteriormente. Al ser esta línea de código redundante se opta por retirarla
	 *  con el fin de contribuir a la construcción de un código más óptimo.
	 */

	/* b) Solución de los errores identificados:
	 *
	 * Lo primero es realizar una operación correcta en el análisis del registro IDR como
	 * herramienta para la determinación del estado del pin en estudio. Para cumplir con
	 * este propósito se sigue la siguiente metodología:
	 *
	 *- Asignar a la variable pinValue el valor del registro IDR.
	 *
	 *- Usar el operador shift ">>" sobre pinValue con un desplazamiento correspondiente
	 *a pinNumber de veces, pues de esta manera podremos conocer si en la posición del pin
	 *número "pinNumber" el IDR guarda el valor de 1 (pin activado) o 0 (pin desactivado).
	 *
	 *- Para que la variable que retorna la función nos informe efectivamente el estado
	 *del pin, se propone un condicional que evalue si el número asociado a
	 *IDR << pinNumber es par (según estructuración de binarios implica que el valor
	 *asignado al bit CERO es 0) o impar (según estructuración de binarios implica que el
	 *valor asignado al bit CERO es 1). En caso de ser el resultado par se establece
	 *pinValue = 0, por el contrario, en caso de ser impar se establece pinValue = 1.
	 *
	 *- La función retorna el valor de pinValue permitiendo interpretar dos posibilidades:
	 * pinValue = 0 -> Pin sujeto a análisis estÁ DESACTIVADO
	 * PinValue = 1 -> Pin sujeto a análisis estÁ ACTIVADO
	 */

	//Llamamos a la función que se desea analizar para corroborar funcionamiento adecuado
	//de la corrección del código.
	gpio_ReadPin(&userLed);

	/* ++++====== FIN PRIMER PUNTO ======++++ */

	/* ++++====== SEGUNDO PUNTO ======++++ */

	/*
	 * Detalle sobre funcionamiento de la función se encuentra en documento gpio_driver_hal.c
	 */

	//Llamamos a la función que se desea analizar
	gpio_TooglePin(&userLed);

	/* ++++====== FIN SEGUNDO PUNTO ======++++ */

	/* ++++====== TERCER PUNTO ======++++ */

	//En primera instancia se define la configuración de los pines a usar:

	/* PinC13 -> User Button */
	userButton.pGPIOx                        = GPIOC; //Determinando el puerto a utilizar
	userButton.pinConfig.GPIO_PinNumber      = PIN_13;
	userButton.pinConfig.GPIO_PinMode        = GPIO_MODE_IN;
	/* Cargamos la configuracion en los registros que gobiernan el puerto */
	gpio_Config(&userButton);

	/* PinA0 -> Dir */
	userLed_00.pGPIOx                        = GPIOA; //Determinando el puerto a utilizar
	userLed_00.pinConfig.GPIO_PinNumber      = PIN_0;
	userLed_00.pinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	userLed_00.pinConfig.GPIO_PinOutputType  = GPIO_OTYPE_PUSHPULL;
	userLed_00.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
	userLed_00.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	/* Cargamos la configuracion en los registros que gobiernan el puerto */
	gpio_Config(&userLed_00);

	/* PinA6 -> bit 5 */
	// Especificamos el bit que va a representar este pin
	uint8_t bit_5 = 5;
	// Definimos la configuración
	userLed_01.pGPIOx                        = GPIOA; //Determinando el puerto a utilizar
	userLed_01.pinConfig.GPIO_PinNumber      = PIN_6;
	userLed_01.pinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	userLed_01.pinConfig.GPIO_PinOutputType  = GPIO_OTYPE_PUSHPULL;
	userLed_01.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
	userLed_01.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	/* Cargamos la configuracion en los registros que gobiernan el puerto */
	gpio_Config(&userLed_01);

	/* PinB9 -> bit 4 */
	// Especificamos el bit que va a representar este pin
	uint8_t bit_4 = 4;
	// Definimos la configuración
	userLed_02.pGPIOx                        = GPIOB; //Determinando el puerto a utilizar
	userLed_02.pinConfig.GPIO_PinNumber      = PIN_9;
	userLed_02.pinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	userLed_02.pinConfig.GPIO_PinOutputType  = GPIO_OTYPE_PUSHPULL;
	userLed_02.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
	userLed_02.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	/* Cargamos la configuracion en los registros que gobiernan el puerto */
	gpio_Config(&userLed_02);

	/* PinC6 -> bit 3 */
	// Especificamos el bit que va a representar este pin
	uint8_t bit_3 = 3;
	// Definimos la configuración
	userLed_03.pGPIOx                        = GPIOC; //Determinando el puerto a utilizar
	userLed_03.pinConfig.GPIO_PinNumber      = PIN_6;
	userLed_03.pinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	userLed_03.pinConfig.GPIO_PinOutputType  = GPIO_OTYPE_PUSHPULL;
	userLed_03.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
	userLed_03.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	/* Cargamos la configuracion en los registros que gobiernan el puerto */
	gpio_Config(&userLed_03);

	/* PinC7 -> bit 2 */
	// Especificamos el bit que va a representar este pin
	uint8_t bit_2 = 2;
	// Definimos la configuración
	userLed_04.pGPIOx                        = GPIOC; //Determinando el puerto a utilizar
	userLed_04.pinConfig.GPIO_PinNumber      = PIN_7;
	userLed_04.pinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	userLed_04.pinConfig.GPIO_PinOutputType  = GPIO_OTYPE_PUSHPULL;
	userLed_04.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
	userLed_04.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	/* Cargamos la configuracion en los registros que gobiernan el puerto */
	gpio_Config(&userLed_04);

	/* PinB6 -> bit 1 */
	// Especificamos el bit que va a representar este pin
	uint8_t bit_1 = 1;
	// Definimos la configuración
	userLed_05.pGPIOx                        = GPIOB; //Determinando el puerto a utilizar
	userLed_05.pinConfig.GPIO_PinNumber      = PIN_6;
	userLed_05.pinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	userLed_05.pinConfig.GPIO_PinOutputType  = GPIO_OTYPE_PUSHPULL;
	userLed_05.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
	userLed_05.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	/* Cargamos la configuracion en los registros que gobiernan el puerto */
	gpio_Config(&userLed_05);

	/* PinA9 -> bit 0 */
	// Especificamos el bit que va a representar este pin
	uint8_t bit_0 = 0;
	// Definimos la configuración
	userLed_06.pGPIOx                        = GPIOA; //Determinando el puerto a utilizar
	userLed_06.pinConfig.GPIO_PinNumber      = PIN_9;
	userLed_06.pinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	userLed_06.pinConfig.GPIO_PinOutputType  = GPIO_OTYPE_PUSHPULL;
	userLed_06.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
	userLed_06.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	/* Cargamos la configuracion en los registros que gobiernan el puerto */
	gpio_Config(&userLed_06);

	//A continuación se está probando la correcta funcionalidad del pin A9
	//gpio_WritePin(&userLed_06,SET);

	// Procedemos a analizar el estado del User Button usando la función gpio_ReadPin
	/*
	 * userButton = 1 -> contador debe incrementar
	 * userButton = 0 -> contador debe decrementar
	 */
	//Definimos variable para almacenar el resultado que brinde gpio_ReadPin
	uint8_t pinUserButton = 0;

	// Definimos variable para activar contador
	uint8_t counter_i = 0;
	uint8_t counter_k = 0;

	/* Loop forever */
	while(1){

		// Definimos ciclo para iniciar contador (creciente) en representación de Leds como binarios
		for(counter_i > 0; counter_i < 61; counter_i ++){
			// Leemos el estado del User Button
			pinUserButton = gpio_ReadPin(&userButton);

			//Evaluamos si es necesario cambiar de contador
			if(pinUserButton == 0){

				//Asignamos al contador decreciente el valor que se lleva actualmente
				//para iniciar en la cuenta
				counter_k = counter_i;
				//Encendemos led Dir para indicar cuenta hacia atrás
				gpio_WritePin(&userLed_00,SET);
				break;
			}

			//Procedemos a usar función encargada de determinar el estado de cada bit
			//individual asociado al número específico que lleva el contador
			gpio_LedBinario(&userLed_06, counter_i, bit_0);
			gpio_LedBinario(&userLed_05, counter_i, bit_1);
			gpio_LedBinario(&userLed_04, counter_i, bit_2);
			gpio_LedBinario(&userLed_03, counter_i, bit_3);
			gpio_LedBinario(&userLed_02, counter_i, bit_4);
			gpio_LedBinario(&userLed_01, counter_i, bit_5);

			//Configuramos un ciclo for para crear delay de aprox un segundo

			//Definimos variable para activar contador del ciclo para el delay
			uint32_t counter_j = 0;

			//Valor límite superior del counter es establecido teniendo en cuenta que la velocidad
			//de operación del MCU es de aprox 16MHz
			for(counter_j = 0; counter_j < 1000000; counter_j ++){}

			//Para garantizar que el contador permanezca activo se realiza la siguiente redefinición
			//de variables
			if(counter_i == 60){
				//Al asignar nuevamente el valor inicial al contador se reinicia el ciclo
				counter_i = 0;
			}

		}

		// Definimos ciclo para iniciar contador (decreciente) en representación de Leds como binarios
		for(counter_k < 61; counter_k > 0; counter_k --){

			// Leemos el estado del User Button
			pinUserButton = gpio_ReadPin(&userButton);

			//Evaluamos si es necesario cambiar de contador
			if(pinUserButton == 1){

				//Asignamos al contador decreciente el valor que se lleva actualmente
				//para iniciar en la cuenta
				counter_i = counter_k;
				//Apagamos led Dir para indicar desactivada la cuenta hacia atrás
				gpio_WritePin(&userLed_00,RESET);
				break;
			}

			//Procedemos a usar función encargada de determinar el estado de cada bit
			//individual asociado al número específico que lleva el contador
			gpio_LedBinario(&userLed_06, counter_k, bit_0);
			gpio_LedBinario(&userLed_05, counter_k, bit_1);
			gpio_LedBinario(&userLed_04, counter_k, bit_2);
			gpio_LedBinario(&userLed_03, counter_k, bit_3);
			gpio_LedBinario(&userLed_02, counter_k, bit_4);
			gpio_LedBinario(&userLed_01, counter_k, bit_5);

			//Configuramos un ciclo for para crear delay de aprox un segundo

			//Definimos variable para activar contador del ciclo para el delay
			uint32_t counter_m = 0;

			//Valor límite superior del counter es establecido teniendo en cuenta que la velocidad
			//de operación del MCU es de aprox 16MHz
			for(counter_m = 0; counter_m < 1000000; counter_m ++){}

			//Para garantizar que el contador permanezca activo se realiza la siguiente redefinición
			//de variables
			if(counter_k == 1){
				//Al asignar nuevamente el valor inicial al contador se reinicia el ciclo
				counter_k = 61;
			}

		}
	}

	/* ++++====== FIN TERCER PUNTO ======++++ */
}

/*
 * Esta funcion sirve para detectar problemas de parametros
 * incorrectos al momento de ejecutar el programa
 */
void assert_failed(uint8_t* file, uint32_t line){
	while(1){
		//PROBLEMS...
	}
}
