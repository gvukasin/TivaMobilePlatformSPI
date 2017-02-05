/****************************************************************************
 Header file for MotorService
   MotorService.c
	 
 Elena Galbally
*****************************************************************************/

#ifndef MotorService_H
#define MotorService_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */
#include "ES_Events.h" 

// Public Function Prototypes
bool InitializeMotor(uint8_t Priority);
bool PostMotor(ES_Event ThisEvent);
ES_Event RunMotor(ES_Event ThisEvent);	

#endif /* MotorService_H */