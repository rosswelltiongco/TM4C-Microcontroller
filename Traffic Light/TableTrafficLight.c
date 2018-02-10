//Rosswell Tiongco 016091762
// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay(void);
void Init_PortB(void);	
void Init_PortE(void);
// ***** 3. Subroutines Section *****
	
int main(void){
  //TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
 
	unsigned int In;
	unsigned int In2;
	unsigned int y;
	Delay();
	
	//Initializing ports
	Init_PortB();
	Init_PortE();
	
	//Initialization
	GPIO_PORTB_DATA_R = 0x00;   //Clear all
	GPIO_PORTB_DATA_R |= 0x04;  //Turns on red
	GPIO_PORTB_DATA_R &= ~0x03; //Turns off yellow and green
	//Method 2:
	//GPIO_PORTB_DATA_R &= ~0x07;
	//GPIO_PORTB_DATA_R |= 0x04;
	

	
  //EnableInterrupts();
  while(1){
		
		 //Switching logic
		 In = GPIO_PORTE_DATA_R&0x01; //Iniitialize to first bit of Port E(Bit 1)
		 if(In == 0x01){              // If sw1 pressed
				//Reset green to start @ red
			  if (GPIO_PORTB_DATA_R == 0x01){
						//Delay();
						GPIO_PORTB_DATA_R = 0x04;  //Turns on red
						Delay();  //Needed, since red will essentially turn on, but immediately turn off
				}
				else{
						GPIO_PORTB_DATA_R >>= 0x01; //Shift right
						Delay();
				}
				
		 }
		 
		 
		 //Flashing logic
		 In2 = GPIO_PORTE_DATA_R&0x02;
		 if(In2 == 0x02){              // zero means SW2 is pressed
			 y = GPIO_PORTB_DATA_R;
			 GPIO_PORTB_DATA_R ^= GPIO_PORTB_DATA_R;
			 Delay();
			 GPIO_PORTB_DATA_R = y;
			 Delay();
			 
		 }
  }
}
void Init_PortB(void){
	unsigned int delay;
  SYSCTL_RCGC2_R |= 0x00000002;     // 1) B clock
  delay = SYSCTL_RCGC2_R;           // delay   
  //GPIO_PORTB_CR_R = 0x07;           // allow changes to PB2-0      
  GPIO_PORTB_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTB_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTB_DIR_R = 0x07;          // 5) PB2-output 
  GPIO_PORTB_AFSEL_R = 0x00;        // 6) no alterna=te function
  //GPIO_PORTB_PUR_R = 0x00;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTB_DEN_R = 0x07;          // 7) enable digital pins PB2-PB0        
}

void Init_PortE(void){
	unsigned int delay;
  SYSCTL_RCGC2_R |= 0x00000010;     // 1) E clock
  delay = SYSCTL_RCGC2_R;           // delay   
  //GPIO_PORTE_CR_R = 0x03;           // allow changes to PE-0       
  GPIO_PORTE_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTE_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTE_DIR_R = 0xFC;         // 5) PE2-output 
  GPIO_PORTE_AFSEL_R = 0x00;        // 6) no alternate function
 // GPIO_PORTE_PUR_R = 0x00;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTE_DEN_R = 0x03;          // 7) enable digital pins PFE-PF0  
}


void Delay(void){unsigned long volatile time;
  time = 2*727240*50/91;  // 0.1sec
  while(time){
		time--;
  }
}