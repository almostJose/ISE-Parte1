/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTC_H
#define __RTC_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
//#include "stm32f4xx_nucleo_144.h"
#include <stdio.h>
#include "Board_LED.h"                  // ::Board Support:LED

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void RTC_Config(void);
void RTC_CalendarConfig(void);
void RTC_CalendarShow(uint8_t *showtime, uint8_t *showdate);
void RTC_Alarm_Config(void);
void get_time(void);

#endif /* __RTC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
