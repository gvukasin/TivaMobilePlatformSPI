/****************************************************************************
 Header file for MotorService
   MotorService.c
	 
 Elena Galbally
*****************************************************************************/

#ifndef ActionService_H
#define ActionService_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */
#include "ES_Events.h" 

// typedefs for the states
// State definitions for use with the query function
//typedef enum { Stop, Rotate, Drive, Align, Drive2Tape} ActionState_t ;

// Public Function Prototypes
bool InitializeActionService(uint8_t Priority);
bool PostActionService(ES_Event ThisEvent);
ES_Event RunActionService(ES_Event ThisEvent);	
void OneShotISR(void);

#endif /* ActionService_H */