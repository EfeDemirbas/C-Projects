#include "NU32.h"          // constants, funcs for startup and UART
#include "LCD.h"
#include<stdio.h>

#define MSG_LEN 32

int main() {
  char msg[MSG_LEN];
  char msg1[MSG_LEN/2];
  char msg2[MSG_LEN/2];
  int nreceived = 1;
  char line[5]={};
  int ln=0;

  NU32_Startup();         // cache on, interrupts on, LED/button init, UART init
  
  LCD_Setup();


  
  while (1) {
    NU32_WriteUART3("What do you want to write? ");
    NU32_ReadUART3(msg, MSG_LEN);             // get the response
    for (char i = 0; i < 16; i++)
    {
      msg1[i]=msg[i];
    }
    for (char i = 0; i < 16; i++)
    {
      msg2[i]=msg[i+16];
    }
    
    LCD_Clear();                              // clear LCD screen
    LCD_Move(0,0);
    LCD_WriteString(msg1);                     // write msg at row 0 col 0
    LCD_Move(1,0);
    LCD_WriteString(msg2);                     // write msg at row 0 col 0
    NU32_WriteUART3("\r\n");   
    NU32_WriteUART3("Which line would you want to delete?");
    NU32_ReadUART3(line, 5);
    sscanf(line,"%d", &ln);
    NU32_WriteUART3("\r\nPush the user button to delete the selected line.");
    while(PORTDbits.RD7){
    }
    LCD_ClearLine(ln);
    NU32_WriteUART3("\r\n");




  }
  return 0;
}
