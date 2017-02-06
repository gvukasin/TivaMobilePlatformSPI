/****************************************************************************
  Header file for TapeService
  based on the Gen 2 Events and Services Framework
 ****************************************************************************/

#ifndef TAPESERVICE_H
#define TAPESERVICE_H

#include "ES_Configure.h"
#include "ES_Types.h"
#include "ES_Events.h" 

// Public Function Prototypes
bool InitTapeService(uint8_t Priority);
void RunSPIService( void );
bool PostSPIService( ES_Event ThisEvent);
void SPI_InterruptResponse( void );

#endif /* TAPESERVICE_H */


