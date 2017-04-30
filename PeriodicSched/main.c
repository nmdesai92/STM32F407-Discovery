
/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"
#include "Board_LED.h"
#include "stm32f4xx_hal.h"

#define ORANGE	1
#define RED 		2
#define BLUE	  3
#define GREEN 	0

extern uint32_t os_time;
uint32_t HAL_GetTick(void)
{
	return os_time;
}

/*--------------------------GLOBAL Variables-----------------------------*/
int thread_count = 0, count = 1;
uint32_t hyperperiod, check_period;


typedef struct pr_TCB{
	uint16_t TCB_id;
	osThreadId Thread_ID;
	uint32_t arrival;
	uint32_t exec;
	uint32_t period;
}prTCB;

prTCB TCB_task[5];
/*----------------------------------------------------------------------------------------*/

typedef struct prThread_paras{
	os_pthread pthread;
	uint32_t 	 arrival;
	uint32_t 	 exec;
	uint32_t	 period;
}prThreadPara;

#define osThreadPeriodicDef(name,arrival,execution,period)  \
const prThreadPara os_prThread_def_##name = \
{ (name), (arrival), (execution), (period)  }

#define osprThread(name)  \
&os_prThread_def_##name


/*--------------------------------------Private Functions---------------------------------------------------*/

void SystemClock_Config(void);
void led_Thread1 (void const *argument);
void led_Thread2 (void const *argument);
void led_Thread3 (void const *argument);

osThreadId osprThreadCreate(const osThreadDef_t *thread_def, const prThreadPara *prThread_def);
void TCBEntry(const prThreadPara *prThread_def,osThreadId T_id);
uint32_t findgcd(uint32_t a,uint32_t b);
void check(void const *argument);
void osPeriodicStart(void);

/*---------------------------------------Thread Definition--------------------------------------------------*/
osThreadDef(led_Thread1, osPriorityNormal, 1, 0);			//Thread1 definition
osThreadPeriodicDef(led_Thread1,0,5000,10000);							//Periodic Thread

osThreadDef(led_Thread2, osPriorityNormal, 1, 0);			//Thread2 Definition
osThreadPeriodicDef(led_Thread2,0,3000,30000);							//Periodic Thread

osThreadDef(led_Thread3, osPriorityNormal, 1, 0);			//Thread2 Definition
osThreadPeriodicDef(led_Thread3,0,300,16000);							//Periodic Thread

osThreadId T_led1, T_led2, T_led3;

osTimerDef(timer1,check);
osTimerId timer1_ID;
/*-----------------------------------------------------------------------------------------------------------*/

/*----------------------------------------Thread Functions-------------------------------------------------*/
/*----------------------------------------------------------------------------
  Flash RED LED
 *---------------------------------------------------------------------------*/
void led_Thread1 (void const *argument) 
{
	prThreadPara *temp = (prThreadPara *)argument;
	for(;;)
	{
		LED_On(RED);                          
		HAL_Delay(temp->exec/2);
		LED_Off(RED);
		HAL_Delay(temp->exec/2);
		osSignalWait(0x01, osWaitForever);
		
	}
}

/*----------------------------------------------------------------------------
  Flash BLUE
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
		osSignalWait(0x01, osWaitForever);
	}
}

/*----------------------------------------------------------------------------
  Flash GREEN
 *---------------------------------------------------------------------------*/
void led_Thread3 (void const *argument) 
{
	prThreadPara *temp = (prThreadPara *)argument;
	for(;;)
	{
		LED_On(GREEN);		
		HAL_Delay(temp->exec/2);
		LED_Off(GREEN);
		HAL_Delay(temp->exec/2);
		osSignalWait(0x01, osWaitForever);
	}
}

int main(void)
{ 
		osKernelInitialize ();                    // initialize CMSIS-RTOS
		HAL_Init();
	
		LED_Initialize();
			
	
			T_led1 = osprThreadCreate(osThread(led_Thread1), osprThread(led_Thread1));
			T_led2 =  osprThreadCreate(osThread(led_Thread2), osprThread(led_Thread2));
			T_led3 =  osprThreadCreate(osThread(led_Thread3), osprThread(led_Thread3));
			
			osPeriodicStart();			
		/*	while(1)
			{
				osThreadYield();
			}*/
  
}

/*----------------------------Periodic Scheduler APIs-------------------------------------*/
osThreadId osprThreadCreate(const osThreadDef_t *thread_def, const prThreadPara *prThread_def)
{
			
			osThreadId T_id = osThreadCreate(thread_def,(void *)prThread_def);
			TCBEntry(prThread_def,T_id);
			return T_id;
}

void TCBEntry(const prThreadPara *prThread_def,osThreadId T_id)
{
		
		TCB_task[thread_count].TCB_id  = thread_count;
		TCB_task[thread_count].Thread_ID = T_id;
	  TCB_task[thread_count].arrival = prThread_def->arrival;
		TCB_task[thread_count].exec    = prThread_def->exec;
		TCB_task[thread_count].period  = prThread_def->period;
		thread_count++;
}

void osPeriodicStart()
{
	int j;
	uint32_t gcd = TCB_task[0].period;
	
	for(j = 1;j < thread_count;j++)
	{
			gcd = findgcd(gcd, TCB_task[j].period);
	}
	
	check_period =  gcd;
	//hyperperiod = lcm;
	timer1_ID = osTimerCreate(osTimer(timer1), osTimerPeriodic, NULL);
	//osKernelStart();
	osTimerStart(timer1_ID,check_period);
	
}

void check(const void *argument)
{
		int j;
		
		for(j = 0;j < thread_count;j++)
		{
			if((count % (TCB_task[j].period/check_period)) == 0)
			{
				osSignalSet(TCB_task[j].Thread_ID,0x01);
			}
		}
		count++;
}

uint32_t findgcd(uint32_t gcd,uint32_t b)
{
	uint32_t temp;
	//int x = gcd, y = b;
	while(b != 0)
	{
		temp = b;
		b = gcd%temp;
		gcd = temp;
	}
	
	return gcd;
}

