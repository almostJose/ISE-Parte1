#ifdef _RTE_
	#include "RTE_Components.h"             // Componentes
#endif
#ifdef RTE_CMSIS_RTOS2                  
	#include "cmsis_os2.h"                  // CMSIS:RTOS2
#endif

#ifndef _POT_H
	#define _POT_H

	/*------------------------- DEFINICIONES --------------------------*/	
		#define S_POT 0x00000001U //flag para realizar las medidas de tensión de los potenciómetros 
		
	/*------------------------- DEFINICIONES --------------------------*/	
	typedef struct{ double temperaturaREF; double temperaturaMED; }potenciometro;
	
	/*---------------------- VARIABLES EXTERNAS -----------------------*/
		extern osTimerId_t tim_pot; //id del timer para realizar la medida 	
		//extern osMessageQueueId_t mid_MsgQueuePOT; //cola de mensajes donde guardaremos la temperatura de referencia y medida
	
	/*---------------- DECLARACION FUNCIONES GLOBALES -----------------*/
		/**
			* @brief Crea el hilo de los potenciómetros, instancia el timer para medir la tensión de cada uno de estos e inicializa el convertidor ADC
			*
		*/
		int8_t Init_Thpot (void);	
	
		int8_t configAD(void);
	
		int32_t tomar_medida(void);
		
#endif
