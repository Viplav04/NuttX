/****************************************************************************
 * configs/olimex-stm32-e407/src/olimex-stm32-e407.h
 *
 *   Copyright (C) 2014 Max Holtzberg. All rights reserved.
 *   Author: Max Holtzberg <mholtzberg@uvc-ingenieure.de>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ifndef __CONFIGS_OLIMEX_STM32_E407_SRC_H
#define __CONFIGS_OLIMEX_STM32_E407_SRC_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/compiler.h>
#include <stdint.h>
#include <arch/stm32/chip.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Configuration from stm32f4discovery */

/* Assume that we have everything */

#define HAVE_USBDEV     1
#define HAVE_USBHOST    1
#define HAVE_USBMONITOR 1
#define HAVE_SDIO       1
#define HAVE_RTC_DRIVER 1
#define HAVE_NETMONITOR 1

/* Can't support USB host or device features if USB OTG FS is not enabled */

#ifndef CONFIG_STM32_OTGFS
#  undef HAVE_USBDEV
#  undef HAVE_USBHOST
#  undef HAVE_USBMONITOR
#endif

/* Can't support USB device monitor if USB device is not enabled */

#ifndef CONFIG_USBDEV
#  undef HAVE_USBDEV
#  undef HAVE_USBMONITOR
#endif

/* Can't support USB host is USB host is not enabled */

#ifndef CONFIG_USBHOST
#  undef HAVE_USBHOST
#endif

/* Check if we should enable the USB monitor before starting NSH */

#if !defined(CONFIG_USBDEV_TRACE) || !defined(CONFIG_USBMONITOR)
#  undef HAVE_USBMONITOR
#endif

/* NSH Network monitor  */

#if !defined(CONFIG_NET) || !defined(CONFIG_STM32_EMACMAC)
#  undef HAVE_NETMONITOR
#endif

#if !defined(CONFIG_NSH_NETINIT_THREAD) || !defined(CONFIG_ARCH_PHY_INTERRUPT) || \
    !defined(CONFIG_NETDEV_PHY_IOCTL) || !defined(CONFIG_NET_UDP) || \
     defined(CONFIG_DISABLE_SIGNALS)
#  undef HAVE_NETMONITOR
#endif

/* Can't support MMC/SD features if mountpoints are disabled or if SDIO support
 * is not enabled.  Can't support MMC/SD features if the upper half MMC/SD SDIO
 * driver is not enabled.
 */

#if defined(CONFIG_DISABLE_MOUNTPOINT) || !defined(CONFIG_STM32_SDIO)
#  undef HAVE_SDIO
#endif

#if !defined(CONFIG_MMCSD_SDIO)
#  undef HAVE_SDIO
#endif

#undef  SDIO_MINOR     /* Any minor number, default 0 */
#define SDIO_SLOTNO 0  /* Only one slot */

#ifdef HAVE_SDIO

#  if defined(CONFIG_NSH_MMCSDSLOTNO) && CONFIG_NSH_MMCSDSLOTNO != 0
#    warning Only one MMC/SD slot, slot 0
#    define CONFIG_NSH_MMCSDSLOTNO SDIO_SLOTNO
#  endif

#  if defined(CONFIG_NSH_MMCSDMINOR)
#    define SDIO_MINOR CONFIG_NSH_MMCSDMINOR
#  else
#    define SDIO_MINOR 0
#  endif

  /* SD card bringup does not work if performed on the IDLE thread because it
   * will cause waiting.  Use either:
   *
   *  CONFIG_LIB_BOARDCTL=y, OR
   *  CONFIG_BOARD_LATE_INITIALIZE=y && CONFIG_BOARD_INITTHREAD=y
   */

#  if defined(CONFIG_BOARD_LATE_INITIALIZE) && !defined(CONFIG_LIB_BOARDCTL) && \
     !defined(CONFIG_BOARD_INITTHREAD)
#    warning SDIO initialization cannot be perfomed on the IDLE thread
#    undef HAVE_SDIO
#  endif
#endif

/* procfs File System */

#ifdef CONFIG_FS_PROCFS
#  ifdef CONFIG_NSH_PROC_MOUNTPOINT
#    define STM32_PROCFS_MOUNTPOINT CONFIG_NSH_PROC_MOUNTPOINT
#  else
#    define STM32_PROCFS_MOUNTPOINT "/proc"
#  endif
#endif

/* Olimex-STM32-E407 GPIOs ****************************************************/
/* LEDs */

#define GPIO_LED_STATUS (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|\
                         GPIO_OUTPUT_CLEAR|GPIO_PORTC|GPIO_PIN13)

/* BUTTONS -- NOTE that all have EXTI interrupts configured */

#define MIN_IRQBUTTON   BUTTON_BUT
#define MAX_IRQBUTTON   BUTTON_BUT
#define NUM_IRQBUTTONS  1

#define GPIO_BTN_BUT   (GPIO_INPUT|GPIO_FLOAT|GPIO_EXTI|GPIO_PORTA|GPIO_PIN0)

/* USB OTG FS - USB-A connector
 *
 * PC4  OTG_FS_VBUS VBUS sensing
 * PB0  OTG_FS_PowerSwitchOn
 * PB1  OTG_FS_Overcurrent
 */

#define GPIO_OTGFS_VBUS   (GPIO_INPUT|GPIO_FLOAT|GPIO_SPEED_100MHz|\
                           GPIO_OPENDRAIN|GPIO_PORTA|GPIO_PIN9)
#define GPIO_OTGFS_PWRON  (GPIO_OUTPUT|GPIO_FLOAT|GPIO_SPEED_100MHz|\
                           GPIO_PUSHPULL|GPIO_PORTB|GPIO_PIN0)

#ifdef CONFIG_USBHOST
#  define GPIO_OTGFS_OVER (GPIO_INPUT|GPIO_EXTI|GPIO_FLOAT|\
                           GPIO_SPEED_100MHz|GPIO_PUSHPULL|\
                           GPIO_PORTB|GPIO_PIN1)

#else
#  define GPIO_OTGFS_OVER (GPIO_INPUT|GPIO_FLOAT|GPIO_SPEED_100MHz|\
                           GPIO_PUSHPULL|GPIO_PORTB|GPIO_PIN1)
#endif

/* USB OTG HS - miniUSB connector
 *
 * PB13  OTG_HS_VBUS VBUS sensing (also connected to the green LED)
 * PA8  OTG_HS_PowerSwitchOn
 * PF11  OTG_HS_Overcurrent
 */

#define GPIO_OTGHS_VBUS (GPIO_INPUT|GPIO_FLOAT|GPIO_SPEED_100MHz|GPIO_OPENDRAIN|GPIO_PORTB|GPIO_PIN13)
#define GPIO_OTGHS_PWRON (GPIO_OUTPUT|GPIO_OUTPUT_SET|GPIO_FLOAT|GPIO_SPEED_100MHz|GPIO_PUSHPULL|GPIO_PORTA|GPIO_PIN8)

#ifdef CONFIG_USBHOST
#  define GPIO_OTGHS_OVER  (GPIO_INPUT|GPIO_EXTI|GPIO_FLOAT|GPIO_SPEED_100MHz|GPIO_PUSHPULL|GPIO_PORTF|GPIO_PIN11)

#else
#  define GPIO_OTGHS_OVER  (GPIO_INPUT|GPIO_FLOAT|GPIO_SPEED_100MHz|GPIO_PUSHPULL|GPIO_PORTF|GPIO_PIN11)
#endif


/*MRF24J40*/
#define GPIO_MRF24J40_INT   (GPIO_INPUT|GPIO_FLOAT|\
                            GPIO_EXTI|GPIO_PORTG|GPIO_PIN12)/*PG12/D8*/
#define GPIO_MRF24J40_RST  (GPIO_OUTPUT|GPIO_OPENDRAIN|GPIO_SPEED_50MHz|\
                            GPIO_OUTPUT_CLEAR|GPIO_PORTG|GPIO_PIN15)/*PG15/D9*/


/* PWN Configuration */

#define OLIMEXSTM32E407_PWMTIMER   1
#define OLIMEXSTM32E407_PWMCHANNEL 3



/* LAN8710 works with LAN8720 driver
 *
 * ---------- ------------- ------------
 * PIO        SIGNAL        Comments
 * ---------- ------------- ------------
 * PG11       TXEN
 * PG13       TXD0
 * PG14       TXD1
 * PC4        RXD0/MODE0
 * PC5        RXD1/MODE1
 * PA7        CRS_DIV/MODE2
 * PA2        MDIO
 * PC1        MDC
 * PA3        NINT/REFCLK0
 * PG6        NRST
 * ---------- ------------- ------------
 */

#if defined(CONFIG_STM32_ETHMAC)
#  define GPIO_EMAC_NINT  (GPIO_INPUT|GPIO_PULLUP|GPIO_EXTI|    \
                         GPIO_PORTA|GPIO_PIN3)
#  define GPIO_EMAC_NRST  (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|    \
                         GPIO_OUTPUT_SET|GPIO_PORTG|GPIO_PIN6)
#endif

/****************************************************************************
 * Public Types
 ****************************************************************************/

/****************************************************************************
 * Public data
 ****************************************************************************/

#ifndef __ASSEMBLY__

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_usbinitialize
 *
 * Description:
 *   Called from stm32_usbinitialize very early in initialization to setup
 *   USB-related GPIO pins for the Olimex-STM32-H405 board.
 *
 ****************************************************************************/

#if defined(CONFIG_STM32_OTGFS) || defined(CONFIG_STM32_OTGHS)
void weak_function stm32_usbinitialize(void);
#endif

/************************************************************************************
 * Name: stm32_adc_setup
 *
 * Description:
 *   Initialize ADC and register the ADC driver.
 *
 ************************************************************************************/

#ifdef CONFIG_ADC
int stm32_adc_setup(void);
#endif

/****************************************************************************
 * Name: stm32_sdio_initialize
 *
 * Description:
 *   Initialize SDIO-based MMC/SD card support
 *
 ****************************************************************************/

#if !defined(CONFIG_DISABLE_MOUNTPOINT) && defined(CONFIG_STM32_SDIO)
int stm32_sdio_initialize(void);
#endif

/****************************************************************************
 * Name: stm32_can_setup
 *
 * Description:
 *  Initialize CAN and register the CAN device
 *
 ****************************************************************************/

#ifdef CONFIG_CAN
int stm32_can_setup(void);
#endif

/************************************************************************************
 * Name: stm32_mfrc522initialize
 *
 * Description:
 *   Function used to initialize the MFRC522 RFID Transceiver
 *
 ************************************************************************************/

#ifdef CONFIG_CL_MFRC522
int stm32_mfrc522initialize(FAR const char *devpath);
#endif

/****************************************************************************
 * Name: stm32_bmp180initialize
 *
 * Description:
 *  Initialize I2C and register BMP180 sensor
 *
 ****************************************************************************/

#ifdef CONFIG_SENSORS_BMP180
int stm32_bmp180initialize(FAR const char *devpath);
#endif

/****************************************************************************
 * Name: stm32_hih6130initialize
 *
 * Description:
 *  Initialize I2C and register HIH-6130 sensor
 *
 ****************************************************************************/


#ifdef CONFIG_SENSORS_HIH6130
int stm32_hih6130initialize(FAR const char *devpath);
#endif

/****************************************************************************
 * Name: stm32_ina219initialize
 *
 * Description:
 *  Initialize I2C and register INA219 sensor
 *
 ****************************************************************************/


#ifdef CONFIG_SENSORS_HIH6130
int stm32_ina219initialize(FAR const char *devpath);
#endif

/************************************************************************************
 * Name: stm32_pwm_setup
 *
 * Description:
 *   Initialize PWM and register the PWM device.
 *
 ************************************************************************************/

#ifdef CONFIG_PWM
int stm32_pwm_setup(void);
#endif

/************************************************************************************
 * Name: stm32_tone_setup
 *
 * Description:
 *   Function used to initialize a PWM and Oneshot timers to Audio Tone Generator.
 *
 ************************************************************************************/

#ifdef CONFIG_AUDIO_TONE
int stm32_tone_setup(void);
#endif


#endif  /* __ASSEMBLY__ */
#endif /* __CONFIGS_OLIMEX_STM32_E407_SRC_INTERNAL_H */
