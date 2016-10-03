/*
 * flash.c
 *
 *  Created on: 11 sept. 2016
 *      Author: mick
 */


#include <stdlib.h>
#include <string.h>
#include "stm32f3xx_hal_flash.h"
#include "flash.h"


int flash_erase_page(const uint32_t address)
{
  HAL_StatusTypeDef status;
  FLASH_EraseInitTypeDef erase;
  uint32_t error;
  erase.NbPages =1;
  erase.PageAddress = address;
  erase.TypeErase = FLASH_TYPEERASE_PAGES;
  status =  HAL_FLASHEx_Erase(&erase, &error);

  if(status != HAL_OK)
  {
    return -1;
  }

  return 0;
}


int flash_Write(const uint32_t address, const uint8_t * const data, const uint16_t length)
{
  HAL_StatusTypeDef status;
  uint8_t *buffer = (uint8_t *)data;
  uint32_t offset = address;
  int32_t len = length ;
  uint32_t data32;

  if((address & 0x3) != 0)
  {
    return -1;
  }

  while(len > 0)
  {
    /* Get U32 */
    data32 = *(uint32_t *)buffer;

    /* Pad partial U32 with 0xFF */
    if(len < 4)
    {
      if(1 == len)
      {
        data32 |= 0x00FFFFFF;
      }
      else if(2 == len)
      {
        data32 |= 0x0000FFFF;
      }
      else if(3 == len)
      {
        data32 |= 0x000000FF;
      }
    }

    /* Write the data at desired location */
    status =  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, offset, data32);

    if(status != HAL_OK)
    {
      return -1;
    }

    offset += 4;
    buffer += 4;
    len  -= 4;
  }

  return 0;
}


int  flash_Update(const uint32_t address, const uint8_t * data, uint16_t length)
{
  /* Check the ability to write at the required address, the length */
  if(!IS_FLASH_PROGRAM_ADDRESS(address) || !IS_FLASH_PROGRAM_ADDRESS(address + length))
  {
    return -1;
  }

  uint16_t len = length;
  /* Unlock Flash */
  HAL_FLASH_Unlock();
  uint8_t *temp = (uint8_t *)malloc(FLASH_PAGE_SIZE);
  /* Use a temporary variable to retrieve page address */
  uint32_t pageAddress = address;
  uint32_t written = 0;

  while(len > 0)
  {
    /* Retrieve page address */
    uint32_t offset = pageAddress % FLASH_PAGE_SIZE;

    if(offset != 0)
    {
      /* In case of the offset, isn't 0, it means that we are not aligned
       * to a page address.
       * We need to retrieve it, to insure the writing*/
      pageAddress -= offset;
    }

    /* Read page address content in order to merge it with our data to write */
    memcpy(temp, (uint8_t*)pageAddress, FLASH_PAGE_SIZE);
    uint16_t write = len;

    if(write + offset > FLASH_PAGE_SIZE)
    {
      write = (FLASH_PAGE_SIZE - offset);
    }

    /* Merge */
    memcpy(temp + offset, data + written, write);

    /* Erase page address */
    if(flash_erase_page(pageAddress) != 0)
    {
      free(temp);
      HAL_FLASH_Lock();
      return -1;
    }

    /* Write part */

    if(flash_Write(pageAddress, temp, FLASH_PAGE_SIZE) != 0)
    {
      free(temp);
      HAL_FLASH_Lock();
      return -1;
    }

    len -= write;
    written += write;

    /* If we have written the whole data, we can quit else, write the other data
     * on the next page in line*/
    if(len > 0)
    {
      pageAddress += FLASH_PAGE_SIZE;
    }
  }

  free(temp);
  /* Lock Flash */
  HAL_FLASH_Lock();
  #ifdef _DEBUG

  if(memcmp(data,(uint8_t*)address,length) != 0)
  {
    return -1;
  }

  #endif
  return 0;
}

