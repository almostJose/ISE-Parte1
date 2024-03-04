#include "Thread.h"
#include "lcd.h"
 
osThreadId_t tid_Thread;                        // thread id
 
static uint8_t aShowTime[50] = {0};
static uint8_t aShowDate[50] = {0};
 
void Thread (void *argument);                   // thread function
void ThAlarma (void *argument);
 
int Init_Thread (void) {
 
  tid_Thread = osThreadNew(Thread, NULL, NULL);
  if (tid_Thread == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread (void *argument) {
  MSGQUEUE_OBJ_t msg;
	
	LED_Initialize();
	RTC_Config();
	
	RTC_CalendarShow(aShowTime, aShowDate);
	RTC_Alarm_Config();
	
  while (1) {
		/*##-3- Display the updated Time and Date ################################*/
    RTC_CalendarShow(aShowTime, aShowDate);
		
		sprintf(msg.inf, aShowTime);
    msg.linea = 1;
    osMessageQueuePut(mid_MsgQueue, &msg, 0U, 0U);
		
		sprintf(msg.inf, aShowDate);
    msg.linea = 2;
    osMessageQueuePut(mid_MsgQueue, &msg, 0U, 0U);
		
    osThreadYield();                            // suspend thread
  }
}
