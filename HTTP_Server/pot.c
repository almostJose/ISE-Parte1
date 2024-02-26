#include "pot.h"
#include "stm32f4xx_hal.h"
#include <math.h>

osMessageQueueId_t mid_MsgQueuePOT;

/*--------------------- DEFINICIONES -----------------------*/
static const double RESOLUTION_12B = 4096;
static const double VREF = 3.3;


/*---------------- VARIABLES NO GLOBALES -------------------*/
	/*------------------- TIMER MEDIDAS -------------------*/
osTimerId_t tim_pot; //id del timer para realizar la medida 
	/*------------------- CONVERTIDOR ADC -------------------*/
static ADC_HandleTypeDef adchandle; //Manejador ADC
	/*------------------- HILO POTENCIOMETROS -------------------*/
osThreadId_t tid_Thpot; //id del hilo que gestiona la temperatura 
	/*------------------- TEMPERATURAS POTENCIÓMETROS -------------------*/
static potenciometro medidas;
  

/*----------- DECLARACION FUNCIONES NO GLOBALES -----------*/
	/*------------------- ADC PINS -------------------*/

/**
 * @brief Configura los pines que vamos a utilizar para los potenciómetros y para el convertidor ADC
 *
*/
static void ADC_pins_config(void); // pins configuration


	/*------------------- TIMER MEDIDA -------------------*/

/**
 * @brief Inicializa el timer usado para realizar las medidas de tensión de los potenciómetros
 *
 * @return Devuelve -1 si ha existido algún error en el proceso o 0 si todo ha salido correctamente
*/
static int8_t Init_Timer(void);

/**
 * @brief Callback del timer de medida que se usa para expresar que se debe realizar la medida
 *
*/
static void TimerPOTENCIA_Callback(void *arg);


	/*------------------- CONVERTIDOR ADC -------------------*/
	
/**
 * @brief Configuración del convertidor ADC
 *
 * @return Devuelve -1 si ha existido algún error en el proceso o 0 si todo ha salido correctamente
*/
static int8_t ADC_Init_Single_Conversion(ADC_HandleTypeDef *, ADC_TypeDef  *);

/**
 * @brief Configuración del canal del convertidor ADC
 *
 * @return Devuelve -1 si ha existido algún error en el proceso o 0 si todo ha salido correctamente
*/
static uint32_t ADC_Channel_Config(uint32_t Channel, uint32_t Rank);

/**
 * @brief Lectura de la tensión del potenciómetro 1/2
 *
 * @return Devuelve tensión potenciómetro 1/2
*/
static double ADC_getVoltage(void); 


	/*------------------- HILO POTENCIOMETROS -------------------*/
	
/**
 * @brief Hilo de los potenciometros donde se comienza el timer, se realizan las medidas de la tensión y se guardan las temperaturas en la cola
 *
*/
__NO_RETURN static void Thpot (void *argument);


	/*------------------- CONVERSIÓN TENSIÓN-TEMPERATURA -------------------*/
	
/**
 * @brief Convierte la tensión proporcionada por los potenciometros: 0-3.3 V, en un rango de temperaturas: 5-30ºC	
 *
 * @return Devuelve la temperatura asociada a una determinada tensión
*/
static double conversionTemperatura(double tension);


/*------------------- CODIGO FUNCIONES -------------------*/

	/*------------------- ADC PINS -------------------*/
static void ADC_pins_config() {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
	//Habilitamos el reloj del ADC1
	__HAL_RCC_ADC1_CLK_ENABLE();
	
	//Configuramos el pin C0 a partir del cual mediremos la temperatura de referencia (POT 2)
	__HAL_RCC_GPIOC_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
	
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	//Configuramos el pin A3 a partir del cual mediremos la temperatura (POT 1)
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


	/*------------------- TIMER MEDIDAS -------------------*/

static int8_t Init_Timer(void){	
	// Creamos el timer para realizar la medida
  tim_pot = osTimerNew((osTimerFunc_t)&TimerPOTENCIA_Callback, osTimerPeriodic, NULL, NULL);
	
	//Comprobamos que el timer se ha creado correctamente
  if (tim_pot == NULL) { 
    return -1;
  } 
	
	return 0;
}

static void TimerPOTENCIA_Callback(void *arg){
	//Procedemos a realizar la medida de la temperatura
	osThreadFlagsSet(tid_Thpot, S_POT);
}


	/*------------------- CONVERTIDOR ADC -------------------*/

static int8_t ADC_Init_Single_Conversion(ADC_HandleTypeDef *hadc, ADC_TypeDef  *ADC_Instance)
{
	//Configuramos el ADC
  hadc->Instance = ADC_Instance; //Asociamos el controlador ADC con una instancia de este
  hadc->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2; //Establecemos el reloj del prescaler en modo sincrono y con prescaler 2
  hadc->Init.Resolution = ADC_RESOLUTION_12B; //Resolucion del ADC de 12 bits
  hadc->Init.ScanConvMode = DISABLE; //Realizaremos conversiones del canal configurado en cada momento
  hadc->Init.ContinuousConvMode = DISABLE; //Deshabilitamos el modo de conversion continua
  hadc->Init.DiscontinuousConvMode = DISABLE; //Deshabilitamos el modo de conversion discontinua
  hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; //El trigger no será externo
  hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START; //El trigger será iniciado por software
  hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT; //Alineacion de los datos del ADC a la derecha
  hadc->Init.NbrOfConversion = 1; //Solo se realizará una conversión de un canal
  hadc->Init.DMAContinuousRequests = DISABLE; //Deshabilitamos las solicitudes DMA continuas para el ADC
	hadc->Init.EOCSelection = ADC_EOC_SINGLE_CONV; //Se generará una interrupción al final de las conversiones
	
		//Comprobamos si el ADC se inicia correctamente
	if (HAL_ADC_Init(hadc) != HAL_OK){
		return -1;
	}
	
	return 0;
}

static uint32_t ADC_Channel_Config(uint32_t Channel, uint32_t Rank){ 
	
	static ADC_ChannelConfTypeDef sConfig = {0}; //Configuracion del canal del ADC1
	
	//Configuramos el canal
	sConfig.Channel = Channel; //Establecemos el canal ADC a configurar
  sConfig.Rank = Rank; //Indicamos la posición del canal en la secuencia de conversion
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES; //Tiempo de muestreo del ADC en este canal
	
	//Establecemos la configuracion
	return ( HAL_ADC_ConfigChannel(&adchandle, &sConfig) );
}

static double ADC_getVoltage(void){
	
	uint32_t raw = 0; //valor obtenido de la conversión del ADC
	
	//Iniciamos la conversión de la señal que llega al canal ADC
	HAL_ADC_Start(&adchandle);
	
	//Esperamos hasta que se complete una conversión en el ADC
	while( HAL_ADC_PollForConversion(&adchandle, 0)!= HAL_OK){}
	
	//Obtenemos el valor de la conversión
	raw = HAL_ADC_GetValue(&adchandle);
		
	HAL_ADC_Stop(&adchandle);
	
	//Convertimos el valor obtenido a tensión
	return ( (raw*VREF)/RESOLUTION_12B);
}


	/*------------------- INICIALIZACIÓN HILO POTENCIÓMETROS Y TIMER -------------------*/

int8_t Init_Thpot (void) {
	
	int8_t status; //estado de las funciones para comprobar si se ha producido algun error
	
	mid_MsgQueuePOT = osMessageQueueNew(1, sizeof(potenciometro), NULL);
	
	//Creamos los timers que emplearemos
	status = Init_Timer();
		//Comprobamos que se ha creado correctamente
	if( status == -1 ){
		return (-1);
	}
	
	//Configuramos los pines del ADC
	ADC_pins_config();
	
	//Configuramos el ADC
	if( ADC_Init_Single_Conversion(&adchandle , ADC1) != 0){
		return -1;
	}
	
	//Creamos el hilo de los potenciometros
  tid_Thpot = osThreadNew(Thpot, NULL, NULL);
  if (tid_Thpot == NULL) {
    return(-1);
  }
 
  return(0);
}


	/*------------------- HILO POTENCIÓMETROS -------------------*/

__NO_RETURN static void Thpot(void *argument) {
	
	while (1) {
		//Esperamos a poder realizar las medidas
		osThreadFlagsWait(S_POT, osFlagsWaitAny, osWaitForever);
		
		//Medimos la temperatura de referencia por medio del canal 10
			//Configuramos el canal 10
		ADC_Channel_Config(10,1);
			//Medimos la temperatura de referencia
		medidas.temperaturaREF = round( conversionTemperatura( ADC_getVoltage() ) * 2.0) / 2.0;
		
		//Medimos la temperatura por medio del canal 3
			//Configuramos el canal 3
		ADC_Channel_Config(3,1);
			//Medimos la temperatura
		medidas.temperaturaMED = round( conversionTemperatura( ADC_getVoltage() ) * 2.0 ) / 2.0;
			
		//Metemos las temperaturas en la cola de mensajes
		osMessageQueuePut(mid_MsgQueuePOT, &medidas, NULL, osWaitForever);
		
		osThreadYield(); //Suspendemos el hilo
	}
}


	/*------------------- CONVERSIÓN TENSIÓN-TEMPERATURA -------------------*/

static double conversionTemperatura(double tension){
	//Convertimos el voltaje de 0-3.3 V en temperatura 5-30 ºC 
	return ( tension*(250/33) + 5 );
}

	/*-------------------ISE-------------------------*/

int8_t configAD(void)
{
	ADC_pins_config();
	if( ADC_Init_Single_Conversion(&adchandle , ADC1) != 0){
		return -1;
	}
}

int32_t tomar_medida(void)
{
	ADC_Channel_Config(10,1);

	return ((int32_t)round( conversionTemperatura( ADC_getVoltage() ) * 2.0 ) / 2.0);
}
