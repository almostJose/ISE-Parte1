#include "main.h"
#include "RTC.h"
#include "lcd.h"

/*variables privadas--------------------------------------*/
/* RTC handler declaration */
static RTC_HandleTypeDef RtcHandle;
static RTC_DateTypeDef sdatestructureget;
static RTC_TimeTypeDef stimestructureget;

osThreadId_t tid_alarma;

osTimerId_t periodic_id;

static void periodic_Callback (void *argument);
void ThAlarma (void *argument);

void RTC_Config(void)
{
	/*##-1- Configure the RTC peripheral #######################################*/
  /* Configure RTC prescaler and RTC data registers */
  /* RTC configured as follows:
      - Hour Format    = Format 24
      - Asynch Prediv  = Value according to source clock
      - Synch Prediv   = Value according to source clock
      - OutPut         = Output Disable
      - OutPutPolarity = High Polarity
      - OutPutType     = Open Drain */ 
  RtcHandle.Instance = RTC; 
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  __HAL_RTC_RESET_HANDLE_STATE(&RtcHandle);
  if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
	
	/*##-2- Check if Data stored in BackUp register1: No Need to reconfigure RTC#*/
  /* Read the Back Up Register 1 Data */
  if (HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR1) != 0x32F2)
  {
    /* Configure RTC Calendar */
    RTC_CalendarConfig();
  }
  else
  {
    /* Check if the Power On Reset flag is set */
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
    {
      /* Turn on LED2: Power on reset occurred */
      //BSP_LED_On(LED2);
    }
    /* Check if Pin Reset flag is set */
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
    {
      /* Turn on LED1: External reset occurred */
      //BSP_LED_On(LED1);
    }
    /* Clear source Reset Flag */
    __HAL_RCC_CLEAR_RESET_FLAGS();
  }
}

/**
  * @brief  Configure the current time and date.
  * @param  None
  * @retval None
  */
void RTC_CalendarConfig(void)
{
  RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;

  /*##-1- Configure the Date #################################################*/
  /* Set Date: Tuesday February 18th 2014 */
  sdatestructure.Year = 0x02;
  sdatestructure.Month = RTC_MONTH_OCTOBER;
  sdatestructure.Date = 0x16;
  sdatestructure.WeekDay = RTC_WEEKDAY_WEDNESDAY;
  
  if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-2- Configure the Time #################################################*/
  /* Set Time: 02:00:00 */
  stimestructure.Hours = 0x23;
  stimestructure.Minutes = 0x59;
  stimestructure.Seconds = 0x40;
  stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

  if (HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
}

/**
  * @brief  Display the current time and date.
  * @param  showtime : pointer to buffer
  * @param  showdate : pointer to buffer
  * @retval None
  */
void RTC_CalendarShow(uint8_t *showtime, uint8_t *showdate)
{
//  RTC_DateTypeDef sdatestructureget;
//  RTC_TimeTypeDef stimestructureget;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
  /* Display time Format : hh:mm:ss */
  sprintf((char *)showtime, "%2d:%2d:%2d", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
  /* Display date Format : mm-dd-yy */
  sprintf((char *)showdate, "%2d-%2d-%2d", sdatestructureget.Month, sdatestructureget.Date, 2000 + sdatestructureget.Year);
}

void RTC_Alarm_Config(void)
{
//	RTC_DateTypeDef sdatestructureget;
//  RTC_TimeTypeDef stimestructureget;
	RTC_AlarmTypeDef def_alarm;
	
	def_alarm.AlarmTime.Hours = stimestructureget.Hours;
	def_alarm.AlarmTime.Minutes = stimestructureget.Minutes;
	def_alarm.AlarmTime.Seconds = 30;
	def_alarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	def_alarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	def_alarm.Alarm = RTC_ALARM_A;
	def_alarm.AlarmDateWeekDay = 1;
	def_alarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	def_alarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY | RTC_ALARMMASK_HOURS | RTC_ALARMMASK_MINUTES;
	//def_alarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	
	HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
	
	HAL_RTC_SetAlarm_IT(&RtcHandle, &def_alarm, RTC_FORMAT_BIN);
	
	tid_alarma = osThreadNew(ThAlarma, NULL, NULL);
  periodic_id = osTimerNew(periodic_Callback, osTimerPeriodic, NULL, NULL);
}

void timer_config()
{
}

void RTC_Alarm_IRQHandler(void)
{
	HAL_RTC_AlarmIRQHandler(&RtcHandle);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	osThreadFlagsSet(tid_alarma, 0x01);
}

static void periodic_Callback (void *argument) {
  osThreadFlagsSet(tid_alarma, 0x02);
}

void ThAlarma (void *argument) {
	uint8_t cnt;
	uint8_t P2 = 0;
	
  while (1) {
		cnt = 0;
		
		osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);
		osTimerStart(periodic_id, 1000U);
		while(cnt < 6)
		{
			osThreadFlagsWait(0x02, osFlagsWaitAny, osWaitForever);
			P2 = (P2 == 1) ? 0 : 1;
			LED_SetOut(P2);
			cnt++;
			if(cnt == 6)
				osTimerStop(periodic_id);
		}
		
    osThreadYield();                            // suspend thread
  }
}
