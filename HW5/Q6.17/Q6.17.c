#include "NU32.h"          
#include "LCD.h"

#define CORE_TICKS 40000000 // 40 M ticks (one second)

//Instead of using INT2 for this question I have wired a physical button to INT1, and I decided to use that button as you will see in the demo.

volatile int dummy = 0; //Variable to keep track of the state of the stopwatch, timing or idle.

void __ISR(7, IPL6SRS) Ext1ISR(void) { 


  int debounce;
  debounce = _CP0_GET_COUNT();
  while(_CP0_GET_COUNT() < debounce + CORE_TICKS/50){ ; } //Debounce algorithm same as Q6.16 but I needed 20ms, lower debounce waits caused occasional bounces.

  if(!PORTDbits.RD8){ //After debounce test



    NU32_LED1++;                  // Flip the LED's for visual que for when the button is pressed.
    NU32_LED2++;
    if (dummy%2 == 0) //Check if the dummy is divisible by 2 and the remainder will tell its state. 
    {
      _CP0_SET_COUNT(0); //Start of the timer, Set the core timer to 0.
      NU32_WriteUART3("Press the button again to stop the timer.\r\n");
 

    }
    if (dummy%2 == 1) //Check the dummy if it is not divisible by 2 it is time to stop the timer.
    {
      float time; //Variable for the passed time to be stored in.
      char msgUART[50] = {}; //String for the UART message.
      char msgLCD[16] = {}; //String for the LCD message.
      time = ((float) _CP0_GET_COUNT())/((float)CORE_TICKS); //Coversion form ticks to seconds.
      sprintf(msgUART,"%.5f seconds elapsed.\r\n",time);
      sprintf(msgLCD,"%.5f sec",time);
      NU32_WriteUART3(msgUART);
      NU32_WriteUART3("Press the right most button to start the timer.\r\n");
      LCD_Move(0,0); 
      LCD_WriteString("Time Elapsed:");
      LCD_Move(1,0);
      LCD_WriteString(msgLCD); //Line36 to line 43 is just printing the time to both LCD and the UART.


    }
    dummy++; //Add one to dummy after each succesfull button push.
  }

  IFS0bits.INT1IF = 0;            // clear interrupt flag for INT1


}


int main(void) {
  NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  LCD_Setup(); //LCD set up and functions
  __builtin_disable_interrupts(); // disable interrupts
  
  INTCONCLR = 0x2;                // INT1 triggers on falling edge
  IPC1bits.INT1IP = 6;            // interrupt priority 6
  IPC1bits.INT1IS = 0;            // interrupt priority 0
  IFS0bits.INT1IF = 0;            // clear the int flag
  IEC0bits.INT1IE = 1;            // enable the INT1 interrupt

  __builtin_enable_interrupts();  // enable interrupts

  NU32_WriteUART3("Press the right most button to start the timer.\r\n"); //Command to get everyhting started.
  while(1){ ; } //Infinite while loop.

  return 0; //Will never return.
}


