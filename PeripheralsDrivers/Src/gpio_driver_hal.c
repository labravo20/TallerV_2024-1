/*
 * gpio_driver_hal.c
 *
 *  Created on: Mar 18, 2024
 *      Author: laurasofia
 */

#include "gpio_driver_hal.h"
#include "stm32f4xx.h"
#include "stm32_assert.h"

/* === Headers for private functions (Porque su header esta en inicio archivo .c) */
// Adicionalmente las privadas son continuadas despues del _ con minuscula
static void gpio_enable_clock_peripheral (GPIO_Handler_t *pGPIOHandler);
static void gpio_config_mode (GPIO_Handler_t *pGPIOHandler);
static void gpio_config_output_type (GPIO_Handler_t *pGPIOHandler);
static void gpio_config_output_speed (GPIO_Handler_t *pGPIOHandler);
static void gpio_config_pullup_pulldown (GPIO_Handler_t *pGPIOHandler);
static void gpio_config_alternate_function (GPIO_Handler_t *pGPIOHandler);

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
		RCC ->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN);
	}
	//Verificamos para GPIOB
	else if(pGPIOHandler ->pGPIOx == GPIOB){
		//Escribimos 1 (SET) en la posicion correspondiente al GPIOB
		RCC ->AHB1ENR |= (RCC_AHB1ENR_GPIOBEN);
	}
	//Verificamos para GPIOC
	else if(pGPIOHandler ->pGPIOx == GPIOC){
		//Escribimos 1 (SET) en la posicion correspondiente al GPIOC
		RCC ->AHB1ENR |= (RCC_AHB1ENR_GPIOCEN);
	}
	//Verificamos para GPIOD
	else if(pGPIOHandler ->pGPIOx == GPIOD){
		//Escribimos 1 (SET) en la posicion correspondiente al GPIOD
		RCC ->AHB1ENR |= (RCC_AHB1ENR_GPIODEN);
	}
	//Verificamos para GPIOE
	else if(pGPIOHandler ->pGPIOx == GPIOE){
		//Escribimos 1 (SET) en la posicion correspondiente al GPIOE
		RCC ->AHB1ENR |= (RCC_AHB1ENR_GPIOEEN);
	}
	//Verificamos para GPIOH
	else if(pGPIOHandler ->pGPIOx == GPIOH){
		//Escribimos 1 (SET) en la posicion correspondiente al GPIOH
		RCC ->AHB1ENR |= (RCC_AHB1ENR_GPIOHEN);
	}
}
/*
 * Configures the mode in which the pin will work:
 * -Input
 * Output
 * -Analog
 * -Alternate function
 */
void gpio_config_mode(GPIO_Handler_t *pGPIOHandler){

	uint32_t auxConfig = 0;

	/* Verificamos si el modo que se ha seleccionado es permitido */
	assert_param(IS_GPIO_MODE(pGPIOHandler -> pinConfig.GPIO_PinMode));

	//Aqui estamos leyendo la config. moviendo "PinNumber" veces hacia la izquierda ese valor (shift left)
	//y eso lo cargamos en la variable auxConfig
	auxConfig = (pGPIOHandler ->pinConfig.GPIO_PinMode <<2 * pGPIOHandler ->pinConfig.GPIO_PinNumber);

	//Antes de cargar el nuevo valor, limpiamos los bits especificos de ese registro (debemos escribir 0b00)
	//para lo cual aplicamos una mascara y una operacion bitwise AND
	pGPIOHandler -> pGPIOx -> MODER &= ~(0b11 <<2 * pGPIOHandler ->pinConfig.GPIO_PinNumber);

	//Cargamos auxConfig en el registro MODER
	pGPIOHandler -> pGPIOx -> MODER |= auxConfig;
}
/*
 * Configures which type of output the PinX will use:
 * -Push-pull
 * -openDrain
 */
void gpio_config_output_type(GPIO_Handler_t *pGPIOHandler){

	uint32_t auxConfig = 0;

	/* Verificamos que el tipo de salida corresponda a los que se pueden utilizar */
	assert_param(IS_GPIO_OUTPUT_TYPE(pGPIOHandler ->pinConfig.GPIO_PinOutputType));

	//De nuevo leemos y movemos el valor un numero "PinNumber" de veces
	auxConfig = (pGPIOHandler ->pinConfig.GPIO_PinOutputType << pGPIOHandler ->pinConfig.GPIO_PinNumber);

	//Limpiamos antes de cargar
	pGPIOHandler -> pGPIOx -> OTYPER &= ~(SET << pGPIOHandler ->pinConfig.GPIO_PinNumber);

	//Cargamos el resultado sobre el registro adecuado
	pGPIOHandler -> pGPIOx -> OTYPER |= auxConfig;
}
/*
 * Selcts between four different possible speeds for output PinX
 * - Low
 * - Medium
 * - Fast
 * - HighSpeed
 */
void gpio_config_output_speed(GPIO_Handler_t *pGPIOHandler){

	uint32_t auxConfig = 0;

	/**/
	assert_param(IS_GPIO_OSPEED(pGPIOHandler ->pinConfig.GPIO_PinOutputSpeed));

	auxConfig = (pGPIOHandler ->pinConfig.GPIO_PinOutputSpeed << 2 * pGPIOHandler ->pinConfig.GPIO_PinNumber);

	//Limpiando la posicion antes de cargar la nueva configuracion
	pGPIOHandler -> pGPIOx -> OSPEEDR &= ~(0b11 << 2 * pGPIOHandler ->pinConfig.GPIO_PinNumber);

	//Cargamos el resultado sobre el registro adecuado
	pGPIOHandler -> pGPIOx -> OSPEEDR |= auxConfig;
}
/*
 * Turns ON/OFF the pull-up and pull-down resistor for each PinX in selected GPIO port
 */
void gpio_config_pullup_pulldown(GPIO_Handler_t *pGPIOHandler){

	uint32_t auxConfig = 0;

	/* Verificamos si la configuracion cargada para las resistencias es correcta */
	assert_param(IS_GPIO_PUPDR(pGPIOHandler ->pinConfig.GPIO_PinPuPdControl));

	auxConfig = (pGPIOHandler ->pinConfig.GPIO_PinPuPdControl << 2 * pGPIOHandler ->pinConfig.GPIO_PinNumber);

	//Limpiando la posicion antes de cargar la nueva configuracion
	pGPIOHandler -> pGPIOx -> PUPDR &= ~(0b11 << 2 * pGPIOHandler ->pinConfig.GPIO_PinNumber);

	//Cargamos el resultado sobre el registro adecuado
	pGPIOHandler -> pGPIOx -> PUPDR |= auxConfig;
}
/*
 * Allows to configure other functions (more specialized) on the selected PinX
 */
void gpio_config_alternate_function(GPIO_Handler_t *pGPIOHandler){

	uint32_t auxPosition = 0;

	if(pGPIOHandler ->pinConfig.GPIO_PinMode == GPIO_MODE_ALTFN){

		//Seleccionamos primero si se debe utilizar el registro bajo (AFRL) o el alto (AFRH)
		if(pGPIOHandler ->pinConfig.GPIO_PinNumber < 8){
				//Estamos en el registro AFRL, que controla los pines del PIN_0 al PIN_7
				auxPosition = 4*pGPIOHandler ->pinConfig.GPIO_PinNumber;

				//Limpiamos primero la posicion del registro que deseamos escribir a continuacion
				pGPIOHandler -> pGPIOx -> AFR[0] &= ~(0b1111 << auxPosition);

				//Escribimos el valor configurado en la posicion seleccionada
				pGPIOHandler -> pGPIOx -> AFR[0] |= (pGPIOHandler ->pinConfig.GPIO_PinAltFunMode << auxPosition);
		}
		else{
				//Estamos en el registro AFRH, que controla los pines del PIN_8 al PIN_15
				auxPosition = 4* (pGPIOHandler ->pinConfig.GPIO_PinNumber -8);

				//Limpiamos primero la posicion del registro que deseamos escribir a continuacion
				pGPIOHandler -> pGPIOx -> AFR[1] &= ~(0b1111 << auxPosition);

				//Escribimos el valor configurado en la posicion seleccionada
				pGPIOHandler -> pGPIOx -> AFR[1] |= (pGPIOHandler ->pinConfig.GPIO_PinAltFunMode << auxPosition);
		}
	}
}
/*
 * Funcion utilizada para cambiar de estado el pin entrregado en el handler, asignando
 * el valor entregado en la variable newState
 */
void gpio_WritePin(GPIO_Handler_t *pPinHandler, uint8_t newState){

	/* Verificamos si la accion que deseamos realizar es permitida */
	assert_param(IS_GPIO_PIN_ACTION(newState));

	//Limpiamos la posicion que deseamos
	//pPinHandler ->pGPIOx->ODR &= ~(SET << pPinHandler -> pinConfig.GPIO_PinNumber);
	if(newState == SET){
		//Trabajando con la parte baja del registro
		pPinHandler -> pGPIOx -> BSRR |= (SET << pPinHandler -> pinConfig.GPIO_PinNumber);
	}
	else{
		//Trabajando con la parte alta del registro
		pPinHandler -> pGPIOx -> BSRR |= (SET << (pPinHandler -> pinConfig.GPIO_PinNumber + 16));
	}
}
/*
 * Función para leer el estado de un pin específico
 */
uint32_t gpio_ReadPin(GPIO_Handler_t *pPinHandler){

	//Creamos una variable auxiliar la cual luego retornaremos
	uint32_t pinValue = 0;

	/* ===== Corrección #1 ===== */
	/* Operación desarrollada a continuación brinda valor resultante incorrecto */

	//Cargamos el valor del registro IDR, desplazado a derecha tantas veces como la ubicacion del pin especifico
	//pinValue = (pPinHandler -> pGPIOx -> IDR << pPinHandler -> pinConfig.GPIO_PinNumber);

	/* Propuesta de solución */

	//Definimos a pinValue en terminos de IDR
	pinValue = pPinHandler -> pGPIOx -> IDR;

	// Para determinar si la posición específica del pin que está activa aplicamos un
	// shift hacia la derecha en cantidad de desplazamiento "pinNumber" de veces:
	pinValue = (pinValue >> pPinHandler -> pinConfig.GPIO_PinNumber);

	//Evaluamos según el resultado del shift si el pin específico de análisis está activo

	//Evaluamos si número asignado a pinValue es par o impar
	if((pinValue%2) == 0){

		//Si el valor de pinValue es par implica pin está desactivado
		pinValue = 0;
	}
	else{

		//Si el valor de pinValue es par implica pin está activado
		pinValue = 1;
	}

	/* ===== Corrección #2 ===== */
	/*Es redundante la asignación de la variable sobre si misma*/
	//pinValue = pinValue;

	//Retornamos el valor de pinValue resultante después del análisis anterior
	// 1 -> pin activado
	// 0 -> pin desactivado
	return pinValue;
}

/* ++++++====== SEGUNDO PUNTO ======++++++ */

//Creación de función gpio_TooglePin
/*
 * Cambia el estado de un PinX (encendido -> apagado o apagado -> encendido)
 */
void gpio_TooglePin(GPIO_Handler_t *pPinHandler){

	// Definimos variable para cargar en ella el resultado de pinValue obtenido al evaluar
	// gpio_ReadPin(pPinHandler)
	uint8_t pinToogle = 0;

	// Llamamos a la función gpio_ReadPin para determinar si el pin está inicialmente
	// encendido o apagado.
	pinToogle = gpio_ReadPin(pPinHandler);

	// Hacemos uso de la función XOR para cambiar el estado del pin:
	//Esta función propone que al comparar dos valores se obtendrá 1 en caso de ser estos
	//diferentes, o 0 en caso de ser iguales.
	pinToogle ^= 1;

	//Llamamos función gpio_WritePin para aplicar el cambio en el estado del pin.
	//El valor asignado en pinToogle será el encargado de configurar el encendido
	//(en caso de ser 1) o apagado (en caso de ser 0) en el estado del Pin.
	gpio_WritePin(pPinHandler,pinToogle);

}

/* ++++++====== FIN SEGUNDO PUNTO ======++++++ */

/*
 * Función para determinar configuración de Leds como binarios en cuenta de 1 a 60
 */
void gpio_LedBinario(GPIO_Handler_t *pPinHandler, uint8_t counter, uint8_t bitNumber){

	// Definimos variable para cargar resultado de pin activo o no activo dependiendo
	// del número específico en análisis
	uint8_t pinLed    = 0;

	// Haciendo uso de la función shift ">>" en un desplazamiento total de "bitNumber"
	// puede llevarse al bit 0 el valor asignado al bit "pinNumber" originalmente.
	pinLed= (counter >> bitNumber);

	//Evaluamos según el resultado del shift si el bit específico de análisis debe estar
	//o no activo para poder representar el número.

	//Evaluamos si el nuevo número asignado a pinLed es par o impar
	if((pinLed%2) == 0){

		//Si el valor de pinLed es par implica pin estará desactivado
		pinLed = 0;
	}
	else{

		//Si el valor de pinLed es impar implica pin estará activado
		pinLed = 1;
	}

	//Llamamos función gpio_WritePin para aplicar el cambio en el estado del pin.
	//El valor asignado en pinLed será el encargado de configurar el encendido
	//(en caso de ser 1) o apagado (en caso de ser 0) en el estado del Pin.
	gpio_WritePin(pPinHandler,pinLed);

}




