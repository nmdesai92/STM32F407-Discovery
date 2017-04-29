/*This program creates three threads and
  shows synchronization among them
	Thread1 : Blinks red LED  
	Thread2 : Blinks blue LED
	Thread3 : Blinks orange LED
	
	Thread1 and Thread2 runs exclusively, i.e. once they created, they
	grab resourses using semaphore,finishes execution compeletly and 
	release resources.
	Thread3 runs independenly and does not lock resources.*/


/*--------------------Include files-----------------------------------*/

#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include "Board_LED.h"


#define BLUE  3
#define RED   2
#define ORANGE 1
extern uint32_t os_time;
uint32_t HAL_GetTick(void)
{
	return os_time;
}


/*-----------------------Private Functions--------------------------*/
void led_red(const void *argument);
void led_blue(const void *argument);
void led_independent(const void *argument);
/*-----------------------Thread Definitions--------------------------*/

osThreadDef(led_red, osPriorityNormal, 1, 0);
osThreadDef(led_blue, osPriorityNormal, 1, 0);
osThreadDef(led_independent, osPriorityNormal, 1, 0);
osThreadId led1_ID, led2_ID, led3_ID;

osSemaphoreDef(sem1);
osSemaphoreId sem1;

/*-------------------------main()-------------------------------------*/
int main()
{
		osKernelInitialize();
		
		LED_Initialize();
		
		sem1 = osSemaphoreCreate(osSemaphore(sem1),1);					//Create Semaphore
		led1_ID = osThreadCreate(osThread(led_red), NULL);
		led2_ID = osThreadCreate(osThread(led_blue), NULL);
		led3_ID = osThreadCreate(osThread(led_independent), NULL);
		osKernelStart();							//Start thread execution
		
}

/*-------------------------function Definitions-------------------------------------*/
void led_red(const void *argument)
{
	for(;;)
	{
		osSemaphoreWait(sem1,osWaitForever);	//Grab Semaphore
		LED_On(RED);				
		HAL_Delay(3000);								//3 sec on
		LED_Off(RED);
		HAL_Delay(1000);								//1 sec off
		osSemaphoreRelease(sem1);							//Release semaphore after finishing task	
	}
}

void led_blue(const void *argument)
{
	for(;;)
	{
		osSemaphoreWait(sem1,osWaitForever);		//Grab Semaphore
		LED_On(BLUE);
		HAL_Delay(2000);								//2 sec on
		LED_Off(BLUE);
		HAL_Delay(4000);								//4 sec off
		osSemaphoreRelease(sem1);								//Release semaphore after finishing task	
	}
}

void led_independent(const void *argument)
{
	for(;;)
	{
		LED_On(ORANGE);
		HAL_Delay(100);								//100 msec on
		LED_Off(ORANGE);
		HAL_Delay(100);								//100 msec off
	}
}
