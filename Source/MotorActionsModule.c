/****************************************************************************
 Module
   MotorActionsModule.c

 Description
	Stablish what the pins connected to the motor should do
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
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
#include "inc/hw_pwm.h"

// the headers to access the TivaWare Library
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

#include "BITDEFS.H"
/*----------------------------- Module Defines ----------------------------*/
#define ALL_BITS (0xff<<2)

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service*/
void rotate(uint16_t dutyCycle, int angle, bool direction);
void drive(uint16_t dutyCycle, int speed, bool direction);
void stop(void);

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable


/*------------------------------ Module Code ------------------------------*/


/***************************************************************************
 private functions
 ***************************************************************************/