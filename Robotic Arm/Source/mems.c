/*Program reads accelerometer X-Y variables.
	Calculates PWM on_time for servo motor according to measured angle.
	Controls servo through GPIO pins connected with LEDs
	Red   LED5 : X-axis
	Green LED4 : Y-axis*/


/*------------------------------- Includes -------------------------------------*/
#include "mems.h"

/* Init af threshold to detect acceleration on MEMS */
/* Typical value: 
      - No  acceleration: X, Y inferior to 100 (positive or negative)
      - Max acceleration: X, Y around 2000 (positive or negative) */
int16_t ThresholdHigh = 200;
int16_t ThresholdLow = -200;

/* Private function prototypes -----------------------------------------------*/
static void ACCELERO_ReadAcc(void);
void us_Delay(int16_t);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Test ACCELERATOR MEMS Hardware.
  *         The main objective of this test is to check acceleration on 2 axes X and Y
  * @param  None
  * @retval None
  */
void ACCELERO_MEMS_Test(void)
{
  /* Init Accelerometer MEMS */
  if(BSP_ACCELERO_Init() != ACCELERO_OK)
  {
    /* Initialization Error */
    Error_Handler(); 
  }

    ACCELERO_ReadAcc();
}

/**
  * @brief  Read Acceleration data.
  * @param  None
  * @retval None
  */
/*----------------------Angle Calulation-----------------------------------------------*/
/*   
		Motor Angle  	|	 90	 |	0	  | 180		|
		Accel Reading |	200  | 2000 | 2000  |
		PWM onTime  	|	2250 |  700	| 3800	|*/

/*------------------------------Read Accelerometer----------------------------------*/
static void ACCELERO_ReadAcc(void)
{
  /* Accelerometer variables */
  int16_t buffer[3] = {0};
  int16_t xval, yval = 0x00;
  int16_t on_time = 0;
	
  /* Read Acceleration */
  BSP_ACCELERO_GetXYZ(buffer);
  
  xval = buffer[0];
  yval = buffer[1];
  
		if((ABS(xval))>(ABS(yval)))
		{
			if(xval > ThresholdHigh)
			{ 
				on_time = 2422 - (xval*1550)/1800;		//Motor Angle : 90->0 Pulse ==> Pulse Time 2250->700
				//on_time = 700;
				/* LED5  PWM pulse*/
				BSP_LED_On(LED5);
				us_Delay(on_time);
				BSP_LED_Off(LED5);
				us_Delay(20000 - on_time);
			}
			else if(xval < ThresholdLow)
			{ 
				on_time = 2077 + (ABS(xval)*1550)/1800;	//Motor Angle : 90->180 Pulse ==> Pulse Time 2250->3800
			//	on_time = 3800;
			/* LED5 PWM Pulse */
				BSP_LED_On(LED5);      
				us_Delay(on_time);
				BSP_LED_Off(LED5);
				us_Delay(20000 - on_time);
			}
			else							//Delay for stable : 90 degre		
			{ 
				BSP_LED_On(LED5);
				us_Delay(2250);
				BSP_LED_Off(LED5);
				us_Delay(20000 - 2250);
			}
  }
  else
  {
    if(yval < ThresholdLow)
    {
      on_time = 2422 - (yval*1550)/1800;	//Motor Angle : 90->0 Pulse ==> Pulse Time 2250->700
			//on_time = 700;
      /* LED4 PWM Pulse */
      BSP_LED_On(LED4);
      us_Delay(on_time);
			BSP_LED_Off(LED4);
      us_Delay(20000 - on_time);
    }
    else if(yval > ThresholdHigh)
    {
     on_time = 2077 + (ABS(yval)*1550)/1800;	//Motor Angle : 90->180 Pulse ==> Pulse Time 2250->3800
      //on_time = 3800;
	  /* LED4 PWM Pulse*/
      BSP_LED_On(LED4);      
      us_Delay(on_time);
			BSP_LED_Off(LED4);
      us_Delay(20000 - on_time); 
    } 
    else							//Delay for stable : 90 degre
    { 
      BSP_LED_On(LED4);
      us_Delay(2250);
			BSP_LED_Off(LED4);
      us_Delay(20000 - 2250);
    }
  } 
  
  BSP_LED_Off(LED3);
  BSP_LED_Off(LED4);
  BSP_LED_Off(LED5);
  BSP_LED_Off(LED6);
}

/*------------------------Delay of ~1 microsecond--------------------------------------------*/
void us_Delay(int16_t period)
{
	int16_t i,j;
	for(i = 0; i < period;i++)
		for(j = 0; j <= 23;j++);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
