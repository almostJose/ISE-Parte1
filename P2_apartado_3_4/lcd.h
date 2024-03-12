#ifndef _LCD_H
  #define _LCD_H
	#include "stm32f4xx_hal.h"
	#include "cmsis_os2.h"
	
	typedef struct {                                // object data type
		uint8_t linea;
		unsigned char inf[256];
	} MSGQUEUE_OBJ_t;
	
	extern osMessageQueueId_t mid_MsgQueue;                // message queue id
	
/*--------FUNCIONES--------*/		
		/**
			* @brief Crea el hilo del timer
			*
		*/
    int Init_LCD (void); 

#endif
