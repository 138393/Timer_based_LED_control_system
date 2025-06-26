#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "msp.h"

#define DELAY_DEB 100000 //Debouncing

uint16_t timer_values[2] = {24575, 49151};
int which_led = 1;
int mode = 1;

int main() {
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

  /* Port 1 Pin Config GPIO */
	P1SEL0 &=(uint8_t)(~((1<<4) | (1<<1) | (1<<0)));
	P1SEL1 &=(uint8_t)(~((1<<4) | (1<<1) | (1<<0)));

  /* Port 2 Pin Config GPIO */
	P2SEL0 &=(uint8_t)(~((1<<2) | (1<<1) | (1<<0)));
	P2SEL1 &=(uint8_t)(~((1<<2) | (1<<1) | (1<<0)));

  /*Port 1 P1, P4 as inputs and P0 as output */
	P1DIR &=(uint8_t)(~((1<<4) | (1<<1)));
	P1DIR |=(uint8_t)(1<<0);

  /*Port 2 P0, P1, P2 as inputs */
	P2DIR |=(uint8_t)(((1<<2) | (1<<1) | (1<<0)));

  /*Port 1 and Port 2 config active high outputs*/
	P1OUT &=~(uint8_t)(1<<0);	
	P2OUT &=~(uint8_t)(((1<<2) | (1<<1) | (1<<0)));

  /*Port 1 config active low inputs*/
	P1OUT |=(uint8_t)((1<<4) | (1<<1));

	/*Port 1 pull up resistor*/
	P1REN |=(uint8_t)((1<<4) | (1<<1));

  /*Device interrupt configuration*/
  P1IES |= (uint8_t)(0x02 | 0x10);
	P1IFG &= (uint8_t)~(0x02 | 0x10); 
	P1IE |= (uint8_t)(0x02 | 0x10); 

  /*NVIC GPIO configuration*/
  NVIC_SetPriority(PORT1_IRQn, 2);
	NVIC_ClearPendingIRQ(PORT1_IRQn);
	NVIC_EnableIRQ(PORT1_IRQn);

  /*NVIC timer configuration*/
  NVIC_SetPriority(TA0_N_IRQn, 3);
	NVIC_ClearPendingIRQ(TA0_N_IRQn);
	NVIC_EnableIRQ(TA0_N_IRQn);
	
	TA0CTL &= (uint16_t)(~(BIT5 | BIT4)); //stop timer
	TA0CTL |= (uint16_t)(BIT8);
	TA0CTL &= (uint16_t)(~(BIT9)); //ACLCK 01
	TA0CTL |= (uint16_t)(BIT6);    //"/2" 01
	TA0CTL &= (uint16_t)(~(BIT7));	
	
	TA0CTL &= (uint16_t)(~BIT0); //clear interrupt flag
	TA0CCR0 = (uint16_t)(24575); //Regular Mode
	TA0CTL |= (uint16_t)(BIT1); //interrupt enable
	TA0CTL |= (uint16_t)(BIT4); //up mode (count to CCR0)

  /*Globally enable interrupts in CPU*/
  __ASM("CPSIE I");

  while (1) 
  {}

	return 0;
}

void PORT1_IRQHandler(void) 
{	
	static int index = 0;
	//clear interrupt flag
	
	if (!(P1IN&(uint8_t)(1<<1))) {
		P1IFG &= (uint16_t)(~BIT1);
		if(which_led){ 
			which_led = 0; 
		}else {which_led = 1;
		}
	}
	else if (!(P1IN&(uint8_t)(1<<4))){
		P1IFG &= (uint16_t)(~BIT4); //clear port 1 interrupgt flag 
		mode = (mode+1)%3;	
	
		if(!(TA0CTL & (uint16_t)(BIT1))) {
			TA0CTL |= (uint16_t)(BIT1); //interrupt enable
			TA0CTL |= (uint16_t)(BIT4); //up mode (count to CCR0)
		}
			
		if (mode==0){
			TA0CTL &= (uint16_t)(~(BIT5 | BIT4)); //stop timer
			TA0CTL &= (uint16_t)(~BIT1); //interrupt disable
			P1OUT &=~(uint8_t)(1<<0);	
			P2OUT &=~(uint8_t)(((1<<2) | (1<<1) | (1<<0)));
		}

		else if (mode == 1){
			TA0CCR0 = (uint16_t)(timer_values[0]);
		}
		else if (mode==2){
			TA0CCR0 = (uint16_t)(timer_values[1]);
		}
	}
	
}

void TA0_N_IRQHandler(void) 
{
	static int rgb_state = 0;
	
	TA0CTL &= (uint16_t)(~BIT0); //
	if (which_led){
		P1OUT ^=(uint8_t)(1<<0);
	}
	else{
		rgb_state++;
		if (rgb_state == 1) {
                    P2OUT |= (uint8_t)(1<<0); //Red on
    } 
		else if (rgb_state == 2) {
                    P2OUT &= ~(uint8_t)(1<<0); // Red off
                    P2OUT |= (uint8_t)(1<<1); // Green on
                } 
		else if (rgb_state == 3) {
                    P2OUT |= (uint8_t)(1<<0); // Red on
                    P2OUT |= (uint8_t)(1<<1); // Green on
                } 
		else if (rgb_state == 4) {
                    P2OUT &= ~(uint8_t)(1<<0); // Red off
                    P2OUT &= ~(uint8_t)(1<<1); // Green off
                    P2OUT |= (uint8_t)(1<<2); // Blue on
                } 
		else if (rgb_state == 5) {
                    P2OUT |= (uint8_t)(1<<0); // Red on
                    P2OUT |= (uint8_t)(1<<2); // Blue on 
                } 
		else if (rgb_state == 6) {
                    P2OUT &= ~(uint8_t)(1<<0); // Red off
                    P2OUT |= (uint8_t)(1<<1); // Green on
                    P2OUT |= (uint8_t)(1<<2); // Blue on 
                } 
		else if (rgb_state == 7) {
                    P2OUT |= (uint8_t)(1<<0); // Red on
                    P2OUT |= (uint8_t)(1<<1); // Green on
                    P2OUT |= (uint8_t)(1<<2); // Blue on
                } 
		else if (rgb_state > 7) {
                    rgb_state = 0;
                    P2OUT &= ~(uint8_t)((1 << 0) | (1 << 1) | (1 << 2));  // Reset RGB
                }
	}
}