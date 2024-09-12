/*
 * i2c_driver_hal.c
 *
 *  Created on: 12/09/2024
 *      Author: laurasofia
 */

#include <stdint.h>
#include  "i2c_driver_hal.h"
#include  "gpio_driver_hal.h"

//GPIO_Handler_t    *sdaPin
//GPIO_Handler_t    *sclPin

/*==== Headers for private functions ====*/
static void ic2_enable_clock_peripheral(I2C_Handler_t  *pHandlerI2C);
static void ic2_soft_reset(I2C_Handler_t  *pHandlerI2C);
static void ic2_set_main_clock(I2C_Handler_t  *pHandlerI2C);
static void ic2_set_mode(I2C_Handler_t  *pHandlerI2C);
static void ic2_enable_port(I2C_Handler_t  *pHandlerI2C);
static void ic2_disable_port(I2C_Handler_t  *pHandlerI2C);
static void ic2_stop_signal(I2C_Handler_t  *pHandlerI2C);
static void ic2_start_signal(I2C_Handler_t  *pHandlerI2C);
static void ic2_restart_signal(I2C_Handler_t  *pHandlerI2C);
static void ic2_send_no_ack(I2C_Handler_t  *pHandlerI2C);
static void ic2_send_ack(I2C_Handler_t  *pHandlerI2C);
static void ic2_send_slave_address_rw(I2C_Handler_t  *pHandlerI2C, uint8_t rw);
static void ic2_send_memory_address(I2C_Handler_t  *pHandlerI2C, uint8_t memAddr);
static void ic2_send_close_comm(I2C_Handler_t  *pHandlerI2C);
static void ic2_send_byte(I2C_Handler_t  *pHandlerI2C, uint8_t dataToWrite);
static uint8_t ic2_read_byte(I2C_Handler_t  *pHandlerI2C);

//static void i2c_config_interrupt(I2C_Handler_t *pHandlerI2C);

/*
 * Recordar que se debe configurar los pines para el I2C (SDA Y SCL),
 * para lo cual se necesita el modulo GPIO y los pines configurados
 * en el modulo Alternate Function.
 * Además, estos pines deben ser configurados como salidas open-drain
 * y con las resistencias del modo pull-up
 * */
void i2c_Config(I2C_Handler_t *pHandlerI2C){

	/*1. Activamos la señal de reloj para el periférico*/
	ic2_enable_clock_peripheral(pHandlerI2C);

	/*disable i2c port*/
	ic2_disable_port(pHandlerI2C);

	/*2. Reiniciamos el periférico, de forma que inicia en un estado conocido*/
	ic2_soft_reset(pHandlerI2C);

	/*3. Indicamos cuál es la velocidad del reloj principal, que es la señal utilizada
	 * por el periférico para generar la señal de reloj para el bus I2C*/
	ic2_set_main_clock(pHandlerI2C);

	/*4. Configuramos el modo I2C en el que el sistema funciona
	 * En esta configuración se incluye también la velocidad del reloj
	 * y el tiempo máximo para el cambio de la señal (T-Rise). */
	ic2_set_mode(pHandlerI2C);

	/*5. Activamos el módulo I2C*/
	ic2_enable_port(pHandlerI2C);

	//i2c_stopTransaction(ptrHandlerI2C);
}

/*
 * Activa la señal de reloj RCC para los I2C
 * */
static void ic2_enable_clock_peripheral(I2C_Handler_t  *pHandlerI2C){

	/*1. Activamos la señal de reloj para el módulo I2C seleccionado*/
	if(pHandlerI2C->pI2Cx == I2C1){
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	}
	else if(pHandlerI2C->pI2Cx == I2C2){
		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	}
	else if (pHandlerI2C->pI2Cx == I2C3){
		RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
	}
}

/**/
static void ic2_set_main_clock(I2C_Handler_t  *pHandlerI2C){
	pHandlerI2C->pI2Cx->CR2 &= ~(0b111111 << I2C_CR2_FREQ_Pos); //Borramos la configuración previa
	pHandlerI2C->pI2Cx->CR2 |= (pHandlerI2C->i2c_mainClock << I2C_CR2_FREQ_Pos);
}

/**/
static void ic2_set_mode(I2C_Handler_t  *pHandlerI2C){

	/*Borramos la información de ambos registros (aunque esto lo debe hacer el reset)*/
	pHandlerI2C->pI2Cx->CCR = 0;
	pHandlerI2C->pI2Cx->TRISE = 0;

	if(pHandlerI2C->i2c_mode == eI2C_MODE_SM){

		//Estamos en modo standar (SM MODE)
		//Seleccionamos el modo estándar
		pHandlerI2C->pI2Cx->CCR &= ~I2C_CCR_FS;

		//Configuramos el registro que se encarga de generar la señal de reloj
		pHandlerI2C->pI2Cx->CCR |= (I2C_MODE_SM_SPEED << I2C_CCR_CCR_Pos);

		//Configuramos el registro que controla el tiempo T-Rise máximo
		pHandlerI2C->pI2Cx->TRISE |= I2C_MAX_RISE_TIME_SM;
	}
	else{

		//Estamos en modo Fast (FM MODE)
		//Seleccionamos el mod Fast
		pHandlerI2C->pI2Cx->CCR |= I2C_CCR_FS;

		//Configuramos el registro que se encarga de generar la señal de reloj
		pHandlerI2C->pI2Cx->CCR |= (I2C_MODE_FM_SPEED << I2C_CCR_CCR_Pos);

		//Configuramos el registro que controla el tiempo T-Rise máximo
		pHandlerI2C->pI2Cx->TRISE |= I2C_MAX_RISE_TIME_FM;
	}
}

/*
 * Activa el puerto, sin esto el sistema NO funciona.
 * El puerto debe estar desactivado para poder ser configurado, si se intenta configurar
 * cuando está activo se genera un error.
 * */
static void ic2_enable_port(I2C_Handler_t  *pHandlerI2C){
	pHandlerI2C->pI2Cx->CR1 |= I2C_CR1_PE;
}

/*
 * Desactiva el puerto, haciendo que el sistema no funciona y habilita para que se pueda configurar.
 * El puerto debe estar desactivado para poder ser configurado, si se intenta
 * configurar cuando está activo se genera un error
 * */
static void ic2_disable_port(I2C_Handler_t  *pHandlerI2C){
	pHandlerI2C->pI2Cx->CR1 &= ~I2C_CR1_PE;
}

/*
 * Esta función se encarga de llevar a 0 todos los bits relacionados con el periférico I2C
 * que se está utilizando
 * */
static void ic2_soft_reset(I2C_Handler_t  *pHandlerI2C){
	pHandlerI2C->pI2Cx->CR1 |= I2C_CR1_SWRST;
	__NOP();
	__NOP();
	pHandlerI2C->pI2Cx->CR1 &= ~I2C_CR1_SWRST;
}

/*8. Generamos condición de STOP*/
static void ic2_stop_signal(I2C_Handler_t  *pHandlerI2C){

	/*7. Generamos la condiciónde stop*/
	pHandlerI2C->pI2Cx->CR1 |= I2C_CR1_STOP;
}

/*
 * Función que genera la señal START de un ciclo de comunicación del I2C.
 * El código está relacionado con la figura 164 (pag 481) del manual de referencia
 * del MCU
 *
 * 1.Configuramos bit I2C_CR1_POS
 * 2.Generamos la señal start
 * 2a.Esperamos a que la bandera del evento start se levante.
 * (mientras esperamos, el valor SB es 0, entonces la negación ! es 1)
 * 3.Leemos el registro SR1
 *
 * Estos pasos hacen parte del evento EV5 de la figura 164
 * */
static void ic2_start_signal(I2C_Handler_t  *pHandlerI2C){

	/*0. Definimos una variable auxiliar*/
	uint8_t auxByte = 0;
	(void) auxByte;

	//Errata del MCU... solución en el foro de ST
	//usuario "ERol.1"
	//https://community.st.com/t5/stm32-mcus-products/stm32f2xx-i2c-not-sending-address-after-start/td-p/423510
	//pHandlerI2C->pI2Cx->CR1 &= ~I2C_CR1_STOP;

	/*0. Reset para tener el periférico en un estado conocido*/
	//i2c_softReset(ptrHandlerI2C);

	/*1. Configuramos el control para generar el bit ACK
	 * Este bit posición lo que hace es controlar si nuestro bit ACK genera dicha
	 * señal para el byte que se está leyendo actualmente (I2C_CR1_POS = 0) o para el Byte
	 * que llegará posteriormente (I2C_CR1_POS = 1).
	 * Lo más lógico es trabajar con el byte que se está recibiendo actualmente.
	 * */
}



