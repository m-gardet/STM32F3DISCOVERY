bin_PROGRAMS = stm32.elf

AUTOMAKE_OPTIONS = subdir-objects

stm32_elf_SOURCES  = \
  Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c \
  Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c \
  Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c \
  Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
  \
  Middlewares/Third_Party/FreeRTOS/Source/croutine.c \
  Middlewares/Third_Party/FreeRTOS/Source/queue.c \
  Middlewares/Third_Party/FreeRTOS/Source/timers.c \
  Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c \
  Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c \
  Middlewares/Third_Party/FreeRTOS/Source/event_groups.c \
  Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c \
  Middlewares/Third_Party/FreeRTOS/Source/list.c \
  Middlewares/Third_Party/FreeRTOS/Source/tasks.c \
  \
  Drivers/CMSIS/Device/ST/STM32F3xx/Source/Templates/system_stm32f3xx.c \
  Drivers/CMSIS/Device/ST/STM32F3xx/Source/Templates/gcc/startup_stm32f303xc.S \
  \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_pcd_ex.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_rcc.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_pwr_ex.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_tim_ex.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_pcd.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_cortex.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_adc_ex.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_pwr.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_dma.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_gpio.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_i2c.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_flash.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_dac_ex.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_iwdg.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_crc.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_tim.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_wwdg.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_flash_ex.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_crc_ex.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_dac.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_rcc_ex.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_i2c_ex.c \
  Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_adc.c \
  \
  Src/usbd_conf.c \
  Src/flash.c \
  Src/gpio.c \
  Src/stm32f3xx_hal_timebase_TIM.c \
  Src/usbd_cdc_if.c \
  Src/i2c.c \
  Src/freertos.c \
  Src/iwdg.c \
  Src/dac.c \
  Src/command.c \
  Src/adc.c \
  Src/wwdg.c \
  Src/main.c \
  Src/usb_device.c \
  Src/usbd_desc.c \
  Src/stm32f3xx_it.c \
  Src/stm32f3xx_hal_msp.c \
  Src/crc.c

CPU_ARG=-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16

stm32_elf_CFLAGS = \
  $(CPU_ARG) \
  -Wl,--gc-sections \
  -g3 \
  -Os \
  -std=gnu99 \
  -fmessage-length=0 \
  -fsigned-char \
  -ffunction-sections \
  -fdata-sections \
  -ffreestanding \
  -Wall \
  -Wextra  \
  -DSTM32F303xC \
  -D_DEBUG \
  -DUSE_FULL_ASSERT \
  -I$(srcdir)/Inc \
  -I$(srcdir)/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc \
  -I$(srcdir)/Middlewares/ST/STM32_USB_Device_Library/Core/Inc \
  -I$(srcdir)/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS \
  -I$(srcdir)/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F \
  -I$(srcdir)/Middlewares/Third_Party/FreeRTOS/Source/include \
  -I$(srcdir)/Drivers/CMSIS/Include \
  -I$(srcdir)/Drivers/CMSIS/Device/ST/STM32F3xx/Include \
  -I$(srcdir)/Drivers/STM32F3xx_HAL_Driver/Inc \
  \
  -I$(srcdir)/src

stm32_elf_CCAS = $(host)-gcc

stm32_elf_CCASFLAGS = $(stm32_elf_CFLAGS)

stm32_elf_LDFLAGS = \
  $(CPU_ARG) \
  -Wl,-Map=$(bin_PROGRAMS).map \
  -fmessage-length=0 \
  -fsigned-char \
  -ffunction-sections \
  -fdata-sections \
  -ffreestanding \
  -Wall \
  -g3 \
  -Wextra \
  -specs=nosys.specs \
  -Wl,--gc-sections \
  -L$(srcdir) \
  -T STM32F303VC_FLASH.ld

all:
	$(host)-size --format=berkeley $(builddir)/$(bin_PROGRAMS)

.PHONY: all
.SILENT: all
