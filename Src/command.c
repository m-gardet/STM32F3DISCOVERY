/*
 * command.c
 *
 *  Created on: 28 août 2016
 *      Author: mick
 */



#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "queue.h"
#include "usbQueue.h"
#include "usbd_cdc_if.h"
#include <stdio.h>
#include "stm32f3xx_hal_gpio.h"
#include "flash.h"
#include "iwdg.h"

#define END_OF_LINE "\n\r"

#define OS_WAITE_TIMEOUT 1000 // in ms

const char RESERVED_PIN[][5] =
{
  //PINs    FUNCTIONs     LABELs
  "PE02",// GPIO_EXTI2      DRDY      [LSM303DLHC_DRDY]
  "PE03",// GPIO_Output     CS_I2C/SPI    [L3GD20_CS_I2C/SPI]
  "PE04",// GPIO_EXTI4      MEMS_INT3     [LSM303DLHC_INT1]
  "PE05",// GPIO_EXTI5      MEMS_INT4     [LSM303DLHC_INT2]
  "PC14",// OSC32_IN      RCC_OSC32_IN  OSC32_IN
  "PC15",// OSC32_OUT     RCC_OSC32_OUT OSC32_OUT
  "PF00",// OSC_IN        RCC_OSC_IN    OSC_IN
  "PF01",// OSC_OUT       RCC_OSC_OUT   OSC_OUT
  "PF02",// ADC1_IN10     ADC
  "PA00",// GPIO_Input      B1        [Blue PushButton]
  "PA04",// DAC_OUT1      DAC
  "PA05",// SPI1_SCK      SPI1_SCK    [L3GD20_SCL/SPC]
  "PA06",// SPI1_MISO     SPI1_MISO     [L3GD20_SA0/SDO]
  "PA07",// SPI1_MOSI     SPI1_MISO     [L3GD20_SDA/SDI/SDO]
  /*
  "PE08",// GPIO_Output     LD4       [Blue Led]
  "PE09",// GPIO_Output     LD3       [Red Led]
  "PE10",// GPIO_Output     LD5       [Orange Led]
  "PE11",// GPIO_Output     LD7       [Green Led]
  "PE12",// GPIO_Output     LD9       [Blue Led]
  "PE13",// GPIO_Output     LD10      [Red Led]
  "PE14",// GPIO_Output     LD8       [Orange Led]
  "PE15",// GPIO_Output     LD6       [Green Led]
  */
  "PA11",// USB_DM        DM
  "PA12",// USB_DP        DP
  "PA13",// SYS_JTMS-SWDIO    SWDIO
  "PA14",// SYS_JTCK-SWCLK    SWCLK
  "PB03",// SYS_JTDO-TRACESWO SWO
  "PB06",// I2C1_SCL      I2C1_SCL [LSM303DLHC_SCL]
  "PB07",// I2C1_SDA      I2C1_SDA [LSM303DLHC_SDA]
  "PE00",// GPIO_EXTI0      MEMS_INT1 [L3GD20_INT1]
  "PE01",// GPIO_EXTI1      MEMS_INT2 [L3GD20_DRDY/INT2]
  "\0"
};

typedef struct
{
  const uint8_t name[5];
  uint8_t mode;
  uint8_t pull;
  uint8_t state;
} gpio_st;

typedef struct
{
  char sys_name[50];
  gpio_st gpio[64];
} config_st;


config_st config =
{
  {
    "IO Demo"
  },
  {
    //{ "PA00" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET }, [Blue PushButton]
    { "PA01" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PA02" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PA03" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    //{ "PA04" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET }, DAC
    //{ "PA05" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET }, L3GD20_SCL/SPC
    //{ "PA06" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET }, L3GD20_SA0/SDO
    //{ "PA07" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET }, L3GD20_SDA/SDI/SDO
    { "PA08" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PA09" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PA10" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    //{ "PA11" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET }, USB_DM
    //{ "PA12" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET }, USB_DP
    //{ "PA13" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET }, SYS_JTMS
    //{ "PA14" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET }, SYS_JTCK
    { "PA15" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PB00" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PB01" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PB02" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    //{ "PB03" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET }, SYS_JTDO-TRACESWO
    { "PB04" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PB05" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    //{ "PB06" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET }, LSM303DLHC_SCL
    //{ "PB07" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET }, LSM303DLHC_SDA

    // PE8 - PE15 => LED
    { "PB08" , GPIO_MODE_OUTPUT_PP , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PB09" , GPIO_MODE_OUTPUT_PP , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PB10" , GPIO_MODE_OUTPUT_PP , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PB11" , GPIO_MODE_OUTPUT_PP , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PB12" , GPIO_MODE_OUTPUT_PP , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PB13" , GPIO_MODE_OUTPUT_PP , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PB14" , GPIO_MODE_OUTPUT_PP , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PB15" , GPIO_MODE_OUTPUT_PP , GPIO_NOPULL , GPIO_PIN_RESET },

    { "PC00" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PC01" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PC02" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PC03" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PC04" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PC05" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PC06" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PC07" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PC08" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PC09" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PC10" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PC11" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PC12" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },

    /* The speed should not exceed 2 MHz with a maximu These GPIOs must not be used as current sources (3 mA)
        { "PC13" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
        { "PC14" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET }, // OSC32_IN
        { "PC15" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET }, // OSC32_OUT
    */
    { "PD00" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PD01" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PD02" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PD03" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PD04" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PD05" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PD06" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PD07" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PD08" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PD09" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PD10" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PD11" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PD12" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PD13" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PD14" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PD15" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },

    //{ "PE00" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET }, L3GD20_INT1
    //{ "PE01" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET }, L3GD20_DRDY/INT2
    //{ "PE02" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET }, LSM303DLHC_DRDY
    //{ "PE03" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET }, L3GD20_CS_I2C/SPI
    //{ "PE04" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET }, LSM303DLHC_INT1
    //{ "PE05" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET }, LSM303DLHC_INT2

    { "PE06" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PE07" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PE08" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PE09" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PE10" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PE11" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PE12" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PE13" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PE14" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PE15" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    //{ "PF00" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET }, //OSC_IN
    //{ "PF01" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET }, //OSC_OUT
    //{ "PF02" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET }, ADC1_IN10
    { "PF04" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PF06" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PF09" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
    { "PF10" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },

    { "\0" , GPIO_MODE_INPUT , GPIO_NOPULL , GPIO_PIN_RESET },
  }
};


#define CMD_OK "*OK* "
#define CMD_KO "*KO* "

#define HELP_NAME "HELP"
#define HELP_NAME_LEN (sizeof(HELP_NAME)-1)

#define SET_SYS_NAME_NAME "SETNAME"
#define SET_SYS_NAME_LEN (sizeof(SET_SYS_NAME_NAME)-1)

#define GET_SYS_NAME_NAME "GETNAME"
#define GET_SYS_NAME_LEN (sizeof(GET_SYS_NAME_NAME)-1)

#define UNKNOWN_TOTAL_LEN  0xFF

#define GET_UID_NAME "GET UID"
#define GET_UID_NAME_LEN (sizeof(GET_UID_NAME)-1)

#define SAVE_CONFIG_NAME "CONFIG SAVE"
#define SAVE_CONFIG_NAME_LEN (sizeof(SAVE_CONFIG_NAME)-1)

#define DUMP_CONFIG_NAME "CONFIG DUMP"
#define DUMP_CONFIG_NAME_LEN (sizeof(DUMP_CONFIG_NAME)-1)

#define GPIO_INIT_NAME "INIT"
#define GPIO_INIT_NAME_LEN (sizeof(GPIO_INIT_NAME)-1)

#define GPIO_NAME_LEN 4

#define GPIO_MODE_LEN     3
#define MODE_INPUT        "INP"
#define MODE_OUTPUT_PP    "OPP"
#define MODE_OUTPUT_OD    "OOD"

#define GPIO_PULL_LEN  2
#define NOPULL        "NO"
#define PULLUP        "UP"
#define PULLDOWN      "DO"

#define GPIO_INIT_TOTAL_LEN (GPIO_INIT_NAME_LEN+1 +GPIO_NAME_LEN+1  + GPIO_MODE_LEN+1 + GPIO_PULL_LEN)


#define GPIO_SET_NAME "SET"
#define GPIO_SET_NAME_LEN (sizeof(GPIO_SET_NAME)-1)
#define GPIO_VALUE_LEN 1
#define GPIO_SET_TOTAL_LEN (GPIO_SET_NAME_LEN+1 +GPIO_NAME_LEN+1  + GPIO_VALUE_LEN)


#define GPIO_GET_NAME "GET"
#define GPIO_GET_NAME_LEN (sizeof(GPIO_GET_NAME)-1)
#define GPIO_GET_TOTAL_LEN (GPIO_GET_NAME_LEN+1 +GPIO_NAME_LEN)


#define LED_PORT GPIOE
#define LED_PIN GPIO_PIN_9


osMessageQDef(osMsgQueue, USB_QUEUE_SIZE, uint32_t);
osMessageQId  osMsgQueue;
message_t usbQueue[USB_QUEUE_SIZE];
uint32_t   queueIndex=0;


void cmd_parsing(uint8_t* cmd,uint8_t  len);

int help(uint8_t* cmd,uint8_t len)
{
  UNUSED(cmd);
  UNUSED(len);
  usb_printf("\n\r");
  usb_printf("----------------------help----------------------\n\r");
  usb_printf("HELP (this message)\n\r");
  usb_printf("--------------system configuration--------------\n\r");
  usb_printf("%s (%d)\n\r",SET_SYS_NAME_NAME,SET_SYS_NAME_LEN+sizeof(config.sys_name));
  usb_printf("%s (%d)\n\r",GET_SYS_NAME_NAME,GET_SYS_NAME_LEN);
  usb_printf("%s (%d)\n\r",GET_UID_NAME,GET_UID_NAME_LEN);
  usb_printf("%s (%d)\n\r",SAVE_CONFIG_NAME,SAVE_CONFIG_NAME_LEN);
  usb_printf("%s (%d)\n\r",DUMP_CONFIG_NAME,DUMP_CONFIG_NAME_LEN);
  usb_printf("---------------gpio configuration---------------\n\r");
  usb_printf("gpio configuration\n\r");
  usb_printf("%s NAME MODE PULL\n\r",GPIO_INIT_NAME);
  usb_printf("  NAME (%d bytes)n\r",GPIO_NAME_LEN);
  usb_printf("        PC13, PA00, PB15, ...\n\r");
  usb_printf("  MODE (%d bytes) \n\r",GPIO_MODE_LEN);
  usb_printf("        %s   (Input Floating)\n\r",MODE_INPUT);
  usb_printf("        %s   (Output Push Pull Mode)\n\r",MODE_OUTPUT_PP);
  usb_printf("        %s   (Output Open Drain Mode)\n\r",MODE_OUTPUT_OD);
  usb_printf("  PULL (%d bytes) \n\r",GPIO_PULL_LEN);
  usb_printf("        %s   (No Pull-up or Pull-down)\n\r",NOPULL);
  usb_printf("        %s   (Pull-up)\n\r",PULLUP);
  usb_printf("        %s   (Pull-down)\n\r",PULLDOWN);
  usb_printf("sample\n");
  usb_printf("INIT PC13 OPP NO (%d bytes)\n\r",GPIO_INIT_NAME_LEN);
  usb_printf("-----------------gpio read/write----------------\n\r");
  usb_printf("GET NAME\n\r");
  usb_printf("%s NAME VALUE (%d bytes)\n\r",GPIO_SET_NAME,GPIO_SET_TOTAL_LEN);
  /*
    usb_printf("--------------------adc  read-------------------\n\r");
    usb_printf("ADC NUMBER ( 00 to 01)\n\r");
    usb_printf("\n\r");
  */
  return 0;
}


int gpio_write_pin(GPIO_TypeDef* GPIOx, uint16_t pin, GPIO_PinState state)
{
  /* Check the parameters */
  if(!IS_GPIO_PIN(pin))
  {
    return -1;
  }

  if(!IS_GPIO_PIN_ACTION(state))
  {
    return -1;
  }

  HAL_GPIO_WritePin(GPIOx, pin, state);
  return 0;
}

int gpio_read_pin(GPIO_TypeDef* GPIOx, uint16_t pin)
{
  /* Check the parameters */
  if(!IS_GPIO_PIN(pin))
  {
    return -1;
  }

  return HAL_GPIO_ReadPin(GPIOx, pin);
}

static void led_on(void)
{
  gpio_write_pin(LED_PORT,  LED_PIN
                 //GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15
                 , GPIO_PIN_SET);
}

static void led_off(void)
{
  gpio_write_pin(LED_PORT,  LED_PIN
                 //GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15
                 , GPIO_PIN_RESET);
}

int gpio_init(GPIO_TypeDef *GPIOx,uint32_t pin,uint32_t mode,uint32_t pull)
{
  if(! IS_GPIO_ALL_INSTANCE(GPIOx))
  {
    return -1;
  }

  if(! IS_GPIO_PIN(pin))
  {
    return -1;
  }

  if(! IS_GPIO_MODE(mode))
  {
    return -1;
  }

  if(! IS_GPIO_PULL(pull))
  {
    return -1;
  }

  GPIO_InitTypeDef gpioInitStructure;
  gpioInitStructure.Pin = pin;
  gpioInitStructure.Mode = mode;
  gpioInitStructure.Pull = pull;
  gpioInitStructure.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOx, &gpioInitStructure);
  return 0;
}

int get_port_and_pin(const uint8_t name[GPIO_NAME_LEN],GPIO_TypeDef **port,uint32_t *pin)
{
  int number=-1;

  if(name[0] != 'P')
  {
    usb_printf(CMD_KO "Invalid format: %c != P\n\r",name[0]);
    return -1;
  }

  switch(name[1])
  {
    case 'A':
      *port = GPIOA;
      break;

    case 'B':
      *port = GPIOB;
      break;

    case 'C':
      *port = GPIOC;
      break;

    case 'D':
      *port = GPIOD;
      break;

    case 'E':
      *port = GPIOE;
      break;

    case 'F':
      *port = GPIOF;
      break;

    default:
      usb_printf(CMD_KO "unknown port: %c\n\r",name[1]);
      return -1;
      break;
  }

  number = (name[2]-'0')*10 + (name[3]-'0');

  switch(number)
  {
    case 0 :
      *pin = GPIO_PIN_0;
      break;

    case 1 :
      *pin = GPIO_PIN_1;
      break;

    case 2 :
      *pin = GPIO_PIN_2;
      break;

    case 3 :
      *pin = GPIO_PIN_3;
      break;

    case 4 :
      *pin = GPIO_PIN_4;
      break;

    case 5 :
      *pin = GPIO_PIN_5;
      break;

    case 6 :
      *pin = GPIO_PIN_6;
      break;

    case 7 :
      *pin = GPIO_PIN_7;
      break;

    case 8 :
      *pin = GPIO_PIN_8;
      break;

    case 9 :
      *pin = GPIO_PIN_9;
      break;

    case 10:
      *pin = GPIO_PIN_10;
      break;

    case 11:
      *pin = GPIO_PIN_11;
      break;

    case 12:
      *pin = GPIO_PIN_12;
      break;

    case 13:
      *pin = GPIO_PIN_13;
      break;

    case 14:
      *pin = GPIO_PIN_14;
      break;

    case 15:
      *pin = GPIO_PIN_15;
      break;

    default:
      usb_printf(CMD_KO "unknown pin number: %d\n\r",number);
      return -1;
      break;
  }

  return 0;
}

int is_reserved(uint8_t name[GPIO_NAME_LEN])
{
  return 0;
  int r=0;

  while(RESERVED_PIN[r][0] != '\0')
  {
    if(strncmp((const char *)name,&RESERVED_PIN[r][0],GPIO_NAME_LEN) == 0)
    {
      usb_printf(CMD_KO "%s is an reserver pin\n\r",RESERVED_PIN[r]);
      return -1;
    }

    r++;
  }

  return 0;
}


int get_mode(uint8_t modeTxt[GPIO_MODE_LEN],uint32_t *mode)
{
  if(strncmp((const char *)modeTxt,MODE_INPUT,GPIO_MODE_LEN) == 0)
  {
    *mode = GPIO_MODE_INPUT;
    return 0;
  }
  else if(strncmp((const char *)modeTxt,MODE_OUTPUT_PP,GPIO_MODE_LEN) == 0)
  {
    *mode = GPIO_MODE_OUTPUT_PP;
    return 0;
  }
  else if(strncmp((const char *)modeTxt,MODE_OUTPUT_OD,GPIO_MODE_LEN) == 0)
  {
    *mode = GPIO_MODE_OUTPUT_OD;
    return 0;
  }

  usb_printf(CMD_KO "unknown mode : %s\n\r",modeTxt);
  return -1;
}

int get_pull(uint8_t pullTxt[GPIO_PULL_LEN],uint32_t *pull)
{
  if(strncmp((const char *)pullTxt,NOPULL,GPIO_PULL_LEN) == 0)
  {
    *pull = GPIO_NOPULL;
    return 0;
  }
  else if(strncmp((const char *)pullTxt,PULLUP,GPIO_PULL_LEN) == 0)
  {
    *pull = GPIO_PULLUP;
    return 0;
  }
  else if(strncmp((const char *)pullTxt,PULLDOWN,GPIO_PULL_LEN) == 0)
  {
    *pull = GPIO_PULLDOWN;
    return 0;
  }

  usb_printf(CMD_KO "unknown pull : %s\n\r",pullTxt);
  return -1;
}


int cmd_uid(uint8_t* cmd,uint8_t len)
{
  UNUSED(cmd);
  UNUSED(len);
  char *id = (char *)UID_BASE;
  char txt[12*2+2];
  int i=0;

  for(; i< 12 ; i++)
  {
    sprintf(&txt[i*2],"%02X",*id++);
  }

  txt[i*2]= '\0';
  usb_printf(CMD_OK "value :%s\n\r",txt);
  return 0;
}

int get_config_index(const uint8_t name[4])
{
  uint16_t i=0;

  while(config.gpio[i].name[0] != '\0')
  {
    if(strncmp((const char *)&config.gpio[i].name[0],(const char *)name,4) == 0)
    {
      return i;
    }

    i++;
  }

  return -1;
}

int update_config_value(const uint8_t name[4],uint8_t state)
{
  int index = get_config_index(name);

  if(index <0)
  {
    return -1;
  }

  config.gpio[index].state = state;
  return 0;
}

int update_config_setting(const uint8_t name[4], uint8_t mode,uint8_t pull)
{
  int index = get_config_index(name);

  if(index <0)
  {
    return -1;
  }

  config.gpio[index].mode = mode;
  config.gpio[index].pull = pull;
  return 0;
}


const char *get_mode_txt(uint32_t mode)
{
  switch(mode)
  {
    case GPIO_MODE_INPUT:
      return MODE_INPUT;

    case GPIO_MODE_OUTPUT_PP:
      return MODE_OUTPUT_PP;

    case GPIO_MODE_OUTPUT_OD:
      return MODE_OUTPUT_OD;

    default:
      return "UNK";
  }
}

const char *get_pull_txt(uint32_t pull)
{
  switch(pull)
  {
    case GPIO_NOPULL:
      return NOPULL;

    case GPIO_PULLUP:
      return PULLUP;

    case GPIO_PULLDOWN:
      return PULLDOWN;

    default:
      return "UNK";
  }
}

int cmd_dump_config(uint8_t* cmd,uint8_t len)
{
  UNUSED(cmd);
  UNUSED(len);
  usb_printf("CONFIG SIZE: %d\n\r",sizeof(config));
  usb_printf("SYS NAME: %s\n\r",config.sys_name);
  uint16_t i=0;

  while(config.gpio[i].name[0] != '\0')
  {
    GPIO_TypeDef *port=0;
    uint32_t pin=0 ;
    int32_t value=-1;

    if(get_port_and_pin(config.gpio[i].name,&port,&pin) ==0)
    {
      value = gpio_read_pin(port, pin);
    }

    usb_printf("%s %s %s %d %d\n\r",
               config.gpio[i].name,
               get_mode_txt(config.gpio[i].mode),
               get_pull_txt(config.gpio[i].pull),
               config.gpio[i].state,
               value
              );
    i++;
  }

  usb_printf(CMD_OK "\n\r");
  return 0;
}

#define CONFIGUATION_ADDRESS  (FLASH_BASE_ADDRESS+ 256*1024 - FLASH_PAGE_SIZE)

int load_config()
{
  uint16_t i=0;
  config_st *config_flash  = (config_st *)CONFIGUATION_ADDRESS;

  if(config_flash->sys_name[0] != 0xFF)
  {
    memcpy(&config,(void*)config_flash,sizeof(config));
  }

  GPIO_TypeDef *port=0;
  uint32_t pin=0 ;

  while(config.gpio[i].name[0] != '\0')
  {
    usb_printf("%s %s %s\n\r",config.gpio[i].name, get_mode_txt(config.gpio[i].mode), get_pull_txt(config.gpio[i].pull));

    if(get_port_and_pin(config.gpio[i].name,&port,&pin) == 0)
    {
      if(gpio_init(port,pin,config.gpio[i].mode,config.gpio[i].pull) == 0)
      {
        if(config.gpio[i].mode != GPIO_MODE_INPUT)
        {
          gpio_write_pin(port, pin, config.gpio[i].state);
        }
      }
    }

    i++;
  }

  return 0;
}

int cmd_save_config(uint8_t* cmd,uint8_t len)
{
  UNUSED(cmd);
  UNUSED(len);
  if(memcmp((void*)CONFIGUATION_ADDRESS,(void*)&config,sizeof(config)) == 0)
  {
    usb_printf(CMD_OK "is uptodate \n\r");
    return 0;
  }

  if(flash_Update(CONFIGUATION_ADDRESS, (const uint8_t *)&config, sizeof(config)) != 0)
  {
    usb_printf(CMD_KO "flash Update error\n\r");
    return -1;
  }

  usb_printf(CMD_OK "\n\r");
  return 0;
}


int cmd_init(uint8_t* cmd,uint8_t  len)
{
  UNUSED(len);
  uint8_t* arg= cmd;
  arg+= (GPIO_INIT_NAME_LEN+1);
  GPIO_TypeDef *GPIOx=0;
  uint32_t pin=0;

  if(get_port_and_pin(arg,&GPIOx,&pin) !=0)
    return -1;

  if(is_reserved(arg) !=0)
    return -1;

  uint32_t mode;
  arg+= (GPIO_NAME_LEN+1);

  if(get_mode(arg,&mode) != 0)
    return -1;

  uint32_t pull;
  arg+= (GPIO_MODE_LEN+1);

  if(get_pull(arg,&pull) != 0)
    return -1;

  if(gpio_init(GPIOx,pin,mode,pull) != 0)
  {
    usb_printf(CMD_KO " gpio_init \n\r");
    return-1;
  }

  update_config_setting(&cmd[(GPIO_INIT_NAME_LEN+1)], mode,pull);
  usb_printf(CMD_OK "\n\r");
  return 0;
}


int cmd_set(uint8_t* cmd,uint8_t len)
{
  UNUSED(len);
  uint8_t* arg= cmd;
  arg+= (GPIO_SET_NAME_LEN+1);
  GPIO_TypeDef *GPIOx=0;
  uint32_t pin=0;

  if(get_port_and_pin(arg,&GPIOx,&pin) !=0)
    return -1;

  arg+= (GPIO_NAME_LEN+1);
  uint32_t value =0;

  if(arg[0] == '0')
    value = GPIO_PIN_RESET;
  else if(arg[0] == '1')
    value = GPIO_PIN_SET;
  else
  {
    usb_printf(CMD_KO "unknown value : %s\n\r",arg);
    return -1;
  }

  if(gpio_write_pin(GPIOx, pin, value) != 0)
  {
    usb_printf(CMD_KO " gpio_write_pin \n\r");
    return -1;
  }

  update_config_value(&cmd[(GPIO_SET_NAME_LEN+1)],value);
  usb_printf(CMD_OK "\n\r");
  return 0;
}

int cmd_get(uint8_t* cmd,uint8_t  len)
{
  UNUSED(len);
  uint8_t* arg= cmd;
  uint32_t value;
  arg+= (GPIO_GET_NAME_LEN+1);
  GPIO_TypeDef *GPIOx=0;
  uint32_t pin=0;

  if(get_port_and_pin(arg,&GPIOx,&pin) !=0)
    return -1;

  value = gpio_read_pin(GPIOx, pin);
  usb_printf(CMD_OK "value :%d\n\r",value);
  return 0;
}



int cmd_set_sys_nanme(uint8_t* cmd,uint8_t len)
{
  uint8_t* arg= cmd;
  arg+= (SET_SYS_NAME_LEN+1);
  uint8_t l=len - (SET_SYS_NAME_LEN+1);

  if(l >= (sizeof(config.sys_name) -1))
    l = sizeof(config.sys_name) -1;

  memcpy(config.sys_name,arg,l);
  config.sys_name[l] = '\0';
  usb_printf(CMD_OK "\n\r");
  return 0;
}

int cmd_get_sys_nanme(uint8_t* cmd,uint8_t len)
{
  UNUSED(cmd);
  UNUSED(len);
  usb_printf(CMD_OK "value :%s\n\r",config.sys_name);
  return 0;
}

typedef int (*parsing_function)(uint8_t* cmd,uint8_t  len);

typedef struct
{
  uint8_t len_total;
  uint8_t len_name;
  const char * name;
  parsing_function func;
} cmd_st;


cmd_st cmd_list[] =
{
  { HELP_NAME_LEN       , HELP_NAME_LEN    ,  HELP_NAME         , help },
  { GET_UID_NAME_LEN    , GET_UID_NAME_LEN ,  GET_UID_NAME      , cmd_uid },
  { UNKNOWN_TOTAL_LEN   , SET_SYS_NAME_LEN ,  SET_SYS_NAME_NAME , cmd_set_sys_nanme  },
  { GET_SYS_NAME_LEN    , GET_SYS_NAME_LEN ,  GET_SYS_NAME_NAME , cmd_get_sys_nanme  },

  { SAVE_CONFIG_NAME_LEN, SAVE_CONFIG_NAME_LEN,   SAVE_CONFIG_NAME,   cmd_save_config },
  { DUMP_CONFIG_NAME_LEN, DUMP_CONFIG_NAME_LEN,   DUMP_CONFIG_NAME,   cmd_dump_config },

  { GPIO_INIT_TOTAL_LEN, GPIO_INIT_NAME_LEN,  GPIO_INIT_NAME, cmd_init },
  { GPIO_SET_TOTAL_LEN , GPIO_SET_NAME_LEN ,  GPIO_SET_NAME , cmd_set  },
  { GPIO_GET_TOTAL_LEN , GPIO_GET_NAME_LEN ,  GPIO_GET_NAME , cmd_get  },

  // end if list
  {0 ,0,"", NULL }
};


void cmd_parsing(uint8_t* cmd,uint8_t  len)
{
  if(len >= HELP_NAME_LEN)
  {
    uint16_t index =0;

    while(cmd_list[index].len_total != 0)
    {
      if((len ==  cmd_list[index].len_total  ||  cmd_list[index].len_total == UNKNOWN_TOTAL_LEN) && (strncmp((const char *)cmd,cmd_list[index].name,cmd_list[index].len_name) == 0))
      {
        cmd_list[index].func(cmd,len);
        return;
      }

      index++;
    }
  }

  usb_printf(CMD_KO "Invalid command: %s (%d bytes) try HELP\n\r",cmd,len);
}

void cmd_config()
{
  osMsgQueue = osMessageCreate(osMessageQ(osMsgQueue), NULL);
  memset(usbQueue,0,sizeof(message_t)*USB_QUEUE_SIZE);
}

void new_data_it(uint8_t* buf, uint32_t *len)
{
  for(uint32_t i=0; i< *len  ; i++)
  {
    usbQueue[queueIndex].buffer[usbQueue[queueIndex].len] = buf[i];

    if(usbQueue[queueIndex].buffer[usbQueue[queueIndex].len] == '\r')
    {
      usbQueue[queueIndex].buffer[usbQueue[queueIndex].len] = '\0';
      osMessagePut(osMsgQueue, (uint32_t)&usbQueue[queueIndex], 0);
      queueIndex++;

      if(queueIndex >= (USB_QUEUE_SIZE))
        queueIndex=0;

      usbQueue[queueIndex].len=0;
    }
    else
    {
      if(usbQueue[queueIndex].len < (BUFFER_SIZE-1))
        usbQueue[queueIndex].len++;
    }
  }
}

void cmd_loop()
{
  osDelay(100);
  gpio_init(LED_PORT,LED_PIN,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL);
  load_config();
  uint32_t i=0;
  #ifndef   _DEBUG
  MX_IWDG_Init();
  #endif

  for(;;)
  {
    osEvent evt = osMessageGet(osMsgQueue, OS_WAITE_TIMEOUT);
    #ifndef   _DEBUG
    MX_IWDG_Refresh();
    #endif
    i++;

    if(evt.status != osEventMessage)
    {
      //usb_printf("loop %08d  returned 0x%02x status\n\r",i, evt.status);
    }
    else
    {
      message_t *usbMsg = (message_t *)evt.value.v;
      cmd_parsing(usbMsg->buffer, usbMsg->len);
    }

    if(i%2)
      led_on();
    else
      led_off();
  }
}

