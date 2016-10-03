/*
 * command.h
 *
 *  Created on: 28 août 2016
 *      Author: mick
 */

#include <stdint.h>

void cmd_config();

void cmd_loop();

void new_data_it(uint8_t* buf, uint32_t *Len);
