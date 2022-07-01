#include "NU32.h"          


int main(void) {
  NU32_Startup();          


  OC3CONbits.OCTSEL = 1; // Set output compare 3 to use timer 3.
  T3CONbits.TCKPS = 0;    
  PR3 = 3999;              
  TMR3 = 0;   
  OC3CONbits.OCM = 0b110;    
  OC3RS=3000;
  OC3R=3000;                 
  T3CONbits.ON = 1;       
  OC3CONbits.ON = 1;      //I was using the OC1 pin for LCD on my NU32 so I wired it to OC3. 

  while(1) { ; }

  return 0;
}

