#include "NU32.h"          

#define NUMSAMPS 1000 // number of points in waveform
static volatile int Waveform[NUMSAMPS]; // waveform

void makeWaveform() {
int i = 0, center = (PR3+1)/2, A = (PR3+1)/4; // square wave, fill in center value and amplitude
for (i = 0; i < NUMSAMPS; ++i) {
if ( i < NUMSAMPS/2) {
Waveform[i] = center + A;
} else {
Waveform[i] = center - A;
}
}
}

void __ISR(8, IPL5SOFT) Controller(void) { // _TIMER_2_VECTOR = 8
static int counter = 0; // initialize counter once

makeWaveform();
OC3RS = Waveform[counter];

counter++; // add one to counter every time ISR is entered
if (counter == NUMSAMPS) {
counter = 0; // roll the counter over when needed
}
  IFS0CLR = 0x100;            // clear the int flag
}

int main(void) {
  NU32_Startup();          

  __builtin_disable_interrupts(); // disable interrupts
  
  IPC2 = 0x14;            // interrupt pririty to 5 and sub-priority to 0
  IFS0CLR = 0x100;            // clear the TM2 flag
  IEC0SET = 0x100;            // enable the TM2 interrupt


  OC3CONbits.OCTSEL = 1; // Set output compare 3 to use timer 3.
  T3CONbits.TCKPS = 0;    
  PR3 = 3999;              
  TMR3 = 0;   
  OC3CONbits.OCM = 0b110;    
  OC3RS=0;
  OC3R=0;                 
  T3CONbits.ON = 1;       
  OC3CONbits.ON = 1;      //I was using the OC1 pin for LCD on my NU32 so I wired it to OC3. 



  T2CONbits.TCKPS = 2;    
  PR2 = 19999;              
  TMR2 = 0;                      
  T2CONbits.ON = 1;       // 1kHz Timer 2 for the interrupt.


  __builtin_enable_interrupts();  // enable interrupts



  while(1) { ; }

  return 0;
}

