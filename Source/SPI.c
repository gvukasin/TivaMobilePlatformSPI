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
#include "inc/hw_ssi.h"

#include "SPI.h"

// to print comments to the terminal
#include <stdio.h>
#include "termio.h" 

/*----------------------------- Module Defines ----------------------------*/
// pin definitions
#define RXPIN BIT4HI
#define TXPIN BIT5HI
#define SPIClock BIT2HI
#define SlaveSelect BIT3HI

// SSI Module definition
#define SSIModule BIT0HI

// CPSR divisor for SSI clock rate
#define CPSDVSR 2

// SCR divisor for SSI clock rate
#define SCR 0

// defining ALL_BITS
#define ALL_BITS (0xff<<2)

#define BitsPerNibble 4


/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/
static void InitSerialHardware(void);
static void WriteSPI(uint8_t);

/*---------------------------- Module Variables ---------------------------*/
static uint8_t MyPriority;

// received data from data register
static uint8_t ReceivedData;

// data to write to data register
static uint8_t LastChunk;

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
	// clear interrupt
	
	// read command (FIX:I don't think this is how you read from this register)
	//ReceivedData = HWREG(SSI0_BASE+SSI_O_DR);
	
	// post command to motor service?
	
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
	HWREG(SYSCTL_RCGCGPIO)|= SYSCTL_RCGCGPIO_R0;
		
	// Enable the clock to SSI module
	HWREG(SYSCTL_RCGCSSI) |= SSIModule;
		
	// Wait for the GPIO port to be ready
	while((HWREG(SYSCTL_RCGCGPIO)& SYSCTL_PRGPIO_R0) != SYSCTL_PRGPIO_R0){};
		
	// Program the GPIO to use the alternate functions on the SSI pins
	HWREG(GPIO_PORTA_BASE+GPIO_O_AFSEL) |= (RXPIN|TXPIN|SPIClock|SlaveSelect);	
		
	// Set mux position in GPIOPCTL to select the SSI use of the pins 
	// map bit RXPIN's alt function to SSI0Rx (2), by clearing nibble then shifting 2 to 4th nibble 
	HWREG(GPIO_PORTA_BASE+GPIO_O_PCTL) = (HWREG(GPIO_PORTA_BASE+GPIO_O_PCTL) & 0xfff0ffff) + (2<<(4*BitsPerNibble));
		
	// map bit TXPIN's alt function to SSI0Tx (2), by clearing nibble then shifting 2 to 5th nibble 
	HWREG(GPIO_PORTA_BASE+GPIO_O_PCTL) = (HWREG(GPIO_PORTA_BASE+GPIO_O_PCTL) & 0xff0fffff) + (2<<(5*BitsPerNibble));
		
	// map bit SPIClock's alt function to SSI0Clk (2), by clearing nibble then shifting 2 to 2nd nibble 
	HWREG(GPIO_PORTA_BASE+GPIO_O_PCTL) = (HWREG(GPIO_PORTA_BASE+GPIO_O_PCTL) & 0xfffff0ff) + (2<<(2*BitsPerNibble));
	
	// map bit SlaveSelect's alt function to SSI0Fss (2), by clearing nibble then shifting 2 to 3rd nibble 
	HWREG(GPIO_PORTA_BASE+GPIO_O_PCTL) = (HWREG(GPIO_PORTA_BASE+GPIO_O_PCTL) & 0xffff0fff) + (2<<(3*BitsPerNibble));		
		
	// Program the port lines for digital I/O
	HWREG(GPIO_PORTA_BASE+GPIO_O_DEN)|= (RXPIN|TXPIN|SPIClock|SlaveSelect);

	// Program the required data directions on the port lines
	HWREG(GPIO_PORTA_BASE+GPIO_O_DIR)|= (RXPIN|TXPIN|SPIClock|SlaveSelect);
		
	// If using SPI mode 3, program the pull-up on the clock line and slave select line
	HWREG(GPIO_PORTA_BASE+GPIO_O_PUR) |= (SPIClock|SlaveSelect);
		
		// Wait for the SSI0 to be ready (FIX: need something like a peripheral ready to check)
	//while((HWREG(SYSCTL_RCGCSSI)& ) != ){};
	
	// Make sure that the SSI is disabled before programming mode bits
	HWREG(SSI0_BASE+SSI_O_CR1) &= (~SSI_CR1_SSE);
		
	// Select master mode (MS) & TXRIS indicating End of Transmit (EOT)
	HWREG(SSI0_BASE+SSI_O_CR1) &= (~SSI_CR1_MS);
	HWREG(SSI0_BASE+SSI_O_CR1) |= SSI_CR1_EOT;

	// Configure the SSI clock source to the system clock
	HWREG(SSI0_BASE+SSI_O_CC) = SSI_CC_CS_SYSPLL; 

	// Configure the clock pre-scaler (FIX: set CPSDVSR)
	HWREG(SSI0_BASE+SSI_O_CPSR) = CPSDVSR;

	// Configure clock rate (SCR) by clearing with a mask and then shifting SCR two nibbles (FIX: set SCR)
	HWREG(SSI0_BASE+SSI_O_CR0) = (HWREG(SSI0_BASE+SSI_O_CR0) & 0xffff00ff)+(SCR<<(2*BitsPerNibble));
	
	// Configure phase & polarity (SPH, SPO), data size (DSS)
	HWREG(SSI0_BASE+SSI_O_CR0) |= (SSI_CR0_SPH |SSI_CR0_SPO|SSI_CR0_DSS_8);
	
	// Configure mode(FRR) using mask to select Freescale SPI Frame Format as FRF mode by clearing
	HWREG(SSI0_BASE+SSI_O_CR0) &= (~SSI_CR0_FRF_M);
	
	// Locally enable interrupts (TXIM in SSIIM)
	//HWREG(SSI0_BASE+SSI_O_IM) |= SSI_IM_RXIM;
	// Do we want to enable rx not tx? do we want to enable receive overrun? 
	HWREG(SSI0_BASE+SSI_O_IM) |= SSI_IM_TXIM;

	// Make sure that the SSI is enabled for operation
	HWREG(SSI0_BASE+SSI_O_CR1) |= SSI_CR1_SSE;

//Enable the NVIC interrupt for the SSI when starting to transmit (FIX: should we put this somewhere else because it will create an infinite loop until 
// data is transmitted)
}

/****************************************************************************
 Function
     WriteSPI

 Parameters
     8 bits to write to data register

 Returns
     void

 Description
     writes 8 bits on Tx line 

 Author
     Team 16, 02/04/17, 23:00
****************************************************************************/
void WriteSPI(uint8_t DataToWrite)
{
	//Enable the NVIC interrupt for the SSI (FIX:how to do this?)
		
	// write to data register
	HWREG(SSI0_BASE+SSI_O_DR) = DataToWrite;
	
	// keep track of last 8 bits written 
	LastChunk = DataToWrite;
}

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/
