/****************************************************************************
 Module
   SPI.c

 Revision
   1.0.1

 Description
   This is the first service for the Test Harness under the 
   Gen2 Events and Services Framework.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 11/02/13 17:21 jec      added exercise of the event deferral/recall module
 08/05/13 20:33 jec      converted to test harness service
 01/16/12 09:58 jec      began conversion from TemplateFSM.c
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for the framework and this service
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"	// Define PART_TM4C123GH6PM in project
#include "driverlib/gpio.h"
#include "inc/hw_nvic.h"
#include "SPI.h"
#include "BITDEFS.H"

// to print comments to the terminal
#include <stdio.h>
#include "termio.h" 

/*----------------------------- Module Defines ----------------------------*/
// pin definitions
#define RXPIN BIT2HI
#define TXPIN BIT3HI
#define SPIClock BIT0HI
#define SlaveSelect BIT1HI



/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/
static void InitSPI(void);

/*---------------------------- Module Variables ---------------------------*/


/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitSPI

 Parameters
     void

 Returns
     void

 Description
     Initializes hardware in the Tiva to create an SPI master
 Notes

 Author
     Team 16, 02/04/17, 16:00
****************************************************************************/
void InitSPI ( void )
{
	//Enable the clock to the GPIO port
	HWREG(SYSCTL_RCGCGPIO)|= SYSCTL_RCGCGPIO_R3;
		
	// Enable the clock to SSI module
		
	// Wait for the GPIO port to be ready
	while((HWREG(SYSCTL_RCGCGPIO)& SYSCTL_PRGPIO_R3) != SYSCTL_PRGPIO_R3){};
		
	// Program the GPIO to use the alternate functions on the SSI pins
		
	// Set mux position in GPIOPCTL to select the SSI use of the pins
		
	// Program the port lines for digital I/O
	HWREG(GPIO_PORTF_BASE+GPIO_O_DEN)|= (RXPIN|TXPIN|SPIClock);
	HWREG(GPIO_PORTF_BASE+GPIO_O_DIR)|= TestPin;
		
	// Program the required data directions on the port lines
		
	// If using SPI mode 3, program the pull-up on the clock line
		
	// Wait for the SSI0 to be ready
		
	// Make sure that the SSI is disabled before programming mode bits
		
	// Select master mode (MS) & TXRIS indicating End of Transmit (EOT)

	// Configure the SSI clock source to the system clock

	// Configure the clock pre-scaler

	// Configure clock rate (SCR), phase & polarity (SPH, SPO), mode (FRF),data size (DSS)
	
	// Locally enable interrupts (TXIM in SSIIM)

	// Make sure that the SSI is enabled for operation

	//Enable the NVIC interrupt for the SSI when starting to transmit
	
}

/****************************************************************************
 Function
     SPIISR

 Parameters
     void

 Returns
     void

 Description
     Initializes hardware in the Tiva to create an SPI master
 Notes

 Author
     Team 16, 02/04/17, 16:00
****************************************************************************/
void SPIISR( void )
{
	
}



/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/