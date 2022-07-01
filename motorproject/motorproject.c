#include "NU32.h" // config bits, constants, funcs for startup and UART
#include "LCD.h"
#include "utilities.h"
#include "encoder.h"
#include "currentcontrol.h"
#include "positioncontrol.h"
#include "ina219.h"
#include <stdio.h>
// include other header files here
#define BUF_SIZE 200

static volatile int mode;

static volatile float kp_cur = 0, ki_cur = 0;
static volatile float kp_pos = 0, ki_pos = 0, kd_pos = 0;

void __ISR(8, IPL5SOFT) CurrentController(){ //Timer 2 for current control 5kHz

    Current_Control(kp_cur,ki_cur);
    
    IFS0CLR = 0x1 << 8;  // clear the int flag
}

void __ISR(20, IPL4SOFT) TestCurrentController(){ //Timer 5 for current control test 5kHz

    
    Current_Control_Test(kp_cur,ki_cur);


    IFS0CLR = 0x1 << 20;  // clear the int flag
}

void __ISR(16, IPL6SRS) PositionController(void) { // Timer 4 for position control 200 Hz//
    
    Position_Control(kp_pos,ki_pos,kd_pos);

    IFS0CLR = 0x1 << 16; // clear interrupt flag
}

int main()
{
    static char buffer[BUF_SIZE];
    NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
    INA219_Startup();
    LCD_Setup();
    NU32_LED1 = 1; // turn off the LEDs
    NU32_LED2 = 1;
    __builtin_disable_interrupts();
    UART2_Startup();
    PWM_SetUp();
    // in future, initialize modules or peripherals here
    __builtin_enable_interrupts();
    while(1)
    {
        NU32_ReadUART3(buffer,BUF_SIZE); // we expect the next character to be a menu command
        NU32_LED2 = 1; // clear the error LED
        static char buff;
        buff = buffer[0];
        LCD_WriteMode(buff);
        switch (buff) {
            case 'a': // dummy command for demonstration purposes
            {
                static signed short raw_current;
                raw_current = readINA219(0x04);
                sprintf(buffer,"%d\r\n", raw_current); 
                NU32_WriteUART3(buffer);
                break;
            }
            case 'b':
            {
                static float current;
                current = INA219_read_current();
                sprintf(buffer,"%6.1f\r\n", current); 
                NU32_WriteUART3(buffer);
                break;
            }
            case 'c':
            {
                static int enc_count;
                enc_count = get_encoder_count();
                sprintf(buffer,"%d\r\n", enc_count); 
                NU32_WriteUART3(buffer);
                break;
            }
            case 'd':
            {
                static float enc_deg;
                enc_deg = Read_Encoder_Deg();
                sprintf(buffer,"%6.3f\r\n", enc_deg); 
                NU32_WriteUART3(buffer);
                break;
            break;
            }
            case 'e':
            {
                static int enc_reset;
                WriteUART2("b");
                enc_reset = get_encoder_count();
                sprintf(buffer,"%d\r\n", enc_reset); 
                NU32_WriteUART3(buffer);
                break;
            break;
            }
            case 'f':
            {
                static int pwm = 0;
                NU32_ReadUART3(buffer,BUF_SIZE);
                sscanf(buffer, "%d", &pwm);
                pwm = PWM_Set(pwm);
                sprintf(buffer,"%d\r\n", pwm); 
                NU32_WriteUART3(buffer);
            break;
            }
            case 'g':
            {
                __builtin_disable_interrupts();
                NU32_ReadUART3(buffer,BUF_SIZE);
                sscanf(buffer, "%f", &kp_cur);
                NU32_ReadUART3(buffer,BUF_SIZE);
                sscanf(buffer, "%f", &ki_cur);
                __builtin_enable_interrupts();

            break;
            }
            case 'h':
            {
                __builtin_disable_interrupts();
                sprintf(buffer,"%f\r\n", kp_cur); 
                NU32_WriteUART3(buffer);
                sprintf(buffer,"%f\r\n", ki_cur); 
                NU32_WriteUART3(buffer);
                __builtin_enable_interrupts();
            break;
            }
            case 'i':
            {
                __builtin_disable_interrupts();
                NU32_ReadUART3(buffer,BUF_SIZE);
                sscanf(buffer, "%f", &kp_pos);
                NU32_ReadUART3(buffer,BUF_SIZE);
                sscanf(buffer, "%f", &ki_pos);
                NU32_ReadUART3(buffer,BUF_SIZE);
                sscanf(buffer, "%f", &kd_pos);
                __builtin_enable_interrupts();
            break;
            }
            case 'j':
            {
                __builtin_disable_interrupts();
                sprintf(buffer,"%f\r\n", kp_pos); 
                NU32_WriteUART3(buffer);
                sprintf(buffer,"%f\r\n", ki_pos); 
                NU32_WriteUART3(buffer);
                sprintf(buffer,"%f\r\n", kd_pos); 
                NU32_WriteUART3(buffer);
                __builtin_enable_interrupts();
            break;
            }
            case 'k':
            {
                mode = 2;
                makeWaveform(); 
                Current_TestTimer_SetUp();

                while (mode == 2)
                {
                    mode = check_mode_current();
                }

                __builtin_disable_interrupts();
                Current_Plot();
                __builtin_enable_interrupts();
                
                
            break;
            }
            case 'l':
            {
                __builtin_disable_interrupts();
                Store_Traj();
                __builtin_enable_interrupts();
                mode = 4;
                WriteUART2("b");
                Current_TimerInterrupt_SetUp();
                Position_TimerInterrupt_SetUp();
                while (mode == 4)
                {
                    mode = check_mode_position();
                }
                __builtin_disable_interrupts();
                PWM_Set(0);
                __builtin_enable_interrupts();

            break;
            }
            case 'm':
            {
                __builtin_disable_interrupts();
                Store_Traj();
                __builtin_enable_interrupts();
            break;
            }
            case 'n':
            {
                __builtin_disable_interrupts();
                Store_Traj();
                __builtin_enable_interrupts();
            break;
            }
            case 'o':
            {
                mode = 4;
                WriteUART2("b");
                Current_TimerInterrupt_SetUp();
                Position_TimerInterrupt_SetUp();
                while (mode == 4)
                {
                    mode = check_mode_position();
                }
                __builtin_disable_interrupts();
                PWM_Set(0);
                Position_Plot();
                __builtin_disable_interrupts();
            break;
            }
            case 'p':
            {
                PWM_Set(0);
                mode = 0;
            break;
            }
            case 'q':
            {
                PWM_Set(0);
                mode = 0;
            break;
            }
            case 'r':
            {
            sprintf(buffer,"%d\r\n", mode); 
            NU32_WriteUART3(buffer);
            break;
            }
            default:
            {
            NU32_LED2 = 0; // turn on LED2 to indicate an error
            break;
            }
        }
    }
    return 0;   
}