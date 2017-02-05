/****************************************************************************
 Module
   ADService.c

 Description
   Handles the reading of the A/D and updating the interval between steps

****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ADService.h"
#include "ADMulti.h"
#include "MotorService.h"

#include <stdio.h>
#include <termio.h>

// the common headers for C99 types 
#include <stdint.h>
#include <stdbool.h>

// the headers to access the GPIO subsystem
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"

// the headers to access the TivaWare Library
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

#include "BITDEFS.H"

/*----------------------------- Module Defines ----------------------------*/
#define ALL_BITS (0xff<<2)
#define TEST_MODE

#define NUM_ADCHANNELS_USED 1
#define AD_TIMER_MS 100

#define MIN_DUTY_CYCLE 0
#define MAX_DUTY_CYCLE 100
#define MIN_POT_VALUE 0
#define MAX_POT_VALUE 4095

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

//static uint16_t TimeStep;
static uint32_t ADResult[1]; //Array containing AD conversion result
static uint16_t PotValue;
static uint16_t DutyCycle;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitializeAD

 Parameters
     uint8_t : the priorty of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, and does any 
     other required initialization for this service
 Notes

 Author
     Elena Galbally
****************************************************************************/
 bool InitializeAD (uint8_t Priority)
 { 
	 //printf("\rLittle man in AD init\n");
	 
 	 ES_Event ThisEvent;
 	 MyPriority = Priority;

	 // initialize the A/D converter to convert on 1 channel (PE0)
	 ADC_MultiInit(NUM_ADCHANNELS_USED); 
	 
	 // initialize timer for 10ms timeout condition
	 ES_Timer_InitTimer(ADTimer, AD_TIMER_MS);
 
	 // post the initial transition event
 	 ThisEvent.EventType = ES_INIT;
 	 if (ES_PostToService( MyPriority, ThisEvent) == true)
 	 {
 		 return true;
 	 }
	 else
 	 {
 		 return false;
 	 }
 }

/****************************************************************************
 Function
     PostAD

 Parameters
     EF_Event ThisEvent ,the event to post to the queue

 Returns
     bool false if the Enqueue operation failed, true otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     Elena Galbally
****************************************************************************/
bool PostAD(ES_Event ThisEvent)
{
  return ES_PostToService( MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunAD

 Parameters
   ES_Event : the event to process
   ThisEvent will be the time that the event occurred.

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   Get duty cycle from the pot readings 
   
 Author
   Elena Galbally
****************************************************************************/
ES_Event RunAD(ES_Event ThisEvent)
{	
	ES_Event ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
	
	if((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == ADTimer))
	{
		// Read AD pin
		ADC_MultiRead(ADResult);
		PotValue = ADResult[0];
		
		// Scale pot value and set as duty cycle
		DutyCycle = ((MAX_DUTY_CYCLE - MIN_DUTY_CYCLE)*PotValue + (MIN_DUTY_CYCLE*MAX_POT_VALUE)) / MAX_POT_VALUE;
		
		// Reset timer
		ES_Timer_InitTimer(ADTimer, AD_TIMER_MS);	
	}
	
	return ReturnEvent;
}

/****************************************************************************
 Function
    GetDutyCycle

 Parameters
   None

 Returns
   DutyCycle value 
   
 Author
   Elena Galbally
****************************************************************************/
uint16_t GetDutyCycle(void)
{
	return DutyCycle;
}

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/