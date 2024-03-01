#ifdef _RTE_
	#include "RTE_Components.h"             // Componentes
#endif
#ifdef RTE_CMSIS_RTOS2                  
	#include "cmsis_os2.h"                  // CMSIS:RTOS2
#endif

#ifndef _TIME_H
	#define _TIME_H

  void Init_Thread();
		
#endif