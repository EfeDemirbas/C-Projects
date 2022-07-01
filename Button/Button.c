#include "NU32.h"          // constants, funcs for startup and UART
#include "LCD.h"

void __ISR(3, IPL2SOFT) Ext0ISR(void) { // step 1: the ISR
  char msg[16]={};
  LCD_Clear();                              // clear LCD screen
  LCD_Move(0,0);
  sprintf(&msg,"Button 1");
  LCD_WriteString(msg);  

  NU32_LED1 = 0;                  // LED1 and LED2 on
  NU32_LED2 = 0;
  _CP0_SET_COUNT(0);
  
  while(_CP0_GET_COUNT() < 10000000) { ; } // delay for 10 M core ticks, 0.25 s
  
  NU32_LED1 = 1;                  // LED1 and LED2 off
  NU32_LED2 = 1;
  IFS0bits.INT0IF = 0;            // clear interrupt flag IFS0<3>
}

void __ISR(7, IPL3SOFT) Ext1ISR(void) { // step 1: the ISR
  char msg[16]={};
  LCD_Clear();                              // clear LCD screen
  LCD_Move(0,0);
  sprintf(&msg,"Button 3");
  LCD_WriteString(msg);  

  NU32_LED1 = 1;                  // LED1 and LED2 on
  NU32_LED2 = 0;
  _CP0_SET_COUNT(0);
  
  while(_CP0_GET_COUNT() < 10000000) { ; } // delay for 10 M core ticks, 0.25 s
  
  NU32_LED1 = 1;                  // LED1 and LED2 off
  NU32_LED2 = 1;
  IFS0bits.INT1IF = 0;            // clear interrupt flag IFS0<3>
}

void __ISR(19, IPL4SOFT) Ext4ISR(void) { // step 1: the ISR
  char msg[16]={};
  LCD_Clear();                              // clear LCD screen
  LCD_Move(0,0);
  sprintf(&msg,"Button 2");
  LCD_WriteString(msg);  

  NU32_LED1 = 0;                  // LED1 and LED2 on
  NU32_LED2 = 1;
  _CP0_SET_COUNT(0);
  
  while(_CP0_GET_COUNT() < 10000000) { ; } // delay for 10 M core ticks, 0.25 s
  
  NU32_LED1 = 1;                  // LED1 and LED2 off
  NU32_LED2 = 1;
  IFS0bits.INT4IF = 0;            // clear interrupt flag IFS0<3>
}

int main(void) {
  NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  LCD_Setup();
  __builtin_disable_interrupts(); // step 2: disable interrupts
  INTCONSET = 0x13;          // step 3: INT0 triggers on falling edge

  IPC0bits.INT0IP = 2;            // step 4: interrupt priority 2
  IPC0bits.INT0IS = 1;            // step 4: interrupt priority 1
  IFS0bits.INT0IF = 0;            // step 5: clear the int flag
  IEC0bits.INT0IE = 1;  

  IPC1bits.INT1IP = 3;            // step 4: interrupt priority 2
  IPC1bits.INT1IS = 0;            // step 4: interrupt priority 1
  IFS0bits.INT1IF = 0;            // step 5: clear the int flag
  IEC0bits.INT1IE = 1; 

  IPC4bits.INT4IP = 4;            // step 4: interrupt priority 2
  IPC4bits.INT4IS = 0;            // step 4: interrupt priority 1
  IFS0bits.INT4IF = 0;            // step 5: clear the int flag
  IEC0bits.INT4IE = 1;        // step 6: enable INT0 by setting IEC0<3>
  __builtin_enable_interrupts();  // step 7: enable interrupts
                                  // Connect RD7 (USER button) to INT0 (RD0)
  while(1) {
      ; // do nothing, loop forever
  }

  return 0;
}


