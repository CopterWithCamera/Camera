#ifndef __COPTER_DATATRANS_H
#define __COPTER_DATATRANS_H

#include "stm32f4xx.h"
#include "include.h"

void Camera_Receive_Handle(unsigned char data);
void Camera_Data_Send(void);

extern float height_ultra;		//ultra.relative_height*10
extern float height_LPF;		//sonar.displacement
extern float height_fusion;		//sonar_fusion.fusion_displacement.out

extern float roll;
extern float pitch;
extern float yaw;


#endif

