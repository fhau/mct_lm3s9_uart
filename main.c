/* *********************************************
   File:        main.c
	 Title:       communicate through UART
	 Purpose:     LM3S9D96-DK
	              communicate through RS232
	 Business:    HSA Elektrotechnik
	 Compiler:    MDK-ARM
	 Author/Date: Franz Haunstetter / 09.09.13
	 Comment:     new
	 Author/Date: Franz Haunstetter / 14.09.13
	 Comment:     User button polling
	 Author/Date: Franz Haunstetter / 16.09.13
	 Comment:     RS232 serial in- and output
   *********************************************
*/

/* includes */
#include <lm3s9d96.h>		// hardware register names

/* private macros */
#define BIT(n)		(1 << n)
#define SYSCLK    80000000

/* private function prototypes */
void init(void);						// initialize.c
void init_sys(void);				// system.c
void SysTickRun(void);
void SysTickISR(void);
void init_rs232(void);			// rs232.c
int send_rs232( char c );
int rec_rs232( char* buf );
void init_IO(void);					// GPIO.c

int main()
{
	volatile unsigned long ulIOBits = 0;
	char c;
	
	//
	// Initialize all device ressources.
	//
	init_sys();
	init_rs232();
	init_IO();
	
	//
	// Run the timer at once with interrupts.
	//
	SysTickRun();

	//
	// Run the action in a superloop.
	//
	for(;;)
	{
		//
		// Wait for character from remote and
		// echo with shift to console
		//
		while (!rec_rs232( &c ));
		while (!send_rs232(c>='A' && c<='Z'? c+32: '*'));
		
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
