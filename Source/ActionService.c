/****************************************************************************
 Module
   ActionService.c

 Description
	PWM generation 	 
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"

#include "ActionService.h"
#include "PWMmodule.h"
#include "SPIService.h"
#include "MotorActionsModule.h"

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
#include "inc/hw_nvic.h"

// the headers to access the TivaWare Library
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

#include "BITDEFS.H"
#include "inc/hw_timer.h"

/*----------------------------- Module Defines ----------------------------*/
#define ALL_BITS (0xff<<2)
#define TEST_MODE

#define STOP 0x00
#define CW_90 0x02 
#define CW_45 0x03 
#define CCW_90 0x04 
#define CCW_45 0x05 
#define FORWARD_HALF_SPEED 0x08 
#define FORWARD_FULL_SPEED 0x09 
#define REVERSE_HALF_SPEED 0x10 
#define REVERSE_FULL_SPEED 0x11 
#define ALIGN_BEACON 0x20 
#define DRIVE2TAPE 0x40 

#define FORWARD 1
#define BACKWARD 0
#define CW 1
#define CCW 0

#define DUTY_50 50
#define DUTY_100 100

// oneshot timeout length in ms
#define OneShotTimeout 1000
// using 40 MHz clock
#define TicksPerMS 40000

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service*/
static void InitOneShotISR();
static void StartOneShot( void );

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;
static uint8_t DutyCycle;
static uint32_t SpeedRPM;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitializeActionService

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
 bool InitializeActionService (uint8_t Priority)
 { 
	 //trying github
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
     PostActionService

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
bool PostActionService(ES_Event ThisEvent)
{
  return ES_PostToService( MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunActionService

 Parameters
   ES_Event : the event to process
   ThisEvent will be the time that the event occurred.

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   Controls the necessary parameters to get the desired motion 
   
 Author
   Elena Galbally
****************************************************************************/
ES_Event RunActionService(ES_Event ThisEvent)
{
  ES_Event ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
	
	//State Machine
	switch(ThisEvent.EventParam)
	{
		//Case 1 
		case STOP:
			stop();
			break;
		
		//Case 2
		case CW_90:
			start2rotate(CW);
			break;
		
		//Case 3
		case CW_45:
			start2rotate(CW);
			break;
		
		//Case 4
		case CCW_90:
			start2rotate(CCW);
			break;
		
		//Case 5
		case CCW_45:
			start2rotate(CCW);
			break;
		
		//Case 6
		case FORWARD_HALF_SPEED:
			drive(DUTY_50, FORWARD);
			break;
		
		//Case 7
		case FORWARD_FULL_SPEED:
			drive(DUTY_100, FORWARD);
			break;
		
		//Case 8
		case REVERSE_HALF_SPEED:
			drive(DUTY_50, BACKWARD);
			break;
		
		//Case 9
		case REVERSE_FULL_SPEED:
			drive(DUTY_100, BACKWARD);
			break;
		
		//Case 10
		case ALIGN_BEACON:
			break;
		
		//Case 11
		case DRIVE2TAPE:
			break;
	}
	
	return ReturnEvent;
}

/***************************************************************************
 private functions
 ***************************************************************************/
/****************************************************************************
 Function
     InitOneShotISR

 Parameters
     void

 Returns
     void

 Description
			Initialization for interrupt response for one shot timer to set
			speed to 0
			
 Notes

 Author
     Gabrielle Vukasin, 1/30/17, 20:00
****************************************************************************/
static void InitOneShotISR(){
	// start by enabling the clock to the timer (Wide Timer 0)
	HWREG(SYSCTL_RCGCWTIMER) |= SYSCTL_RCGCWTIMER_R0;

	// kill a few cycles to let the clock get going
	while((HWREG(SYSCTL_PRWTIMER) & SYSCTL_PRWTIMER_R0) != SYSCTL_PRWTIMER_R0){}

	// make sure that timer (Timer A) is disabled before configuring
	HWREG(WTIMER0_BASE+TIMER_O_CTL) &= ~TIMER_CTL_TBEN; //TAEN = Bit1

	// set it up in 32bit wide (individual, not concatenated) mode
	// the constant name derives from the 16/32 bit timer, but this is a 32/64
	// bit timer so we are setting the 32bit mode
	HWREG(WTIMER0_BASE+TIMER_O_CFG) = TIMER_CFG_16_BIT; //bits 0-2 = 0x04

	// set up timer A in 1-shot mode so that it disables timer on timeouts
	// first mask off the TAMR field (bits 0:1) then set the value for
	// 1-shot mode = 0x01
	HWREG(WTIMER0_BASE+TIMER_O_TBMR) = (HWREG(WTIMER0_BASE+TIMER_O_TBMR)& ~TIMER_TBMR_TBMR_M)| TIMER_TBMR_TBMR_1_SHOT;
	
	// set timeout
	HWREG(WTIMER0_BASE+TIMER_O_TBILR) = TicksPerMS*OneShotTimeout;
	
	// enable a local timeout interrupt. TBTOIM = bit 1
	HWREG(WTIMER0_BASE+TIMER_O_IMR) |= TIMER_IMR_TBTOIM; // bit1

	// enable the Timer A in Wide Timer 0 interrupt in the NVIC
	// it is interrupt number 94 so appears in EN2 at bit 31
	HWREG(NVIC_EN2) |= BIT31HI;
	
	// set priority of this timer above below encoder edge
	HWREG(NVIC_PRI23) |= NVIC_PRI23_INTC_M;

	// make sure interrupts are enabled globally
	__enable_irq();

	// now kick the timer off by enabling it and enabling the timer to
	// stall while stopped by the debugger. TAEN = Bit0, TASTALL = bit1
	HWREG(WTIMER0_BASE+TIMER_O_CTL) |= (TIMER_CTL_TBEN | TIMER_CTL_TBSTALL);
}

/****************************************************************************
 Function
     StartOneShot

 Parameters
     void

 Returns
     void

 Description
			Restart the oneshot timer

			
 Notes

 Author
     Gabrielle Vukasin, 1/30/17, 20:00
****************************************************************************/ 
static void StartOneShot( void ){
// now kick the timer off by enabling it and enabling the timer to stall while stopped by the debugger
HWREG(WTIMER0_BASE+TIMER_O_CTL) |= (TIMER_CTL_TBEN | TIMER_CTL_TBSTALL);
}

/****************************************************************************
 Function
     OneShotISR

 Parameters
     void

 Returns
     void

 Description
			Interrupt response for one shot timer to set
			speed to 0

			
 Notes

 Author
     Gabrielle Vukasin, 1/30/17, 20:00
****************************************************************************/ 
void OneShotISR(void){
	// clear interrupt
	HWREG(WTIMER0_BASE+TIMER_O_ICR) = TIMER_ICR_TBTOCINT; 
	
	// set speed to zero
	SpeedRPM = 0;
	
	// restart timer
	StartOneShot();
}
/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/