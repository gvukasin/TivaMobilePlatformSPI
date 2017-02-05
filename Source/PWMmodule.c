/****************************************************************************
 Module
   PWMmodule.c

 Description
	 Generate PWM waveforms
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "PWMmodule.h"

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
#define TEST_MODE

#define GenA_Normal (PWM_0_GENA_ACTCMPAU_ONE | PWM_0_GENA_ACTCMPAD_ZERO )
#define GenB_Normal (PWM_0_GENB_ACTCMPBU_ONE | PWM_0_GENB_ACTCMPBD_ZERO )

//freq(Hz)	period(us)
//50				20000  ---> for measuring motor time constant
//2500			400		 ---> frequency for lab#7
//200				5000
//250				4000
//500				2000
//1000			1000
//2000			500
//10000			100

#define PeriodInUS 1000 
#define PWMTicksPerUS 40000/(1000*32) //System clock (40MHz) / 32
#define BitsPerNibble 4

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service*/
static void Set100DC(void);
static void Set0DC(void);
static void RestoreDC(void);

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint16_t PeriodInUSVal = PeriodInUS;

/*------------------------------ Module Code ------------------------------*/

void InitializePWM(void)
{
	// Enable the clock to the PWM Module (PWM0)
	HWREG(SYSCTL_RCGCPWM) |= SYSCTL_RCGCPWM_R0;
	// Enable the clock to Port B
	HWREG(SYSCTL_RCGCGPIO) |= SYSCTL_RCGCGPIO_R1;
	// Select the PWM clock as System Clock/32
	HWREG(SYSCTL_RCC) = (HWREG(SYSCTL_RCC) & ~SYSCTL_RCC_PWMDIV_M) | (SYSCTL_RCC_USEPWMDIV | SYSCTL_RCC_PWMDIV_32);
	// Make sure that the PWM module clock has gotten going
	while ((HWREG(SYSCTL_PRPWM) & SYSCTL_PRPWM_R0) != SYSCTL_PRPWM_R0);
	
	// Disable the PWM while initializing
	HWREG( PWM0_BASE+PWM_O_0_CTL ) = 0;
	
	// program generators to go to 1 at rising compare A/B, 0 on falling compare A/B
	HWREG( PWM0_BASE+PWM_O_0_GENA) = GenA_Normal;
	HWREG( PWM0_BASE+PWM_O_0_GENB) = GenB_Normal;
	
	// Set the PWM period
	HWREG( PWM0_BASE+PWM_O_0_LOAD) = ((PeriodInUS * PWMTicksPerUS))>>1;
	
	// Set the initial Duty cycle on A and B to 0 
	HWREG( PWM0_BASE+PWM_O_0_GENA) = PWM_0_GENA_ACTZERO_ZERO;
	HWREG( PWM0_BASE+PWM_O_0_GENB) = PWM_0_GENB_ACTZERO_ZERO;
	
	// Enable the PWM outputs
	HWREG( PWM0_BASE+PWM_O_ENABLE) |= (PWM_ENABLE_PWM1EN | PWM_ENABLE_PWM0EN);
	
	// Configure the Port B pins to be PWM outputs
	HWREG(GPIO_PORTB_BASE+GPIO_O_AFSEL) |= (BIT7HI | BIT6HI);
	HWREG(GPIO_PORTB_BASE+GPIO_O_PCTL) = (HWREG(GPIO_PORTB_BASE+GPIO_O_PCTL) & 0x00ffffff) + (4<<(7*BitsPerNibble)) + (4<<(6*BitsPerNibble));
	// Enable pins 6 & 7 on Port B for digital I/O
	HWREG(GPIO_PORTB_BASE+GPIO_O_DEN) |= (BIT7HI | BIT6HI);
	// make pins 6 & 7 on Port B into outputs
	HWREG(GPIO_PORTB_BASE+GPIO_O_DIR) |= (BIT7HI |BIT6HI);
	
	// set the up/down count mode, enable the PWM generator and make
	// both generator updates locally synchronized to zero count
	HWREG(PWM0_BASE+ PWM_O_0_CTL) = (PWM_0_CTL_MODE | PWM_0_CTL_ENABLE | PWM_0_CTL_GENAUPD_LS | PWM_0_CTL_GENBUPD_LS);

	//printf("\rLil' man finished PWM init\r\n");
}

void SetPWMDutyCycle(uint8_t DutyCycle)
{
	if (DutyCycle == 0)
	{
		Set0DC();
	}
	else if (DutyCycle == 100)
	{
		Set100DC();
	}
	else
	{
		RestoreDC();
		HWREG( PWM0_BASE+PWM_O_0_CMPA) = (HWREG( PWM0_BASE+PWM_O_0_LOAD)) - ((DutyCycle*(PeriodInUS * PWMTicksPerUS)/100)>>1);	
	}
}

void SetPWMPeriodUS(uint16_t Period)
{
		HWREG( PWM0_BASE+PWM_O_0_LOAD) = ((Period * PWMTicksPerUS))>>1;
}

uint16_t GetPWMPeriodUS(void)
{
	static uint16_t PeriodUS = PeriodInUS;
	return PeriodUS;
}

/***************************************************************************
 private functions
 ***************************************************************************/
static void Set100DC(void){
// Program 100% DC - set the action on Zero to set the output to one
HWREG( PWM0_BASE+PWM_O_0_GENA) = PWM_0_GENA_ACTZERO_ONE;
}

static void Set0DC(void){
// Program 0% DC - set the action on Zero to set the output to zero
HWREG( PWM0_BASE+PWM_O_0_GENA) = PWM_0_GENA_ACTZERO_ZERO;
}

static void RestoreDC(void){
// Restore the previous DC - set the action back to the normal actions
HWREG( PWM0_BASE+PWM_O_0_GENA) = GenA_Normal;
}
