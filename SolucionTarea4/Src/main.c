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

//Definición de pin necesario para led de estado
GPIO_Handler_t   blinkyPin   = {0};

//Timer a utilizar para funcionamiento del led de estado
Timer_Handler_t  blinkyTimer  = {0};

//Definición pines para configuración del PWM en led RGB
GPIO_Handler_t   ledRed_PWMChannel_4    = {0};
GPIO_Handler_t   ledGreen_PWMChannel_3  = {0};
GPIO_Handler_t   ledBlue_PWMChannel_1   = {0};

//Definición de canales a utilizar para hacer uso de PWM
PWM_Handler_t    redPWM_Channel4       = {0};
PWM_Handler_t    greenPWM_Channel3    = {0};
PWM_Handler_t    bluePWM_Channel1      = {0};

//Definición de variables para asignar el valor del duttyCycle de cada canal PWM
uint16_t   duttyValueRed     = 0;
uint16_t   duttyValueGreen   = 0;
uint16_t   duttyValueBlue    = 0;

//Definición de canal USART a utilizar
USART_Handler_t  usart2commSerial = {0};

//Definición de pines a usar  para realizar la comunicación serial
GPIO_Handler_t   pinTx   = {0};
GPIO_Handler_t   pinRx   = {0};

//Timer a utilizar para funcionamiento de muestreo de datos del acelerometro
Timer_Handler_t  accelTimer   = {0};

//Pines a usar para funcionamiento del I2C
GPIO_Handler_t  pinSCL    = {0};
GPIO_Handler_t  pinSDA    = {0};

//Definición de handler de I2C a utilizar
I2C_Handler_t   accelSensor   = {0};

//Definición variable bandera para timer control muestreo acelerómetro
uint8_t banderaTimerAccel = 0;

//Definición variables banderas para timer blinky --> Mostrar en USART datos acelerómetro y PWM
uint8_t bandera_Accel_PWM_Tx = 0;
uint8_t bandera_Accel_PWM_Rx = 0;

/*Registros y valores relacionados con el Acelerómetro*/
#define  ACCEL_ADDRESS       0x1D //Dirección del Accel GY-291 (ADXL345)
#define  ACCEL_XOUT_H        51
#define  ACCEL_XOUT_L        50
#define  ACCEL_YOUT_H        53
#define  ACCEL_YOUT_L        52
#define  ACCEL_ZOUT_H        55
#define  ACCEL_ZOUT_L        54
#define  WHO_AM_I            0x00 //Dirección asociada a DEVICE ID.
#define  DATA_FORMAT         0x31 //Registro asociado a la RANGE SETTING
#define  BW_RATE             0x2C //Registro asociado al BAUD RATE
#define  POWER_CTL           0x2D //Registro asociado al POWER SAVING FEATURES CONTROL

/* Configuraciones iniciales para el acelerómetro */
#define DATA_FORMAT_CONFIG   0b100   //Resolucion configurada en +- 2g, y también se activa justify
#define BW_RATE_CONFIG       0x0A   //Data output rate a 100Hz --> Recomendación presentada en datasheet del accel.
#define POWER_CTL_CONFIG     0b1000 //Activación modo MEASUREMENT

//Definición de variables para cargar los datos de la aceleración en cada eje
uint8_t accelX_low  = 0;
uint8_t accelX_high = 0;
int16_t accelX      = 0;

uint8_t accelY_low  = 0;
uint8_t accelY_high = 0;
int16_t accelY      = 0;

uint8_t accelZ_low  = 0;
uint8_t accelZ_high = 0;
int16_t accelZ      = 0;

//Se establece un offset para garantizar valores positivos en el Dutty --> Dutty depende del valor aceleración
#define  OFFSET_ACCEL_X    17000 //Rango máximo aprox alcanzado por accel en dirección negativa X
#define  OFFSET_ACCEL_Y    19000 //Rango máximo aprox alcanzado por accel en dirección negativa Y
#define  OFFSET_ACCEL_Z    15000 //Rango máximo aprox alcanzado por accel en dirección negativa Z


//Definición de variable para cargar datos de recepción USART
uint8_t   getDataRecv   = '\0';

//Definición de variable auxiliar para lectura de l
uint8_t i2c_AuxBuffer     = 0;

//Definición de buffer para cargar información en aplicación de USART
char bufferMsg[128] =  {0} ;

/*Definición de cabeceras de las funciones*/

//Función para configuración inicial del sistema
void initialSystem(void);

//Función para configuración inicial del acelerómetro
void config_Accel(void);

//Función para configuración inicial del RGB usando PWM
void configPWM_RGB(void);

//Función para configuración I2C
void config_I2C(void);

//Función para configuración USART
void config_USART(void);

//Función para obtener datos de acelerómetro en ejes x,y,z
void get_Accel(void);

//Función para actualizar el dutty del pwm (RGB) en función de datos acelerómetro
void updateDutty_RGB(void);

//Función para actualizar datos a mostrar en comm serial
void show_USART(void);

/*  Main function  */
int main(void)
{
	//Se ejecuta función para configuración inicial
	initialSystem();

	//Se ejecuta función para configuación del I2C
	config_I2C();

	//Se ejecuta función para configuración acelerómetro
	config_Accel();

	//Se ejecuta función para configuracipon del led RGB con PWM
	configPWM_RGB();

	//Se ejecuta función para configuracipon del USART
	config_USART();

    /* Loop forever */
	while(1){

		//Verificamos si la bandera asociada al timer que controla MEASURE acelerómetro está activa
		if(banderaTimerAccel){

			//Se ejecuta función para buscar datos del acelerómetro
			get_Accel();

			//Se actualiza el valor del dutty del PWM relacionado con led RGB
		    updateDutty_RGB();

			//Se desactiva la bandera
			banderaTimerAccel = 0;
		}

		//Llamamos a la función encargada de la representación en USART
		show_USART();
	}

}

//Función para configuración inicial
void initialSystem(void){

	/* Configuramos el pin H1*/
	blinkyPin.pGPIOx                         = GPIOH;
	blinkyPin.pinConfig.GPIO_PinNumber       = PIN_1;
	blinkyPin.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	blinkyPin.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	blinkyPin.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
	blinkyPin.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	//Cargamos la configuración en los registros que gobiernan el puerto
	gpio_Config(&blinkyPin);

	//Ejecutamos la configuración realizada en H1
	//gpio_WritePin(&blinkyPIn, SET);


	/* Configuramos el timer del blinky*/
	blinkyTimer.pTIMx                             = TIM2;
	blinkyTimer.TIMx_Config.TIMx_Prescaler        = 16000;
	blinkyTimer.TIMx_Config.TIMx_Period           = 1000;     //De la mano con el prescaler, genera int cada 1000 ms
	blinkyTimer.TIMx_Config.TIMx_mode             = TIMER_UP_COUNTER;
	blinkyTimer.TIMx_Config.TIMx_InterruptEnable  = TIMER_INT_ENABLE;

	//Cargamos configuración en los registros del Timer
	timer_Config(&blinkyTimer);

	//Encendemos el Timer
	timer_SetState(&blinkyTimer, TIMER_ON);

}

//Función configuración de caracterśiticas para I2C
void config_I2C(void){

	/*Configuración pin asociado a SCL--> Clock*/
	pinSCL.pGPIOx                          = GPIOB;
	pinSCL.pinConfig.GPIO_PinNumber        = PIN_8;
	pinSCL.pinConfig.GPIO_PinMode          = GPIO_MODE_ALTFN;
	pinSCL.pinConfig.GPIO_PinOutputType    = GPIO_OTYPE_OPENDRAIN;
	pinSCL.pinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
	pinSCL.pinConfig.GPIO_PinOutputSpeed   = GPIO_OSPEED_FAST;
	pinSCL.pinConfig.GPIO_PinAltFunMode    = AF4;

	//Cargamos la configuración del pin SCL
	gpio_Config(&pinSCL);

	/*Configuración del pin asociado a SDA --> Data*/
	pinSDA.pGPIOx                          = GPIOB;
	pinSDA.pinConfig.GPIO_PinNumber        = PIN_9;
	pinSDA.pinConfig.GPIO_PinMode          = GPIO_MODE_ALTFN;
	pinSDA.pinConfig.GPIO_PinOutputType    = GPIO_OTYPE_OPENDRAIN;
	pinSDA.pinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
	pinSDA.pinConfig.GPIO_PinOutputSpeed   = GPIO_OSPEED_FAST;
	pinSDA.pinConfig.GPIO_PinAltFunMode    = AF4;

	//Cargamos la configuración del pin SDA
	gpio_Config(&pinSDA);

	/*Configuración del canal I2C a utilizar*/
	accelSensor.pI2Cx          = I2C1;
	accelSensor.i2c_mainClock  = I2C_MAIN_CLOCK_16_MHz;
	accelSensor.i2c_mode       = eI2C_MODE_SM;
	accelSensor.slaveAddress   = ACCEL_ADDRESS;

	//Cargamos la configuración del canal I2C
	i2c_Config(&accelSensor);

	/* Configuramos el timer de control de muestreo del acelerómetro*/
	//The number of samples averaged is a choice of the system designer, but a
	//recommended starting point is 0.1 sec worth of data for data --> En este dato por características descritas en condiciones de la tarea se usa 0.25s
	//rates of 100 Hz or greater. This corresponds to 10 samples at
	//the 100 Hz data rate.
	accelTimer.pTIMx                             = TIM5;
	accelTimer.TIMx_Config.TIMx_Prescaler        = 16000;
	accelTimer.TIMx_Config.TIMx_Period           = 250;   //De la mano con el prescaler, genera int cada 250 ms
	accelTimer.TIMx_Config.TIMx_mode             = TIMER_UP_COUNTER;
	accelTimer.TIMx_Config.TIMx_InterruptEnable  = TIMER_INT_ENABLE;

	//Configuramos el Timer de muestreo Accel
	timer_Config(&accelTimer);

	//Encendemos el Timer
	timer_SetState(&accelTimer, TIMER_ON);
}

//Función de configuración para acelerómetro
void config_Accel(void){

	//Se configura el registro respectivo del data format
	i2c_WriteSingleRegister(&accelSensor, DATA_FORMAT, DATA_FORMAT_CONFIG);

	//Se configura el registro responsable del baudrate
	i2c_WriteSingleRegister(&accelSensor, BW_RATE, BW_RATE_CONFIG);

	//Se configura el rergistro encargado del control de potencia
	i2c_WriteSingleRegister(&accelSensor, POWER_CTL, POWER_CTL_CONFIG);

}

//Función configuración del led RGB
void configPWM_RGB(void){

	/*Configuración timer para generar señal pwm -> Channel 1*/
	redPWM_Channel4.ptrTIMx                = TIM3;
	redPWM_Channel4.config.channel         = PWM_CHANNEL_4;
	redPWM_Channel4.config.duttyCicle      = duttyValueRed;
	redPWM_Channel4.config.periodo         = 1000; //Cant. en ms del periodo
	redPWM_Channel4.config.prescaler       = 160;

	//Cargamos la configuración en los registros
	pwm_Config(&redPWM_Channel4);

	//Se activa el output correspondiente a la salida señal PWM
	pwm_Enable_Output(&redPWM_Channel4);

	//Se activa la emisión de la señal PWM
	pwm_Start_Signal(&redPWM_Channel4);

	/*Configuración PWM -> Channel 1*/
	ledRed_PWMChannel_4.pGPIOx                        = GPIOC;
	ledRed_PWMChannel_4.pinConfig.GPIO_PinNumber      = PIN_9;
	ledRed_PWMChannel_4.pinConfig.GPIO_PinMode        = GPIO_MODE_ALTFN;
	ledRed_PWMChannel_4.pinConfig.GPIO_PinOutputType  = GPIO_OTYPE_PUSHPULL;
	ledRed_PWMChannel_4.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	ledRed_PWMChannel_4.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	ledRed_PWMChannel_4.pinConfig.GPIO_PinAltFunMode  = AF2;

	//Cargamos la configuración en los registros
	gpio_Config(&ledRed_PWMChannel_4);

	/*Configuración timer para generar señal pwm -> Channel 3*/
	greenPWM_Channel3.ptrTIMx                = TIM3;
	greenPWM_Channel3.config.channel         = PWM_CHANNEL_3;
	greenPWM_Channel3.config.duttyCicle      = duttyValueGreen;
	greenPWM_Channel3.config.periodo         = 1000; //Cant. en ms del periodo
	greenPWM_Channel3.config.prescaler       = 160;

	//Cargamos la configuración en los registros
	pwm_Config(&greenPWM_Channel3);

	//Se activa el output correspondiente a la salida señal PWM
	pwm_Enable_Output(&greenPWM_Channel3);

	//Se activa la emisión de la señal PWM
	pwm_Start_Signal(&greenPWM_Channel3);

	/*Configuración PWM -> Channel 3*/
	ledGreen_PWMChannel_3.pGPIOx                        = GPIOC;
	ledGreen_PWMChannel_3.pinConfig.GPIO_PinNumber      = PIN_8;
	ledGreen_PWMChannel_3.pinConfig.GPIO_PinMode        = GPIO_MODE_ALTFN;
	ledGreen_PWMChannel_3.pinConfig.GPIO_PinOutputType  = GPIO_OTYPE_PUSHPULL;
	ledGreen_PWMChannel_3.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	ledGreen_PWMChannel_3.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	ledGreen_PWMChannel_3.pinConfig.GPIO_PinAltFunMode  = AF2;

	//Cargamos la configuración en los registros
	gpio_Config(&ledGreen_PWMChannel_3);

	/*Configuración timer para generar señal pwm -> Channel 1*/
	bluePWM_Channel1.ptrTIMx                = TIM3;
	bluePWM_Channel1.config.channel         = PWM_CHANNEL_1;
	bluePWM_Channel1.config.duttyCicle      = duttyValueBlue;
	bluePWM_Channel1.config.periodo         = 1000; //Cant. en ms del periodo
	bluePWM_Channel1.config.prescaler       = 160;

	//Cargamos la configuración en los registros
	pwm_Config(&bluePWM_Channel1);

	//Se activa el output correspondiente a la salida señal PWM
	pwm_Enable_Output(&bluePWM_Channel1);

	//Se activa la emisión de la señal PWM
	pwm_Start_Signal(&bluePWM_Channel1);

	/*Configuración PWM -> Channel 4*/
	ledBlue_PWMChannel_1.pGPIOx                        = GPIOC;
	ledBlue_PWMChannel_1.pinConfig.GPIO_PinNumber      = PIN_6;
	ledBlue_PWMChannel_1.pinConfig.GPIO_PinMode        = GPIO_MODE_ALTFN;
	ledBlue_PWMChannel_1.pinConfig.GPIO_PinOutputType  = GPIO_OTYPE_PUSHPULL;
	ledBlue_PWMChannel_1.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	ledBlue_PWMChannel_1.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_FAST;
	ledBlue_PWMChannel_1.pinConfig.GPIO_PinAltFunMode  = AF2;

	//Cargamos la configuración en los registros
	gpio_Config(&ledBlue_PWMChannel_1);

}

//Función para configuración USART
void config_USART(void){

	/*Configuración pines Comm serial*/
	pinTx.pGPIOx                           = GPIOA;
	pinTx.pinConfig.GPIO_PinNumber         = PIN_2;
	pinTx.pinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
	pinTx.pinConfig.GPIO_PinAltFunMode     = AF7;
	pinTx.pinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
	pinTx.pinConfig.GPIO_PinOutputSpeed    = GPIO_OSPEED_FAST;

	//Cargamos la configuración pin A2 (Tx) en los registros
	gpio_Config(&pinTx);

	pinRx.pGPIOx                           = GPIOA;
	pinRx.pinConfig.GPIO_PinNumber         = PIN_3;
	pinRx.pinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
	pinRx.pinConfig.GPIO_PinAltFunMode     = AF7;
	pinRx.pinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
	pinRx.pinConfig.GPIO_PinOutputSpeed    = GPIO_OSPEED_FAST;

	//Cargamos la configuración pin A3 (Rx) en los registros
	gpio_Config(&pinRx);

	/*Configuración pines Comm serial --> USART2*/
	usart2commSerial.ptrUSARTx                = USART2;
	usart2commSerial.USART_Config.baudrate    = USART_BAUDRATE_19200;
	usart2commSerial.USART_Config.datasize    = USART_DATASIZE_8BIT;
	usart2commSerial.USART_Config.parity      = USART_PARITY_NONE;
	usart2commSerial.USART_Config.stopbits    = USART_STOPBIT_1;
	usart2commSerial.USART_Config.mode        = USART_MODE_RXTX;
	usart2commSerial.USART_Config.enableIntRX = USART_RX_INTERRUP_ENABLE;
	usart2commSerial.USART_Config.enableIntTX = USART_TX_INTERRUP_DISABLE;

	//Cargamos la configuración en los registros
	usart_Config(&usart2commSerial);

	//Inicializamos el valor en comm serial
	usart_WriteChar(&usart2commSerial, '\0');
}

//Función para obtener datos de aceleración en los tres ejes x.y,z
void get_Accel(void){

	//Se realiza la lectura de los resgistros correspondientes al eje X y sus valores High and Low
	accelX_low  = i2c_ReadSingleRegister(&accelSensor, ACCEL_XOUT_L);
	accelX_high = i2c_ReadSingleRegister(&accelSensor, ACCEL_XOUT_H);
	//Asignamos en una variable el valor relacionado a la aceleración en el eje X
	accelX = (accelX_high << 8) | accelX_low;


	//Se realiza la lectura de los resgistros correspondientes al eje Y y sus valores High and Low
	accelY_low  = i2c_ReadSingleRegister(&accelSensor, ACCEL_YOUT_L);
	accelY_high = i2c_ReadSingleRegister(&accelSensor, ACCEL_YOUT_H);
	//Asignamos en una variable el valor relacionado a la aceleración en el eje Y
	accelY = (accelY_high << 8) | accelY_low;

	//Se realiza la lectura de los resgistros correspondientes al eje Z y sus valores High and Low
	accelZ_low  = i2c_ReadSingleRegister(&accelSensor, ACCEL_ZOUT_L);
	accelZ_high = i2c_ReadSingleRegister(&accelSensor, ACCEL_ZOUT_H);
	//Asignamos en una variable el valor relacionado a la aceleración en el eje Z
	accelZ = (accelZ_high << 8) | accelZ_low;

}

//Función para actualizar el dutty del pwm (RGB) en función de datos acelerómetro
void updateDutty_RGB(void){

	/*Al momento de realizar la actulización del Dutty se debe tener en cuenta que el PWM NO puede ser mayor a 750 (aprox se establece 75% de
	 * max dutty cycle) por este motivo se deben actualizar los valores tanto para respetar este límite, como también para garantizar
	 * que no se carguen valores negativos al dutty del PWM:
	 *
	 * Se sumará en cada eje el aprox MAX_VALUE (en eje negativo) del Accel en cada eje para garantizar dutty positivo, posteriormente se divide entre 55
	 * para no superar el máximo aprox establecido en el periodo de la configuración PWM */
	//Asignamos: valor del acelerómetro en X -->  dutty value RED
	duttyValueRed     = (accelX + OFFSET_ACCEL_X)/55;
	//Actualizamos el valor del dutty dentro de la configuración del PWM
	pwm_Update_DuttyCycle(&redPWM_Channel4, duttyValueRed);

	//Asignamos: valor del acelerómetro en Y -->  dutty value GREEN
	duttyValueGreen   = (accelY + OFFSET_ACCEL_Y)/55;
	//Actualizamos el valor del dutty dentro de la configuración del PWM
	pwm_Update_DuttyCycle(&greenPWM_Channel3, duttyValueGreen);

	//Asignamos: valor del acelerómetro en Z -->  dutty value BLUE
	duttyValueBlue    = (accelZ + OFFSET_ACCEL_Z)/55;
	//Actualizamos el valor del dutty dentro de la configuración del PWM
	pwm_Update_DuttyCycle(&bluePWM_Channel1, duttyValueBlue);

}

//Función para actualizar datos a mostrar en comm serial
void show_USART(void){

	//Verificamos si bandera de USART para transmisión está activa
	if(bandera_Accel_PWM_Tx){

		//Bajamos la bandera de la interrupción
		bandera_Accel_PWM_Tx = 0;

		//Escribimos mensaje con valores de aceleración en los tres ejes
		sprintf(bufferMsg, "Valores aceleración en cada eje:\n X = %d, Y = %d,  Z = %d \n", (int) accelX,(int) accelY,(int) accelZ );
		usart_writeMsg(&usart2commSerial, bufferMsg);

		//Escribimos mensaje con valores del dutty asociado a cada eje/color RGB
		sprintf(bufferMsg, "Dutty de señal PWM en led RGB en cada eje:\n X(Red) = %d, Y(Green) = %d,  Z(Blue) = %d \n\r", (uint) duttyValueRed,(uint) duttyValueGreen,(uint) duttyValueBlue );
		usart_writeMsg(&usart2commSerial, bufferMsg);
	}

	//Verificamos si bandera de USART para recepción está activa
	if(bandera_Accel_PWM_Rx){

		//Bajamos la bandera de la interrupción
		bandera_Accel_PWM_Rx = 0;

		//Importante!!!
		// Asignamos es valor de la función que llama usrt_getRxData, puesto que esta toma el
		//valor que está cargado en el DR
		getDataRecv = usart_getRxData();

		if(getDataRecv == 'm'){

			//Se escribe mensaje predeterminado
			usart_writeMsg(&usart2commSerial,"Taller V Rocks!!!\n\r"  );

			//Limpiamos variable de recepción
			getDataRecv = '\0';
		}
		else if(getDataRecv == 'w'){

			//Se escribe que estamos dentro de la función de búsqueda WHO_AM_I
			sprintf(bufferMsg, "WHO_AM_I? \n");
			usart_writeMsg(&usart2commSerial, bufferMsg);

			//Leemos registro asociado a WHO_AM_I
			i2c_AuxBuffer = i2c_ReadSingleRegister(&accelSensor, WHO_AM_I);

			//Se escribe el resultado obtenido en la lectura
			sprintf(bufferMsg,"dataRead = 0x%x \n\r", (unsigned int) i2c_AuxBuffer);
			usart_writeMsg(&usart2commSerial, bufferMsg);

			//Limpiamos variable de recepción
			getDataRecv = '\0';
		}

		//Limpiamos variable de recepción
		getDataRecv = '\0';
	}
}

/*
 * Overwrite function for H1
 * */
void Timer2_Callback(void){

	gpio_TooglePin(&blinkyPin);

	//Se activa la bandera asociada a USART Tx
	bandera_Accel_PWM_Tx = 1;
}

/*
 * Overwrite function for control--> Muestreo del Acelerómetro.
 * */
void Timer5_Callback(void){

	//Se activa bandera encargada de generar tiempos de espera entre análisis de datos del accel
	banderaTimerAccel = 1;
}

/*
 * Overwrite function for usart Rx
 * */
void usart2_RxCallback(void){

	//Activamos bandera asociada a USART Rx
	bandera_Accel_PWM_Rx = 1;
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
