#include "positioncontrol.h"
#include "NU32.h"
#include "encoder.h"
#include <stdio.h>
#include <xc.h>
#include "currentcontrol.h"


#define BUF_SIZE 200


static char buffer[BUF_SIZE];

static volatile float Kp = 0 , Ki = 0, Kd = 0;
static volatile float Eint = 0, Eder = 0;


static volatile float pos_to_cur = 0;


static int traj_length;
static float trajectory[2000];
static float ENCarray[2000];


static short int mode = 4;


void Store_Traj(){
    static float dummy;
    traj_length = 0;
    NU32_ReadUART3(buffer,BUF_SIZE);
    sscanf(buffer, "%d", &traj_length);
    for (int i = 0; i < traj_length; i++)
    {
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%f", &dummy);
        trajectory[i] = dummy;
    }
}


void Position_TimerInterrupt_SetUp(){

    __builtin_disable_interrupts();

    IPC4 = 0x18;            // interrupt pririty to 6 SRS and sub-priority to 0
    IFS0CLR = 0x1 << 16;            // clear the TM4 flag
    IEC0SET = 0x1 << 16;            // enable the TM4 interrupt

    T4CONbits.TCKPS = 6;    
    PR4 = 6249;              
    TMR4 = 0;                      
    T4CONbits.ON = 1;       // 200Hz Timer 2 for the interrupt.

    __builtin_enable_interrupts();


}

void Position_Control(float kp_pos,float ki_pos,float kd_pos){

    Kp = kp_pos;
    Ki = ki_pos;
    Kd = kd_pos;
 
    static int counter = 0; // initialize counter once

    static float encval; 
    static float error;
    static float error_prev = 0;
    static float u;

    encval = Read_Encoder_Deg();
 
    error = trajectory[counter] - encval;

    Eint = Eint + error;

    if (Eint > 100.0) {
    Eint = 100.0;
    } else if (Eint < -100.0) {
    Eint = -100.0;
    }

    Eder = error - error_prev;

    u = Kp*error + Ki*Eint + Kd*Eder;



    cur_get(-u);

    //PWM_Set(-u);

    ENCarray[counter] = encval; // store data in global arrays

    counter++; // add one to counter every time ISR is entered

    if (counter == traj_length) {
        counter = 0; // rollover counter over when end of waveform reached
        T4CONbits.ON = 0;       // 200Hz Timer 4 for the interrupt.
        T2CONbits.ON = 0;       // 5kHz Timer 2 for the interrupt.
        PWM_Set(0);
        cur_get(0);
        mode = 3;        
        error = 0;
        error_prev = 0;
        Eint = 0;
    }
    
    error_prev = error;
    LATFINV = 0x1;



}





void Position_Plot(){
    static char message[BUF_SIZE];
    sprintf(message,"%d\n", traj_length);
    NU32_WriteUART3(message);
    for (int i=0; i<traj_length; i++) { // send plot data to MATLAB
        // when first number sent = 1, MATLAB knows we’re done
        sprintf(message, "%f %f \n", ENCarray[i], trajectory[i]);
        NU32_WriteUART3(message);
    }
    mode = 4;
}

short int check_mode_position(){

    return mode;

}