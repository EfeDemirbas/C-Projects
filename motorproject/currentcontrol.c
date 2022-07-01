#include "currentcontrol.h"
#include "NU32.h"
#include "ina219.h"
#include "positioncontrol.h"
#include <stdio.h>
#include <xc.h>
#include <sys/attribs.h>

#define PLOTPTS 200 //Number of data points to send to MATLAB
#define NUMSAMPS 100 // number of points in waveform
#define NUMWAVES 4 // number of waves
#define BUF_SIZE 200


static int Waveform[NUMSAMPS*NUMWAVES]; // waveform
static int CURarray[NUMSAMPS*NUMWAVES]; //Sensor Signal

static volatile float Kp = 0 , Ki = 0;
static volatile int Eint = 0;
static volatile float ref = 0;

static volatile short int mode_cur;

void PWM_SetUp(){
    OC3CONbits.OCTSEL = 1; // Set output compare 3 to use timer 3.
    T3CONbits.TCKPS = 0;    
    PR3 = 3999;              
    TMR3 = 0;   
    OC3CONbits.OCM = 0b110;    
    OC3RS=0;
    OC3R=0;                 
    T3CONbits.ON = 1;       // 20kHz Timer 3 to be used woth OC3.
    OC3CONbits.ON = 1;      //I was using the OC1 pin for a button on my NU32 so I wired it to OC3. 
}

void Current_TimerInterrupt_SetUp(){

    IPC2 = 0x15;            // interrupt pririty to 5 and sub-priority to 0
    IFS0CLR = 0x1 << 8;            // clear the TM2 flag
    IEC0SET = 0x1 << 8;            // enable the TM2 interrupt

    T2CONbits.TCKPS = 2;    
    PR2 = 3999;              
    TMR2 = 0;                      
    T2CONbits.ON = 1;       // 5kHz Timer 2 for the interrupt.


}

void Current_TestTimer_SetUp(){

    IPC5 = 0x10;            // interrupt pririty to 5 and sub-priority to 0
    IFS0CLR = 1 << 20;            // clear the TM2 flag
    IEC0SET = 1 << 20;            // enable the TM2 interrupt

    T5CONbits.TCKPS = 2;    
    PR5 = 3999;              
    TMR5 = 0;                      
    T5CONbits.ON = 1;       // 5kHz Timer 2 for the interrupt.


}

int PWM_Set(int pwm){

    TRISBbits.TRISB9 = 0;   // Make the B9 pin output for motor direction.
    
    if (pwm > 0)
    {
        if (pwm > 101){pwm = 100;}
        LATBbits.LATB9 = 0;
        OC3RS = (pwm*(PR3+1))/100-1;
    }
    else if (pwm == 0)
    {
        LATBbits.LATB9 = 0;
        OC3RS=0;
    }
    else if (pwm < 0)
    {
        pwm = -pwm;
        if (pwm > 101){pwm = 100;}
        LATBbits.LATB9 = 1;
        OC3RS = (pwm*(PR3+1))/100-1;
    }
    
    return pwm;

}


void Current_Control_Test(float Kp_cur, float Ki_cur){

    Kp = Kp_cur;
    Ki = Ki_cur;

    static int counter = 0; // initialize counter once
    static float curval; 
    static float u;
    static float error;

    curval = INA219_read_current();

    error = Waveform[counter] - curval;

    Eint = Eint + error;

    if (Eint > 1000.0) {
    Eint = 1000.0;
    } else if (Eint < -1000.0) {
    Eint = -1000.0;
    }

    u = (Kp*error + Ki*(Eint));

    PWM_Set(u);

    CURarray[counter] = curval; // store data in global arrays

    counter++; // add one to counter every time ISR is entered

    if (counter >= NUMSAMPS*NUMWAVES) {
        counter = 0; // rollover counter over when end of waveform reached
        mode_cur = 0;
        Eint = 0;
        PWM_Set(0);
        T5CONbits.ON = 0;
    }
    
    LATFINV = 0x1;


}

void makeWaveform() {
    int i = 0, center = 0, A = 200;
    for (int j = 0; j <= NUMWAVES; j++)
    {
    for (int i = 0; i < NUMSAMPS; ++i) {
        if ( i < NUMSAMPS/2) {
            Waveform[i+j*NUMSAMPS] = center + A;
        } else {
            Waveform[i+j*NUMSAMPS] = center - A;
        }
    }
    }   
}

void Current_Plot(){
    static char message[BUF_SIZE];
    sprintf(message,"%d\n", NUMSAMPS*NUMWAVES);
    NU32_WriteUART3(message);
    for (int i=0; i<NUMSAMPS*NUMWAVES; i++) { // send plot data to MATLAB
        // when first number sent = 1, MATLAB knows we’re done
        sprintf(message, "%d %d \n", CURarray[i], Waveform[i]);
        NU32_WriteUART3(message);
    }
}
  
    

void Current_Control(float Kp_cur, float Ki_cur){// PI controller to be used with current control

    Kp = Kp_cur;
    Ki = Ki_cur;


    static float amps;
    static float error;
    static float out;

    amps = INA219_read_current();
    error = ref - amps;
    Eint = error + Eint;

    if (Eint > 1000.0) {
    Eint = 1000.0;
    } else if (Eint < -1000.0) {
    Eint = -1000.0;
    }

    out = (Kp*error + Ki*(Eint));

    PWM_Set(out);

}

int check_mode_current(){

    return mode_cur;

}

void cur_get(float ref_cur){

    ref = ref_cur;

}