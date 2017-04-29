
/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"
#include "Board_LED.h"
#include "stm32f4xx_hal.h"

#define ORANGE	1
#define RED 		2
#define BLUE	  3
#define GREEN 	4

extern uint32_t os_time;
uint32_t HAL_GetTick(void)
{
	return os_time;
}

int thread_count = 0;
int hyperperiod;


typedef struct pr_TCB{
	uint16_t TCB_id;
	uint32_t arrival;
	uint32_t exec;
	uint32_t period;
}prTCB;

typedef struct prThread_paras{
	os_pthread pthread;
	uint32_t 	 arrival;
	uint32_t 	 exec;
	uint32_t	 period;
}prThreadPara;

prTCB *TCB_task[5];

#define osThreadPeriodicDef(name,arrival,execution,period)  \
const prThreadPara os_prThread_def_##name = \
{ (name), (arrival), (execution), (period)  }

#define osprThread(name)  \
&os_prThread_def_##name


/*--------------------------------------Private Functions---------------------------------------------------*/

void SystemClock_Config(void);
void led_Thread1 (void const *argument);
void led_Thread2 (void const *argument);
osThreadId osprThreadCreate(const osThreadDef_t *thread_def, const prThreadPara *prThread_def);
void TCBEntry(const prThreadPara *prThread_def,int i);
int findlcm(int a,int b);
void osPeriodicStart(void);

/*---------------------------------------Thread Definition--------------------------------------------------*/
osThreadDef(led_Thread1, osPriorityNormal, 1, 0);			//Thread1 definition
osThreadPeriodicDef(led_Thread1,0,5000,10000);							//Periodic Thread

osThreadDef(led_Thread2, osPriorityNormal, 1, 0);			//Thread2 Definition
osThreadPeriodicDef(led_Thread2,0,3000,30000);							//Periodic Thread


osThreadId T_ledOn;
osThreadId T_ledOff;
/*-----------------------------------------------------------------------------------------------------------*/

/*----------------------------------------Thread Functions-------------------------------------------------*/
void led_Thread1 (void const *argument) 
{
	prThreadPara *temp = (prThreadPara *)argument;
	for(;;)
	{
		LED_On(RED);                          
		HAL_Delay(temp->exec/2);
		LED_Off(RED);
		HAL_Delay(temp->exec/2);
		osDelay(temp->period - temp->exec);
	}
}

/*----------------------------------------------------------------------------
  Flash LED 2 and 'release' a semaphore token to sem1
 *---------------------------------------------------------------------------*/
void led_Thread2 (void const *argument) 
{
	prThreadPara *temp = (prThreadPara *)argument;
	for(;;)
	{
		LED_On(BLUE);		
		HAL_Delay(temp->exec/2);
		LED_Off(BLUE);
		HAL_Delay(temp->exec/2);
		osDelay(temp->period - temp->exec);
	}
}


int main(void)
{ 
		osKernelInitialize ();                    // initialize CMSIS-RTOS
		LED_Initialize ();
				
	
			T_ledOff = osprThreadCreate(osThread(led_Thread2), osprThread(led_Thread2));
			T_ledOn =  osprThreadCreate(osThread(led_Thread1), osprThread(led_Thread1));
			
			osPeriodicStart();
			
		  osKernelStart ();                         // start thread execution 
			
		/*	while(1)
			{
				osThreadYield();
			}*/
  
}


osThreadId osprThreadCreate(const osThreadDef_t *thread_def, const prThreadPara *prThread_def)
{
			TCBEntry(prThread_def,thread_count);
			
			return osThreadCreate(thread_def,(void *)prThread_def);
	
}

void TCBEntry(const prThreadPara *prThread_def,int i)
{
		
		TCB_task[thread_count]->TCB_id  = thread_count;
	  TCB_task[thread_count]->arrival = prThread_def->arrival;
		TCB_task[thread_count]->exec    = prThread_def->exec;
		TCB_task[thread_count]->period  = prThread_def->period;
		thread_count++;
}

void osPeriodicStart()
{
	int j;
	int lcm = TCB_task[0]->period;
	
	for(j = 1;j < thread_count;j++)
	{
			lcm = findlcm(lcm, TCB_task[j]->period);
	}
	
	hyperperiod = lcm;
	
	
	
}

int findlcm(int gcd,int b)
{
	int temp;
	int x = gcd, y = b;
	while(b != 0)
	{
		temp = b;
		b = gcd%temp;
		gcd = temp;
	}
	
	return (x*y)/gcd;
}

