#include "TExaS.h"
#include "tm4c123gh6pm.h"


//Global variables
int FallingEdges = 0;
int last,current= 0;
char status = 'R'; //Initialize to red status
int counter = 0;
unsigned int timer_count = 0;
unsigned int y;
unsigned int count1=0;
unsigned int count2=0;
unsigned int winner = 0;

//Function Prototypes
void EnableInterrupts(void);
void WaitForInterrupt(void);  // low power mode
void PortF_Init(void);
void GPIOPortF_Handler(void);
//void GPIOPortB_Handler(void);
void SysTick_Init(unsigned long period);
void delay(unsigned long int time)    // This function provides delay in terms of seconds
{
		//Roughly 1 second delay on 16MHz TM4C
    unsigned char i,j,k,l;
 
    for(i = 0; i < time; i++){
        for(j=0; j<250; j++){
					for(k=0; k< 250; k++){
						for (l=0; l< 60; l++){
						}
					}
				}
		}
}

//Subroutines
int main(void){
	PortF_Init();									//Port F Init (Onboard switches and leds)
	SysTick_Init(8000000);        // initialize SysTick timer to half a second each
	EnableInterrupts();           // Global Interrupt enable
	GPIO_PORTF_DATA_R = 0x02;  // LED is red at start
	timer_count = 0;
  while(1){	
		if (count1 >= 3){
			GPIO_PORTF_DATA_R = 0x08;  // Player 1 wins: Green
			delay(3);
			GPIO_PORTF_DATA_R = 0x02;  // LED is red at start
			delay(90);
		}
		if (count2 >= 3){
			GPIO_PORTF_DATA_R = 0x04;  // Player 2 wins: Blue
			delay(3);
			GPIO_PORTF_DATA_R = 0x02;  // LED is red at start
			delay(90);
		}
		WaitForInterrupt();
  } //While(1)
} //Main


void SysTick_Handler(void){
	//Reset all players' scores if time is exceeded
	timer_count++;
	if (timer_count >= 12){
		timer_count = 0;
		count1=0;
		count2=0;
	}
}


void PortF_Init(void){    
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R |= 0x1F;           // allow changes to PF4-0       
	GPIO_PORTF_DIR_R &= ~0x11;    // (c) make PF4(sw2), PF0(sw1) inputs (built-in button)
  GPIO_PORTF_DIR_R |=  0x0E;    //  make PF3,PF2,PF1 output+++
  GPIO_PORTF_AFSEL_R &= ~0x11;  //     disable alt funct on PF4
  GPIO_PORTF_DEN_R |= 0x1F;     //     enable digital I/O on PF4 - PF0  
  GPIO_PORTF_PCTL_R &= ~0x000F0000; // configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R &= ~0x11;       //     disable analog functionality on PF
  GPIO_PORTF_PUR_R |= 0x11;     //     enable weak pull-up on PF4
	GPIO_PORTF_IS_R &= ~0x11;     // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x11;    //     PF4 is not both edges
  GPIO_PORTF_IEV_R &= ~0x11;    //     PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x11;      // (e) clear flag4
  GPIO_PORTF_IM_R |= 0x11;      // (f) arm interrupt on PF4
  NVIC_PRI7_R |= (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R |= 0x40000000;      // (h) enable interrupt 30 in NVIC
}



void SysTick_Init(unsigned long period){
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = period-1;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // priority 2
                              // enable SysTick with core clock and interrupts
  NVIC_ST_CTRL_R = 0x07;
}


//Dual push button ISR
void GPIOPortF_Handler(void){
  if(GPIO_PORTF_RIS_R&0x01){  // SW2 touch
    GPIO_PORTF_ICR_R = 0x01;  // acknowledge flag0
    count1 = count1 + 1;
  }
  if(GPIO_PORTF_RIS_R&0x10){  // SW1 touch
    GPIO_PORTF_ICR_R = 0x10;  // acknowledge flag4
    count2 = count2 + 1;
	}
}

//Msc. Information
//Port F Onboard LED Color Codes
// Color    LED(s) PortF
// dark     ---    0
// red      R--    0x02
// blue     --B    0x04
// green    -G-    0x08
// yellow   RG-    0x0A
// sky blue -GB    0x0C	
// white    RGB    0x0E
// pink     R-B    0x06
