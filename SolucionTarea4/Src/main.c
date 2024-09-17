/**
 ******************************************************************************
 * @file           : main.c
 * @author         : labravo (Laura Sofia Bravo Revelo)
 * @brief          : Solución Tarea 4.
 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32_assert.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"
#include "usart_driver_hal.h"
#include "i2c_driver_hal.h"
#include "pwm_driver_hal.h"

//Definición de los pines necesarios para led de estado
GPIO_Handler_t   blinkyPin       = {0};

//Definición de los pines necesarios para led RGB
GPIO_Handler_t ledRed       = {0};
GPIO_Handler_t ledGreen     = {0};
GPIO_Handler_t ledBlue      = {0};

//Timers a utilizar para funcionamiento del led de estado
Timer_Handler_t  blinkyTimer       = {0};

//Comunicación RS-232 con el PC, ya habilitada en la board Nucleo
//Hace uso de la conexión USB
USART_Handler_t  usart2commSerial = {0};

//Definición de pines a usar  para realizar la comunicación serial
GPIO_Handler_t   pinTx            = {0};
GPIO_Handler_t   pinRx            = {0};

//Pines a usar para funcionamiento del I2C
GPIO_Handler_t  pinSCL      = {0};
GPIO_Handler_t  pinSDA      = {0};

//Definició
I2C_Handler_t   accelSensor = {0};
uint8_t i2c_AuxBuffer    = 0;

/*Registros y valores relacionados con el MPU*/
#define  ACCEL_ADDRESS       0b1101000  //0xD0 --> dirección del Accel con Logic_0
#define  ACCEL_XOUT_H        59 //0x3B
#define  ACCEL_XOUT_L        60 //0x3C
#define  ACCEL_YOUT_H        61 //0x3D
#define  ACCEL_YOUT_L        62 //0X3E
#define  ACCEL_ZOUT_H        63 //0x3F
#define  ACCEL_ZOUT_L        64 //0x40
#define  PWR_MGMT_1          107
#define  WHO_AM_I            117

uint8_t          sendMsg          = {0};
uint8_t          usart2DataRecv   = '\0';

//Mensaje que se imprimer
char bufferMsg[64]  = "Accel MPU-6050 Testing...\n";
char greelingMsg[]  = "Taller V Rocks!!!\n";


//Definición de cabeceras de las funciones
void initialSystem(void);
void config_RGB(void);
void config_I2C(void);


/*  Main function  */
int main(void)
{
	initialSystem();

	config_RGB();

	config_I2C();

	usart_writeMsg(&usart2commSerial, greelingMsg);

    /* Loop forever */
	while(1){

		//El sistema siempre está verificando si el valor de rxData ha cambiado
		//(lo cual sucede en la ISR de la recepción RX).
		//Si este valor deja de ser '\0'significa que se recibió un caracter,
		//por lo tanto entra en el bloque if para analizar que se recibió
		if(usart2DataRecv != '\0'){

			if(usart2DataRecv == 'm'){
				usart_writeMsg(&usart2commSerial, bufferMsg);
				usart2DataRecv = '\0';
			}

			if(usart2DataRecv == 'w'){
				sprintf(bufferMsg, "WHO_AM_I? (r)\n");
				usart_writeMsg(&usart2commSerial, bufferMsg);

				i2c_AuxBuffer = i2c_ReadSingleRegister(&accelSensor, WHO_AM_I);
				sprintf(bufferMsg,"dataRead = 0x%x \n", (unsigned int) i2c_AuxBuffer);
				usart_writeMsg(&usart2commSerial, bufferMsg);
				usart2DataRecv = '\0';
 			}

			else if(usart2DataRecv == 'p'){
				sprintf(bufferMsg, "PWR_MGMT_1 state (r)\n");
				usart_writeMsg(&usart2commSerial, bufferMsg);

				i2c_AuxBuffer = i2c_ReadSingleRegister(&accelSensor, PWR_MGMT_1);
				sprintf(bufferMsg,"dataRead = 0x%x \n", (unsigned int) i2c_AuxBuffer);
				usart_writeMsg(&usart2commSerial, bufferMsg);
				usart2DataRecv = '\0';
			}

			else if(usart2DataRecv == 'r'){
				sprintf(bufferMsg, "PWR_MGMT_1 reset (r)\n");
				usart_writeMsg(&usart2commSerial, bufferMsg);

				i2c_WriteSingleRegister(&accelSensor, PWR_MGMT_1, 0x00);
				usart2DataRecv = '\0';
			}

			else if(usart2DataRecv == 'x'){
				sprintf(bufferMsg, "Axis X data (r)\n");
				usart_writeMsg(&usart2commSerial, bufferMsg);

				uint8_t AccelX_low  = i2c_ReadSingleRegister(&accelSensor, ACCEL_XOUT_L);
				uint8_t AccelX_high = i2c_ReadSingleRegister(&accelSensor, ACCEL_XOUT_H);
				int16_t AccelX = AccelX_high << 8 | AccelX_low;

				sprintf(bufferMsg, "AccelX = %d \n", (int) AccelX);
				usart_writeMsg(&usart2commSerial, bufferMsg);
				usart2DataRecv = '\0';
			}

			else if(usart2DataRecv == 'y'){
				sprintf(bufferMsg, "Axis Y data (r)\n");
				usart_writeMsg(&usart2commSerial, bufferMsg);

				uint8_t AccelY_low  = i2c_ReadSingleRegister(&accelSensor, ACCEL_YOUT_L);
				uint8_t AccelY_high = i2c_ReadSingleRegister(&accelSensor, ACCEL_YOUT_H);
				int16_t AccelY = AccelY_high << 8 | AccelY_low;

				sprintf(bufferMsg, "AccelY = %d \n", (int) AccelY);
				usart_writeMsg(&usart2commSerial, bufferMsg);
				usart2DataRecv = '\0';
			}

			else if(usart2DataRecv == 'z'){
				sprintf(bufferMsg, "Axis Z data (r)\n");
				usart_writeMsg(&usart2commSerial, bufferMsg);

				uint8_t AccelZ_low  = i2c_ReadSingleRegister(&accelSensor, ACCEL_ZOUT_L);
				uint8_t AccelZ_high = i2c_ReadSingleRegister(&accelSensor, ACCEL_ZOUT_H);
				int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;

				sprintf(bufferMsg, "AccelZ = %d \n", (int) AccelZ);
				usart_writeMsg(&usart2commSerial, bufferMsg);
				usart2DataRecv = '\0';
			}

			else if(usart2DataRecv == 'q'){
				sprintf(bufferMsg, "All 3 Axis(r)\n");
				usart_writeMsg(&usart2commSerial, bufferMsg);

				uint8_t AccelData[6]  = {0};
				i2c_ReadManyRegisters(&accelSensor, ACCEL_XOUT_H, AccelData, 6);
				int16_t AccelX = AccelData[0] << 8 | AccelData[1];
				int16_t AccelY = AccelData[2] << 8 | AccelData[3];
				int16_t AccelZ = AccelData[4] << 8 | AccelData[5];
				sprintf(bufferMsg, "Accel x, y, z -> %d; %d; %d \n", (int) AccelX,(int) AccelY,(int) AccelZ );
				usart_writeMsg(&usart2commSerial, bufferMsg);
				usart2DataRecv = '\0';
			}

			usart2DataRecv = '\0';
		}


	}

	return 0;

}

//Función para configuración inicial
void initialSystem(void){

	/* Configuramos el pin A5*/
	blinkyPin.pGPIOx                         = GPIOH;
	blinkyPin.pinConfig.GPIO_PinNumber       = PIN_1;
	blinkyPin.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	blinkyPin.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	blinkyPin.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
	blinkyPin.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&blinkyPin);

	//Ejecutamos la configuración realizada en A5
	//gpio_WritePin(&blinkyPIn, SET);


	/* Configuramos el timer del blinky*/
	blinkyTimer.pTIMx                             = TIM2;
	blinkyTimer.TIMx_Config.TIMx_Prescaler        = 16000;  //Genera incrementos de 1 ms
	blinkyTimer.TIMx_Config.TIMx_Period           = 500;     //De la mano con el prescaler, genera int ada 500 ms
	blinkyTimer.TIMx_Config.TIMx_mode             = TIMER_UP_COUNTER;
	blinkyTimer.TIMx_Config.TIMx_InterruptEnable  = TIMER_INT_ENABLE;

	/* Configuramos el Timer */
	timer_Config(&blinkyTimer);

	//Encendemos el Timer
	timer_SetState(&blinkyTimer, TIMER_ON);

	/*Configuración Comm serial*/
	pinTx.pGPIOx                           = GPIOA;
	pinTx.pinConfig.GPIO_PinNumber         = PIN_2;
	pinTx.pinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
	pinTx.pinConfig.GPIO_PinAltFunMode     = AF7;
	pinTx.pinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
	pinTx.pinConfig.GPIO_PinOutputSpeed    = GPIO_OSPEED_FAST;

	gpio_Config(&pinTx);

	pinRx.pGPIOx                           = GPIOA;
	pinRx.pinConfig.GPIO_PinNumber         = PIN_3;
	pinRx.pinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
	pinRx.pinConfig.GPIO_PinAltFunMode     = AF7;
	pinRx.pinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
	pinRx.pinConfig.GPIO_PinOutputSpeed    = GPIO_OSPEED_FAST;

	gpio_Config(&pinRx);

	usart2commSerial.ptrUSARTx                = USART2;
	usart2commSerial.USART_Config.baudrate    = USART_BAUDRATE_115200;
	usart2commSerial.USART_Config.datasize    = USART_DATASIZE_8BIT;
	usart2commSerial.USART_Config.parity      = USART_PARITY_NONE;
	usart2commSerial.USART_Config.stopbits    = USART_STOPBIT_1;
	usart2commSerial.USART_Config.mode        = USART_MODE_RXTX;
	usart2commSerial.USART_Config.enableIntRX = USART_RX_INTERRUP_ENABLE;
	usart2commSerial.USART_Config.enableIntTX = USART_TX_INTERRUP_DISABLE;

	usart_Config(&usart2commSerial);

	usart_WriteChar(&usart2commSerial, '\0');



}

//Función configuración de caracterśiticas para I2C
void config_I2C(void){

	pinSCL.pGPIOx                          = GPIOB;
	pinSCL.pinConfig.GPIO_PinNumber        = PIN_8;
	pinSCL.pinConfig.GPIO_PinMode          = GPIO_MODE_ALTFN;
	pinSCL.pinConfig.GPIO_PinOutputType    = GPIO_OTYPE_OPENDRAIN;
	pinSCL.pinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
	pinSCL.pinConfig.GPIO_PinOutputSpeed   = GPIO_OSPEED_FAST;
	pinSCL.pinConfig.GPIO_PinAltFunMode    = AF4;

	gpio_Config(&pinSCL);

	pinSDA.pGPIOx                          = GPIOB;
	pinSDA.pinConfig.GPIO_PinNumber        = PIN_9;
	pinSDA.pinConfig.GPIO_PinMode          = GPIO_MODE_ALTFN;
	pinSDA.pinConfig.GPIO_PinOutputType    = GPIO_OTYPE_OPENDRAIN;
	pinSDA.pinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
	pinSDA.pinConfig.GPIO_PinOutputSpeed   = GPIO_OSPEED_FAST;
	pinSDA.pinConfig.GPIO_PinAltFunMode    = AF4;

	gpio_Config(&pinSDA);

	accelSensor.pI2Cx          = I2C1;
	accelSensor.i2c_mainClock  = I2C_MAIN_CLOCK_16_MHz;
	accelSensor.i2c_mode       = eI2C_MODE_SM;
	accelSensor.slaveAddress   = ACCEL_ADDRESS;

	i2c_Config(&accelSensor);
}

//Función configuración del led RGB
void config_RGB(void){


	ledRed.pGPIOx                         = GPIOB;
	ledRed.pinConfig.GPIO_PinNumber       = PIN_0;
	ledRed.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	ledRed.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	ledRed.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_FAST;
	ledRed.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&ledRed);
	gpio_WritePin(&ledRed, RESET);

	ledGreen.pGPIOx                         = GPIOC;
	ledGreen.pinConfig.GPIO_PinNumber       = PIN_1;
	ledGreen.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	ledGreen.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	ledGreen.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_FAST;
	ledGreen.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&ledGreen);
	gpio_WritePin(&ledGreen, RESET);

	ledBlue.pGPIOx                         = GPIOC;
	ledBlue.pinConfig.GPIO_PinNumber       = PIN_0;
	ledBlue.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	ledBlue.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	ledBlue.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_FAST;
	ledBlue.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&ledBlue);
	gpio_WritePin(&ledBlue, RESET);
}

/*
 * Overwrite function for A5
 * */
void Timer2_Callback(void){
	gpio_TooglePin(&blinkyPin);
	sendMsg++;;
}

void usart2_RxCallback(void){

	//Importante!!!
	// Asignamos es valor de la función que llama usrt_getRxData, puesto que esta toma el
	//valor que está cargado en el DR
	usart2DataRecv = usart_getRxData();
}

/*
 * Esta función sirve para detectar problemas de parámetros
 * incorrectos al momento de ejecutar un programa.
 * */
void assert_failed(uint8_t* file, uint32_t line){
	while(1){
		//Problems...
	}
}
