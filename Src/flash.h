/*
 * flash.h
 *
 *  Created on: 11 sept. 2016
 *      Author: mick
 */

#ifndef _FLASH_H_
#define _FLASH_H_

#include <stdint.h>
#include "stm32f3xx.h"


#define FLASH_BASE_ADDRESS FLASH_BASE
#define FLASH_SIZE  ((*((uint16_t *)FLASH_SIZE_DATA_REGISTER)) * 1024)

int  flash_Update(const uint32_t address, const uint8_t * data, uint16_t length);

#endif /* _FLASH_H_ */
