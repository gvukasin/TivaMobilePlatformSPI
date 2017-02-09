/****************************************************************************
 Header file for MotorActionsModule
   MotorActionsModule.c
	 
 Elena Galbally
*****************************************************************************/

#ifndef MotorActionsModule_H
#define MotorActionsModule_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */
#include "ES_Events.h" 

// Public Function Prototypes
void rotate(uint16_t dutyCycle, int angle, bool direction);
void drive(uint16_t dutyCycle, int speed, bool direction);
void stop(void);

#endif /* MotorActionsModule_H */
