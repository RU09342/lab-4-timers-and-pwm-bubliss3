# Software Debouncing

Utilize the TIMER modules within 5 different MSP430 microcontrollers to implement a debounced swith to control the state of an LED:

* MSP430G2553
* MSP430F5529
* MSP430FR2311
* MSP430FR5994
* MSP430FR6989
 
This process should allow your processor to run other code, while relying on timers and interrupts to manage the debouncing in the background.

## Code Configuration

The following code can be used on all of the boards with a slight change in code for the FR2311,where it uses Timer B vs Timer A. The only other change that is needed to be made is the pin assignments labeled as x's for LED1 and y's for Button

```c
	int main(void)
	{
    		WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    		//PM5CTL0 &= ~LOCKLPM5;

    		PxDIR  |= BITx;                     // Set output Direction
    		PyREN  |= BITy;                   // Enables resistor for P1.1
    		PyOUT  = BITy;                    // Makes resistor P1.1 a pull up
    		PyIE |= BITy;
   	        PyIES = 0;                     // Interrupts when 1.1 is pressed or low to high transition
    		PyIFG &= ~BITy;                   // Clear interrupt flag
    		TA0CCTL0 = 0x0010;                  // Enables Capture/compare register for interrupt of timer A0
    		TA0CCR0 = 60000;                    // Set value for Timer A0 to count up to
    		TA0CTL = TASSEL_2 + MC_0;           // Timer_A Control Register - SMCLK high frequency clock, timer stopped

        	__enable_interrupt();

        	__bis_SR_register(LPM0 + GIE);      // Low Power Mode w/ general interrupts enabled

    	}

    		// Interrupt Service Routine 1 which toggles on the LED on the release of the Button and starts timer a to act as a delay
    		#pragma vector = PORT1_VECTOR
    		__interrupt void Interrupt_1(void)
    		{
        		PxOUT ^= BITx;                      // Toggle LED
        		PyIE &= ~BITy;                    // reset interrupt
        		TA0CTL = TASSEL_2 + MC_1;           // Timer_A Control Register - SMCLK high frequency clock, Up
    		}

    		// Interrupt Service Routine 2 which stops Timer A0 and resets the interrupt
    		#pragma vector=TIMER0_A0_VECTOR
    		__interrupt void Interrupt_2(void)
    		{
        		TA0CTL = MC_0;                      // Stop timer
        		PyIE |= BITy;                     // Interrupt enable set to BUTTON
        		PyIFG &= ~BITy;                     // Clear interrupt flag
    		}
```

## Pin Assignments

```c		
		   LED1		LED2	BUTTON
* MSP430G2553	=> P1.0 	P1.6	P1.3
* MSP430F5529	=> P1.0 	P4.7	P2.1
* MSP430FR2311	=> P1.0 	P2.0	P1.1
* MSP430FR5994	=> P1.0 	P1.1	P5.6
* MSP430FR6989	=> P1.0 	P9.7	P1.1
```

## Code Example
Make an LED change states with a button using Timer modules within the MSP430F5529 

```c
	int main(void)
	{
    		WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    		//PM5CTL0 &= ~LOCKLPM5;

    		P1DIR  |= BIT0;                     // Set output Direction
    		P2REN  |= BIT1;                   // Enables resistor for P1.1
    		P2OUT  = BIT1;                    // Makes resistor P1.1 a pull up
    		P2IE |= BIT1;
   	        P2IES = 0;                     // Interrupts when 1.1 is pressed or low to high transition
    		P2IFG &= ~BIT1;                   // Clear interrupt flag
    		TA0CCTL0 = 0x0010;                  // Enables Capture/compare register for interrupt of timer A0
    		TA0CCR0 = 60000;                    // Set value for Timer A0 to count up to
    		TA0CTL = TASSEL_2 + MC_0;           // Timer_A Control Register - SMCLK high frequency clock, timer stopped

        	__enable_interrupt();

        	__bis_SR_register(LPM0 + GIE);      // Low Power Mode w/ general interrupts enabled

    	}

    		// Interrupt Service Routine 1 which toggles on the LED on the release of the Button and starts timer a to act as a delay
    		#pragma vector = PORT2_VECTOR
    		__interrupt void Interrupt_1(void)
    		{
        		P1OUT ^= BIT0;                      // Toggle LED
        		P2IE &= ~BIT1;                    // reset interrupt
        		TA0CTL = TASSEL_2 + MC_1;           // Timer_A Control Register - SMCLK high frequency clock, Up
    		}

    		// Interrupt Service Routine 2 which stops Timer A0 and resets the interrupt
    		#pragma vector=TIMER0_A0_VECTOR
    		__interrupt void Interrupt_2(void)
    		{
        		TA0CTL = MC_0;                      // Stop timer
        		P2IE |= BIT1;                     // Interrupt enable set to BUTTON
        		P2IFG &= ~BIT1;                     // Clear interrupt flag
    		}
```