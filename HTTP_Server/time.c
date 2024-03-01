#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "main.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
uint8_t aShowTime[50] = {0};
uint8_t aShowDate[50] = {0};
 
osThreadId_t tid_Thread;                        // thread id
 
void Thread (void *argument);                   // thread function
 
int Init_Thread (void) {
 
  tid_Thread = osThreadNew(Thread, NULL, NULL);
  if (tid_Thread == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread (void *argument) {
 
  while (1) {
    RTC_CalendarShow(aShowTime, aShowDate);
    
    osThreadYield();                            // suspend thread
  }
}
