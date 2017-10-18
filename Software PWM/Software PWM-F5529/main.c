#include <msp430.h>

/*** Global Variable ***/
int Push_Button;
void main(void){

    /*** Watchdog timer and clock Set-Up ***/
    WDTCTL = WDTPW + WDTHOLD;       // Stop watchdog timer
    DCOCTL = 0;                     // Select lowest DCOx and MODx
    BCSCTL1 = CALBC1_1MHZ;          // Set range
    DCOCTL = CALDCO_1MHZ;           // Set DCO step + modulation

    /*** GPIO Set-Up ***/
    P1DIR |= BIT6;                  // P1.6 set as output (Green LED)
    P1SEL |= BIT6;                  // P1.6 selected Timer0_A Out1 output

    P1DIR |= BIT0;

    P1REN |= BIT3;                              //setup button 1.3 using pull up resistor and enable
    P1OUT |= BIT3;
    P1IE |= BIT3;
    P1IES |= BIT3;
    P1IFG &= ~BIT3;

    /*** Timer0_A Set-Up ***/
    TA0CCR0 |= 1000;                    // PWM period
    TA0CCR1 |= 500;                   // TA0CCR1 PWM duty cycle
    TA0CCTL1 |= OUTMOD_7;           // TA0CCR1 output mode = reset/set
    TA0CTL |= TASSEL_2 + MC_1;      // SMCLK, Up Mode (Counts to TA0CCR0)

    /*** Timer1_A Set-Up ***/
    TA1CCR0 |= 4000;                    // Counter value
    TA1CCTL0 |= CCIE;               // Enable Timer1_A interrupts
    TA1CTL |= TASSEL_2 + MC_1;      // SMCLK, Up Mode (Counts to TA1CCR0)

    _BIS_SR(LPM0_bits + GIE);       // Enter Low power mode 0 with interrupts enabled


}

#pragma vector=PORT1_VECTOR
__interrupt void PORT_1(void)
{
    if (Push_Button == 0)              //Falling-edge of button
{
    TA0CCR1 = TA0CCR1+100;               //Initialize value of TA1CCR0
    TA1CCTL0 = CAP;                 //Capture mode
    Push_Button = 1;
    P1OUT ^=  BIT0;
    if (TA0CCR1 == 1100)
                TA0CCR1 = 0;

}
else if (Push_Button == 1)         //Rising-edge of a button
{

    TA0CCR1 = TA0CCR1;
    P1OUT &=~  BIT0;
    Push_Button = 0;
}
    P1IES ^= BIT3;                      //toggle to set as rising edge
    P1IE |= BIT3;                       // Enable interrupt
    P1IFG &= ~(BIT3);

}
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_1A1(void) {
   TA1CTL &= ~(MC1 | MC0);     // Stop TimerA1
   P1IE |= BIT3;
 TA1CCTL0 &= ~CCIFG;
}
