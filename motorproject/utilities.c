#include "utilities.h"
#include "LCD.h"
#include <stdio.h>


void LCD_WriteMode(char buff){
    static char msg1[16];
    static char msg2[16];
    sprintf(msg1,"Selected Mode= %c",buff);
    sprintf(msg2,"     (o_O)     ");
    LCD_Clear();                              // clear LCD screen
    LCD_Move(0,0);
    LCD_WriteString(msg1);                     // write msg at row 0 col 0
    LCD_Move(1,0);
    LCD_WriteString(msg2);     
}