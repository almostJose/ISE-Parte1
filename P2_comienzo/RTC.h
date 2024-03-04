/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTC_H
#define __RTC_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
//#include "stm32f4xx_nucleo_144.h"
#include <stdio.h>
#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "Board_LED.h"                  // ::Board Support:LED

/* Exported types ------------------------------------------------------------*/
extern osThreadId_t tid_RTC;
/* Exported constants --------------------------------------------------------*/
#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* Exported thread functions,  
  Example: extern void app_main (void *arg); */
int Init_RTC (void);

#endif /* __RTC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
