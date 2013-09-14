/* *********************************************
   File:        main.c
	 Title:       communicate through UART
	 Purpose:     LM3S9D96-DK
	              blink user led, spaced by time
                loops
	 Business:    HSA Elektrotechnik
	 Compiler:    MDK-ARM
	 Author/Date: Franz Haunstetter / 09.09.13
	 Comment:     new
   *********************************************
*/

/* includes */
#include <lm3s9d96.h>		// hardware register names

/* private macros */
#define BIT(n)		(1 << n)
#define SYSCLK    80000000

/* private function prototypes */
void SysTickISR()
{
	unsigned long ulShadow;
	//
	// Toggle the LED each time the counter reaches 0.
	//
	GPIO_PORTF_DATA_R ^= BIT(3);
	//
	// To avoid jitter at small division values remove the ST pending flag
	// and leave the others unchanged (must write 0 to write only bits).
	//
  ulShadow = NVIC_INT_CTRL_R & ~NVIC_INT_CTRL_UNPEND_SV | NVIC_INT_CTRL_PENDSTCLR;
	NVIC_INT_CTRL_R = ulShadow;

	// Needs appx. 800 ns at 80 MHz.
}


int main()
{
	volatile unsigned long ulIOBits;
	
	//
	// Initialize the System Tick Timer for 333 ms light change, then
	// clear the counting element
	//
	NVIC_ST_CURRENT_R = NVIC_ST_RELOAD_R = SYSCLK / (3 * 2);

	//
	// Enable the GPIO ports used.
	//
	SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOF | SYSCTL_RCGC2_GPIOJ;

	//
	// Do a dummy read to insert a few cycles after enabling the peripheral.
	//
	ulIOBits = SYSCTL_RCGC2_R;

	//
	// Enable the GPIO pin for the LEDs (PF2, PF3).  Set the direction as output, and
	// enable the GPIO pin for digital function.
	//
	GPIO_PORTF_DIR_R = BIT(3) | BIT(2);
	GPIO_PORTF_DEN_R = BIT(3) | BIT(2);

	//
	// Enable the GPIO pin for the BUTTON (PJ7).  Set the direction as input, and
	// enable the GPIO pin for digital function.
	//
	GPIO_PORTJ_DEN_R = BIT(7);
	ulIOBits = 0;
	
	//
	// Run the timer at once with interrupts.
	//
	NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE | NVIC_ST_CTRL_INTEN;

	//
	// Run the action in a superloop.
	//
	for(;;)
	{

		//
		// Reflect the button's state on second LED with every change.
		//
		if ((GPIO_PORTJ_DATA_R & BIT(7)) != (ulIOBits & BIT(7)))
		{
			if ((GPIO_PORTJ_DATA_R & BIT(7)) == 0)
				GPIO_PORTF_DATA_R &= ~BIT(2);
			else
				GPIO_PORTF_DATA_R |= BIT(2);
			ulIOBits = GPIO_PORTJ_DATA_R;
		}
	}
}
