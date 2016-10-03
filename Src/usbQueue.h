/*
 * queue.h
 *
 *  Created on: 24 août 2016
 *      Author: mick
 */

#ifndef _QUEUE_H_
#define _QUEUE_H_



#define USB_QUEUE_SIZE 5
#define BUFFER_SIZE    (sizeof(int)*15)

typedef struct
{
  uint8_t  buffer[BUFFER_SIZE];
  uint32_t  len;
} message_t;

#endif /* _QUEUE_H_ */
