// ADCTestMain.c
// Runs on LM4F120/TM4C123
// This program periodically samples ADC channel 1 and stores the
// result to a global variable that can be accessed with the JTAG
// debugger and viewed with the variable watch feature.
// Daniel Valvano
// October 20, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
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

// input signal connected to PE2/AIN1

#include "ADCSWTrigger.h"
#include "../tm4c123gh6pm.h"
#include "PLL.h"
#include "Nokia5110.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void Display_Info(volatile unsigned long cm);
unsigned int getCm(unsigned long ADCvalue);
unsigned int getPercent(unsigned long ADCvalue);
unsigned int getLookup(unsigned long ADCvalue);
unsigned int getAbs(int n);
void Delay(void);

volatile unsigned long ADCvalue;
volatile unsigned long  cm;
volatile unsigned long  pct;
volatile unsigned long  lookupCm;
// The digital number ADCvalue is a representation of the voltage on PE4 
// voltage  ADCvalue
// 0.00V     0
// 0.75V    1024
// 1.50V    2048
// 2.25V    3072
// 3.00V    4095
int main(void){unsigned long volatile delay;
	Nokia5110_Init();
  PLL_Init();                           // 80 MHz
  ADC0_InitSWTriggerSeq3_Ch1();         // ADC initialization PE2/AIN1
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF; // activate port F
  delay = SYSCTL_RCGC2_R;
  GPIO_PORTF_DIR_R |= 0x04;             // make PF2 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x04;          // disable alt funct on PF2
  GPIO_PORTF_DEN_R |= 0x04;             // enable digital I/O on PF2
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF0FF)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
  while(1){
    GPIO_PORTF_DATA_R |= 0x04;          // profile
    ADCvalue = ADC0_InSeq3();
    GPIO_PORTF_DATA_R &= ~0x04;
		cm = getCm(ADCvalue);
		lookupCm = getLookup(ADCvalue);
		pct = getPercent(ADCvalue);
		Display_Info(cm);
		Delay();
  }
}


unsigned int getCm(unsigned long ADCvalue){
	return -2.6238372 + 46332.8701/ADCvalue;
}

unsigned int getPercent(unsigned long ADCvalue){
	pct = ADCvalue/40;
	if (pct >= 100)	pct = 100;
	return pct;
}
	

void Display_Info(volatile unsigned long cm){
	
	Nokia5110_Clear();
  Nokia5110_OutString("Eqn:");
	Nokia5110_OutUDec(cm);
	
	Nokia5110_SetCursor(0, 2);
	Nokia5110_OutString("Tbl:");
	Nokia5110_OutUDec(lookupCm);
}

unsigned int getAbs(int n) 
{ 
  int const mask = n >> (sizeof(int) * 8 - 1); 
  return ((n + mask) ^ mask); 
} 

unsigned int getLookup(unsigned long ADCvalue){
	int adcOutput[15] = {3400, 2650, 2100, 1730, 1440, 1200, 1070, 960, 840, 790, 630, 650, 530};
	int distance[15] =  {  10,   15,   20,   25,   30,   35,   40,  45,  50,  55,  60,  65,  70};
	unsigned int closest = getAbs(ADCvalue-adcOutput[0]);
	unsigned int val = distance[0];
	unsigned int i = 0;
	
	for (i = 0; i < 15; i++){
		if (  (getAbs(adcOutput[i]-ADCvalue)) < closest ){
				closest = getAbs(ADCvalue-adcOutput[i]);
				val = distance[i];
		}
	}
	return val;
}
	
	
void Delay(void){unsigned long volatile time;
  time = 2*727240*50/91;  // 0.1sec
  while(time){
		time--;
  }
}