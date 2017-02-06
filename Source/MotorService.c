/****************************************************************************
 Module
   MotorService.c

 Description
	 PWM generation 	 
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"

#include "ADService.h"
#include "MotorService.h"
#include "PWMmodule.h"

#include "ADMulti.h"

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

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service*/

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

static uint8_t DutyCycle;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitializeMotor

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
 bool InitializeMotorService (uint8_t Priority)
 { 
	//printf("\rLittle man in motor init\n");
	 
 	ES_Event ThisEvent;
 	MyPriority = Priority;
	 
	// Initialize PWM functionality
	InitializePWM();
	SetPWMPeriodUS(GetPWMPeriodUS());
	 
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
     PostMotor

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
bool PostMotorService(ES_Event ThisEvent)
{
  return ES_PostToService( MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunMotor

 Parameters
   ES_Event : the event to process
   ThisEvent will be the time that the event occurred.

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   Sets PWM duty cycle 
   
 Author
   Elena Galbally
****************************************************************************/
ES_Event RunMotorService(ES_Event ThisEvent)
{
  ES_Event ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
	
	// 10 times per second:
	if((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == ADTimer))
	{
		//Get duty cycle value
		DutyCycle = GetDutyCycle();
	
		//Output corresponding PWM
		SetPWMDutyCycle(DutyCycle);			
	}						
	
	return ReturnEvent;
}

/***************************************************************************
 private functions
 ***************************************************************************/

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/