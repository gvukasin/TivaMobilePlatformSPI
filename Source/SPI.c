/****************************************************************************
 Module
   SPIService.c

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
#include "inc/hw_ssi.h"

#include "SPI.h"

// to print comments to the terminal
#include <stdio.h>
#include "termio.h" 

/*----------------------------- Module Defines ----------------------------*/
// pin definitions
#define RXPIN BIT2HI
#define TXPIN BIT3HI
#define SPIClock BIT0HI
#define SlaveSelect BIT1HI

// SSI Module definition
#define SSIModule BIT1HI
#define SSI_NVIC_HI BIT7HI

// defining ALL_BITS
#define ALL_BITS (0xff<<2)

#define BitsPerNibble 4


/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/
static void InitSerialHardware(void);

/*---------------------------- Module Variables ---------------------------*/
static uint8_t MyPriority;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitSPIService

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
bool InitSPIService ( uint8_t Priority )
{
	 ES_Event ThisEvent;
	 MyPriority = Priority;
	 
	 // Initialize hardware
	 InitSerialHardware();

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
     RunSPIService

 Parameters
     void

 Returns
     void

 Description
     Xmits info
 Notes

 Author
     Team 16, 02/04/17, 16:00
****************************************************************************/
void RunSPIService ( void )
{
	//State machine - idling or Xmitting (class) 
}

/****************************************************************************
 Function
     PostSPIService

 Parameters
     EF_Event ThisEvent ,the event to post to the queue

 Returns
		 bool false if the Enqueue operation failed, true otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     Team 16, 02/04/17, 16:00
****************************************************************************/
bool PostSPIService( ES_Event ThisEvent )
{
	 return ES_PostToService(MyPriority, ThisEvent);
}


/****************************************************************************
 Function
     SPI_InterruptResponse

 Parameters
     void

 Returns
     void

 Description
     Initializes hardware in the Tiva to create an SPI master

 Author
     Team 16, 02/04/17, 16:00
****************************************************************************/
void SPI_InterruptResponse( void )
{
	
}


/*----------------------------------------------------------------------------
private functions
-----------------------------------------------------------------------------*/
/****************************************************************************
 Function
     InitSerialHardware

 Parameters
     void

 Returns
     void

 Description
     keeps the service init more readable

 Author
     Team 16, 02/04/17, 16:00
****************************************************************************/
static void InitSerialHardware(void)
{
	//Enable the clock to the GPIO port
	HWREG(SYSCTL_RCGCGPIO)|= SYSCTL_RCGCGPIO_R3;		
	// Enable the clock to SSI module
	HWREG(SYSCTL_RCGCSSI) = SSIModule;		
	// Wait for the GPIO port to be ready
	while((HWREG(SYSCTL_RCGCGPIO) & SYSCTL_PRGPIO_R3) != SYSCTL_PRGPIO_R3){};
		
	// Program the GPIO to use the alternate functions on the SSI pins
	HWREG(GPIO_PORTD_BASE + GPIO_O_AFSEL) |= (RXPIN|TXPIN|SPIClock|SlaveSelect);	
		
	// Set mux position in GPIOPCTL to select the SSI use of the pins:
	// map bit RXPIN's alt function to SSI1Rx (2), by clearing nibble then shifting 2 to 2nd nibble 
	HWREG(GPIO_PORTD_BASE + GPIO_O_PCTL) = (HWREG(GPIO_PORTD_BASE + GPIO_O_PCTL) & 0xfffff0ff) + (2<<(2*BitsPerNibble));		
	// map bit TXPIN's alt function to SSI1Tx (2), by clearing nibble then shifting 2 to 3rd nibble 
	HWREG(GPIO_PORTD_BASE + GPIO_O_PCTL) = (HWREG(GPIO_PORTD_BASE + GPIO_O_PCTL) & 0xffff0fff) + (2<<(3*BitsPerNibble));		
	// map bit SPIClock's alt function to SSI1Clk (2), by clearing nibble then shifting 2 to 0th nibble 
	HWREG(GPIO_PORTD_BASE + GPIO_O_PCTL) = (HWREG(GPIO_PORTD_BASE + GPIO_O_PCTL) & 0xfffffff0) + (2);	
	// map bit SlaveSelect's alt function to SSI1Fss (2), by clearing nibble then shifting 2 to 1st nibble 
	HWREG(GPIO_PORTD_BASE + GPIO_O_PCTL) = (HWREG(GPIO_PORTD_BASE + GPIO_O_PCTL) & 0xffffff0f) + (2<<(BitsPerNibble));		
		
	// Program the port lines for digital I/O
	HWREG(GPIO_PORTD_BASE + GPIO_O_DEN)|= (RXPIN|TXPIN|SPIClock|SlaveSelect);
	// Program the required data directions on the port lines
	HWREG(GPIO_PORTD_BASE + GPIO_O_DIR)|= (RXPIN|TXPIN|SPIClock|SlaveSelect);
		
	// If using SPI mode 3, program the pull-up on the clock line
	HWREG(GPIO_PORTD_BASE + GPIO_O_PUR) |= SPIClock;		
	// Wait for the SSI0 to be ready
	while ((HWREG(SYSCTL_RCGCSSI)& SYSCTL_RCGCSSI_R0)!= SYSCTL_RCGCSSI_R0);		
	// Make sure that the SSI is disabled before programming mode bits
	HWREG(SSI1_BASE + SSI_O_CR1) &= (~SSI_CR1_SSE);	
	// Select master mode (MS) & TXRIS indicating End of Transmit (EOT)
	HWREG(SSI1_BASE + SSI_O_CR1) &= (~SSI_CR1_MS);
	HWREG(SSI1_BASE + SSI_O_CR1) &= SSI_CR1_EOT;
	
	// Configure the SSI clock source to the system clock
	HWREG(SSI1_BASE + SSI_O_CC) = SSI_CC_CS_SYSPLL; 
	// Configure the clock pre-scaler

	// Configure clock rate (SCR), phase & polarity (SPH, SPO), mode (FRF),data size (DSS)


	// Locally enable interrupts (TXIM in SSIIM) 
	//unmasking -tiva DS pg.977
	HWREG(SSI1_BASE + SSI_O_IM) |= SSI_IM_TXIM;
	// Make sure that the SSI is enabled for operation
	HWREG(SSI1_BASE + SSI_O_CR1) |= SSI_CR1_SSE;

	//Enable the NVIC interrupt for the SSI when starting to transmit
	//Interrupt number -tiva DS pg.104
	HWREG(NVIC_EN0) |= SSI_NVIC_HI;
}

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/
