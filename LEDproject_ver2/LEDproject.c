#include "NU32.h" 
#include "LCD.h"         
#include <stdio.h>

#define PLOTPTS 200 //Number of data points to send to MATLAB
#define NUMSAMPS 1000 // number of points in waveform
#define DECIMATION 10 //Plot every 10th point
#define SAMPLE_TIME 10       // 10 core timer ticks = 250 ns


static volatile int Waveform[NUMSAMPS]; // waveform
static volatile int ADCarray[PLOTPTS]; //Sensor Signal
static volatile int REFarray[PLOTPTS]; //REference signal
static volatile int StoringData = 0; //flag for storing status

static volatile float Kp = 0, Ki = 0; // control gains

static volatile int Eint = 0;


void makeWaveform() {
  int i = 0, center = 500, A = 300; // square wave, fill in center value and amplitude
  for (i = 0; i < NUMSAMPS; ++i) {
    if ( i < NUMSAMPS/2) {
      Waveform[i] = center + A;
    } else {
      Waveform[i] = center - A;
    }
  }
}

void printGainsToLCD(){
  LCD_Clear();                              // clear LCD screen
  LCD_Move(0,0);
  char msg1[16]; char msg2[16];
  sprintf(msg1,"Kp = %7.4f",Kp);
  sprintf(msg2,"Ki = %7.4f",Ki);
  LCD_WriteString(msg1);                     // write msg at row 0 col 0
  LCD_Move(1,0);
  LCD_WriteString(msg2); 
}


unsigned int adc_sample_convert(int pin) { // sample & convert the value on the given 
                                           // adc pin the pin should be configured as an 
                                           // analog input in AD1PCFG
  unsigned int elapsed = 0, finish_time = 0;
  AD1CHSbits.CH0SA = pin;                // connect chosen pin to MUXA for sampling
  AD1CON1bits.SAMP = 1;                  // start sampling
  elapsed = _CP0_GET_COUNT();
  finish_time = elapsed + SAMPLE_TIME;
  while (_CP0_GET_COUNT() < finish_time) { 
    ;                                   // sample for more than 250 ns
  }
  AD1CON1bits.SAMP = 0;                 // stop sampling and start converting
  while (!AD1CON1bits.DONE) {
    ;                                   // wait for the conversion process to finish
  }
  return ADC1BUF0;                      // read the buffer with the result
}



void __ISR(8, IPL5SOFT) Controller(void) {
  static int counter = 0; // initialize counter once
  static int plotind = 0; // index for data arrays; counts up to PLOTPTS
  static int decctr = 0; // counts to store data one every DECIMATION
  static int adcval; 
  static int u;
  static int unew;

  adcval = adc_sample_convert(10);

  Eint = Eint + (Waveform[counter] - adcval);

  u = Kp*(Waveform[counter] - adcval) + Ki*(Eint);

  unew = u + 50.0;
  if (unew > 100.0) {
  unew = 100.0;
  } else if (unew < 0.0) {
  unew = 0.0;
  }

  OC3RS = (unsigned int) ((unew/100.0)*(PR3+1));

  if (StoringData) {
    decctr++;
    if (decctr == DECIMATION) { // after DECIMATION control loops,
    decctr = 0; // reset decimation counter
    ADCarray[plotind] = adcval; // store data in global arrays
    REFarray[plotind] = Waveform[counter];
    plotind++; // increment plot data index
    }
    if (plotind == PLOTPTS) { // if max number of plot points plot is reached,
    plotind = 0; // reset the plot index
    StoringData = 0; // tell main data is ready to be sent to MATLAB
    }
  }
  counter++; // add one to counter every time ISR is entered
  if (counter == NUMSAMPS) {
    counter = 0; // rollover counter over when end of waveform reached
  }
  IFS0CLR = 0x100;            // clear the int flag
}






int main(void) {
  NU32_Startup();   
  LCD_Setup();       

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

  char message[100]; // message to and from MATLAB
  float kptemp = 0, kitemp = 0; // temporary local gains
  int i = 0; // plot data loop counter


  AD1CON3bits.ADCS = 2;
  AD1PCFGbits.PCFG10 = 0;                 // AN10 is the adc pin that I wired
  AD1CON1bits.ADON = 1;                   // turn on A/D converter


  makeWaveform();


  __builtin_enable_interrupts();  // enable interrupts




  while (1) {
    NU32_ReadUART3(message, sizeof(message)); // wait for a message from MATLAB
    sscanf(message, "%f %f" , &kptemp, &kitemp);
    __builtin_disable_interrupts(); // keep ISR disabled as briefly as possible
    Kp = kptemp; // copy local variables to globals used by ISR
    Ki = kitemp;
    __builtin_enable_interrupts(); // only 2 simple C commands while ISRs disabled
    Eint = 0;
    StoringData = 1; // message to ISR to start storing data
    printGainsToLCD();
    while (StoringData) { // wait until ISR says data storing is done
    ; // do nothing
    }
    for (i=0; i<PLOTPTS; i++) { // send plot data to MATLAB
    // when first number sent = 1, MATLAB knows we’re done
    sprintf(message, "%d %d %d\r\n", PLOTPTS-i, ADCarray[i], REFarray[i]);
    NU32_WriteUART3(message);
    }
  }

  return 0;
}

