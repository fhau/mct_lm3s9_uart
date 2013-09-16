/* *********************************************
   File:        system.c
	 Title:       communicate through UART
	 Purpose:     LM3S9D96-DK
	              system hardware driver
	 Business:    HSA Elektrotechnik
	 Compiler:    MDK-ARM
	 Author/Date: Franz Haunstetter / 16.09.13
	 Comment:     new
   *********************************************
*/

/* includes */
#include <lm3s9d96.h>		// hardware register names

/* private macros */
#define BIT(n)		(1 << n)
#define SYSCLK    80000000

//
// Initialize system devices out of reset:
//
void init_sys()
{
	//
	// Initialize the System Tick Timer for 333 ms (3 Hz) light change, then
	// clear the counting element
	//
	NVIC_ST_CURRENT_R = NVIC_ST_RELOAD_R = SYSCLK / (3 * 2);
}

//
// Run the timer together with interrupts.
//
void SysTickRun(void)
{
	NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE | NVIC_ST_CTRL_INTEN;
}

//
// System Tick Timer interrupt service.
//
void SysTickISR()
{
	unsigned long ulShadow;
	
	//
	// Toggle the LED each time the counter reaches 0.
	//
	GPIO_PORTF_DATA_R ^= BIT(3);
	
	//
	// To avoid jitter at small division values remove the ST pending flag
	// and leave the others unchanged (must write 0 to write-only bits).
	//
  ulShadow = NVIC_INT_CTRL_R & ~NVIC_INT_CTRL_UNPEND_SV | NVIC_INT_CTRL_PENDSTCLR;
	NVIC_INT_CTRL_R = ulShadow;

	// Needs appx. 800 ns at 80 MHz.
}
