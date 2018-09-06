/*
//Rosswell Tiongco
//016091762

// This is your first program to run on the LaunchPad
// You will run this program without modification as your Lab 2
// If the left switch SW1 is 
//      not pressed the LED toggles blue-red
//      pressed the LED toggles blue-green

// 0.Documentation Section 
// main.c
// Runs on LM4F120 or TM4C123
// Lab2_HelloLaunchPad, Input from PF4, output to PF3,PF2,PF1 (LED)
// Authors: Daniel Valvano, Jonathan Valvano and Ramesh Yerraballi
// Date: January 15, 2016

// LaunchPad built-in hardware
// SW1 left switch is negative logic PF4 on the Launchpad
// SW2 right switch is negative logic PF0 on the Launchpad
// red LED connected to PF1 on the Launchpad
// blue LED connected to PF2 on the Launchpad
// green LED connected to PF3 on the Launchpad

// 1. Pre-processor Directives Section
// Constant declarations to access port registers using 
// symbolic names instead of addresses
#include "TExaS.h"
#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))

// 2. Declarations Section
//   Global Variables
unsigned long In;  // input from PF4
unsigned long Out; // outputs to PF3,PF2,PF1 (multicolor LED)

//   Function Prototypes
void PortF_Init(void);
void Delay(void);
void EnableInterrupts(void);


	// 3. Subroutines Section
// MAIN: Mandatory for a C Program to be executable
int main(void){    
	TExaS_Init(SW_PIN_PF40,LED_PIN_PF321); // this initializes the TExaS grader lab 2
  PortF_Init();        // Call initialization of port PF4 PF2    
  EnableInterrupts();  // The grader uses interrupts
  while(1){
		In = GPIO_PORTF_DATA_R&0x10; // read PF4 into In
    if(In == 0x00){              // zero means SW1 is pressed
      GPIO_PORTF_DATA_R = 0x02;  // LED is green
		} else{                      // 0x10 means SW1 is not pressed
      GPIO_PORTF_DATA_R = 0x0A;  // LED is red
    }
    Delay();                     // wait 0.1 sec
    GPIO_PORTF_DATA_R = 0x04;    // LED is blue
    Delay();                     // wait 0.1 sec
  }
}

// Subroutine to initialize port F pins for input and output
// PF4 and PF0 are input SW1 and SW2 respectively
// PF3,PF2,PF1 are outputs to the LED
// Inputs: None
// Outputs: None
// Notes: These five pins are connected to hardware on the LaunchPad
void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0        
}
// Color    LED(s) PortF
// dark     ---    0
// red      R--    0x02
// blue     --B    0x04
// green    -G-    0x08
// yellow   RG-    0x0A
// sky blue -GB    0x0C
// white    RGB    0x0E
// pink     R-B    0x06

// Subroutine to wait 0.1 sec
// Inputs: None
// Outputs: None
// Notes: ...
void Delay(void){unsigned long volatile time;
  time = 2*727240*200/91;  // 0.1sec
  while(time){
		time--;
  }
}
*/




// DCMotor.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a software PWM to drive
// a DC motor at a given duty cycle.  The built-in button SW1
// increases the speed, and SW2 decreases the speed.
// Daniel Valvano, Jonathan Valvano
// August 6, 2013

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// PA5 connected to DC motor interface

#define GPIO_PORTA_DATA_R       (*((volatile unsigned long *)0x400043FC))
#define GPIO_PORTA_DIR_R        (*((volatile unsigned long *)0x40004400))
#define GPIO_PORTA_AFSEL_R      (*((volatile unsigned long *)0x40004420))
#define GPIO_PORTA_DR8R_R       (*((volatile unsigned long *)0x40004508))
#define GPIO_PORTA_DEN_R        (*((volatile unsigned long *)0x4000451C))
#define GPIO_PORTA_AMSEL_R      (*((volatile unsigned long *)0x40004528))
#define GPIO_PORTA_PCTL_R       (*((volatile unsigned long *)0x4000452C))
#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_IS_R         (*((volatile unsigned long *)0x40025404))
#define GPIO_PORTF_IBE_R        (*((volatile unsigned long *)0x40025408))
#define GPIO_PORTF_IEV_R        (*((volatile unsigned long *)0x4002540C))
#define GPIO_PORTF_IM_R         (*((volatile unsigned long *)0x40025410))
#define GPIO_PORTF_RIS_R        (*((volatile unsigned long *)0x40025414))
#define GPIO_PORTF_ICR_R        (*((volatile unsigned long *)0x4002541C))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520))
#define GPIO_LOCK_KEY           0x4C4F434B  // Unlocks the GPIO_CR register
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define NVIC_ST_CTRL_R          (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R        (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R       (*((volatile unsigned long *)0xE000E018))
#define NVIC_EN0_R              (*((volatile unsigned long *)0xE000E100))
#define NVIC_PRI7_R             (*((volatile unsigned long *)0xE000E41C))
#define NVIC_SYS_PRI3_R         (*((volatile unsigned long *)0xE000ED20))

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

unsigned long H,L;
void Motor_Init(void){
  SYSCTL_RCGC2_R |= 0x00000001; // activate clock for port A
  H = L = 40000;                // 50%
  GPIO_PORTA_AMSEL_R &= ~0x20;      // disable analog functionality on PA5
  GPIO_PORTA_PCTL_R &= ~0x00F00000; // configure PA5 as GPIO
  GPIO_PORTA_DIR_R |= 0x20;     // make PA5 out
  GPIO_PORTA_DR8R_R |= 0x20;    // enable 8 mA drive on PA5
  GPIO_PORTA_AFSEL_R &= ~0x20;  // disable alt funct on PA5
  GPIO_PORTA_DEN_R |= 0x20;     // enable digital I/O on PA5
  GPIO_PORTA_DATA_R &= ~0x20;   // make PA5 low
  NVIC_ST_CTRL_R = 0;           // disable SysTick during setup
  NVIC_ST_RELOAD_R = L-1;       // reload value for 500us
  NVIC_ST_CURRENT_R = 0;        // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // priority 2
  NVIC_ST_CTRL_R = 0x00000007;  // enable with core clock and interrupts
}

void LED_Init(void){unsigned long volatile delay;
	SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0       
}


void SysTick_Handler(void){
  if(GPIO_PORTA_DATA_R&0x20){   // toggle PA5
    GPIO_PORTA_DATA_R &= ~0x20; // make PA5 low
    NVIC_ST_RELOAD_R = L-1;     // reload value for low phase
  } else{
    GPIO_PORTA_DATA_R |= 0x20;  // make PA5 high
    NVIC_ST_RELOAD_R = H-1;     // reload value for high phase
  }
}
void Switch_Init(void){  unsigned long volatile delay;
  SYSCTL_RCGC2_R |= 0x00000020; // (a) activate clock for port F
  delay = SYSCTL_RCGC2_R;
  GPIO_PORTF_LOCK_R = 0x4C4F434B; // unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x11;         // allow changes to PF4,0
  GPIO_PORTF_DIR_R &= ~0x11;    // (c) make PF4,0 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x11;  //     disable alt funct on PF4,0
  GPIO_PORTF_DEN_R |= 0x11;     //     enable digital I/O on PF4,0
  GPIO_PORTF_PCTL_R &= ~0x000F000F; //  configure PF4,0 as GPIO
  GPIO_PORTF_AMSEL_R &= ~0x11;  //     disable analog functionality on PF4,0
  GPIO_PORTF_PUR_R |= 0x11;     //     enable weak pull-up on PF4,0
  GPIO_PORTF_IS_R &= ~0x11;     // (d) PF4,PF0 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x11;    //     PF4,PF0 is not both edges
  GPIO_PORTF_IEV_R &= ~0x11;    //     PF4,PF0 falling edge event
  GPIO_PORTF_ICR_R = 0x11;      // (e) clear flags 4,0
  GPIO_PORTF_IM_R |= 0x11;      // (f) arm interrupt on PF4,PF0
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00400000; // (g) priority 2
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
}
// L range: 8000,16000,24000,32000,40000,48000,56000,64000,72000
// power:   10%    20%   30%   40%   50%   60%   70%   80%   90%
void GPIOPortF_Handler(void){ // called on touch of either SW1 or SW2
  if(GPIO_PORTF_RIS_R&0x01){  // SW2 touch
    GPIO_PORTF_ICR_R = 0x01;  // acknowledge flag0
    if(L>8000) L = L-8000;    // slow down
  }
  if(GPIO_PORTF_RIS_R&0x10){  // SW1 touch
    GPIO_PORTF_ICR_R = 0x10;  // acknowledge flag4
    if(L<72000) L = L+8000;   // speed up
  }
  H = 80000-L; // constant period of 1ms, variable duty cycle
}

int main(void){	
  DisableInterrupts();  // disable interrupts while initializing
  Motor_Init();         // output from PA5, SysTick interrupts
  Switch_Init();        // arm PF4, PF0 for falling edge interrupts
  EnableInterrupts();   // enable after all initialization are done
	LED_Init();
  while(1){
    // main program is free to perform other tasks
    WaitForInterrupt(); // low power mode
  }
}

