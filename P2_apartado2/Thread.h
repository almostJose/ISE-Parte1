/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __THREAD_H
#define __THREAD_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
//#include "stm32f4xx_nucleo_144.h"
#include <stdio.h>
#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "Board_LED.h"                  // ::Board Support:LED
#include "RTC.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* Exported thread functions,  
  Example: extern void app_main (void *arg); */
int Init_Thread (void);

#endif /* __THREAD_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/