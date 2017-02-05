/****************************************************************************
 Header file for ADService
   ADService.c
	 
 Elena Galbally
*****************************************************************************/

#ifndef ADService_H
#define ADService_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */

// Public Function Prototypes
bool InitializeAD(uint8_t Priority);
bool PostAD(ES_Event ThisEvent);
ES_Event RunAD(ES_Event ThisEvent);		
uint16_t GetDutyCycle(void);

#endif /* ADService_H */