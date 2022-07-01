#ifndef POSCON_H
#define POSCON_H

void Store_Traj();

void Position_TimerInterrupt_SetUp();
void Position_Control(float kp_pos,float ki_pos,float kd_pos);
void Position_Plot();
short int check_mode_position();
float PosToCur();

#endif