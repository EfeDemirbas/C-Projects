#ifndef CURCON_H
#define CURCON_H

void PWM_SetUp();
int PWM_Set(int pwm);
void Current_TimerInterrupt_SetUp();
void Current_Control_Test(float Kp_cur, float Ki_cur);
void Current_Control(float Kp_cur, float Ki_cur);
void makeWaveform();
int check_mode_current();
void Current_Plot();
void Current_TestTimer_SetUp();
void cur_get(float ref_cur);

#endif