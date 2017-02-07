/****************************************************************************
  Header file for SPIService
  based on the Gen 2 Events and Services Framework
 ****************************************************************************/

#ifndef SPI_H
#define SPI_H

#include "ES_Configure.h"
#include "ES_Types.h"
#include "ES_Events.h" 

// Public Function Prototypes
bool InitSPIService ( uint8_t );
ES_Event RunSPIService( ES_Event );
bool PostSPIService( ES_Event );
void WriteSPI( uint8_t );

#endif /* SPI_H */

