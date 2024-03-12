/*
 * Ejemplo.h
 *
 *  Created on: 12/03/2024
 *      Author: laurasofia
 */

#ifndef EJEMPLO_H_
#define EJEMPLO_H_
#include <stdint.h>

typedef struct
{
	char     ID;
	uint8_t  repetitions;
	uint8_t  dummy;
	int16_t  counterUp;
	uint32_t timestamp;
	float    promedio;

}BasicExample;


#endif /* EJEMPLO_H_ */
