#ifndef __COPTER_DATATRANS_H
#define __COPTER_DATATRANS_H

#include "stm32f4xx.h"
#include "include.h"

void Copter_Receive_Handle(unsigned char data);
void Up_To_FC(void);

extern float height_ultra;		//ultra.relative_height*10
extern float height_LPF;		//sonar.displacement
extern float height_fusion;		//sonar_fusion.fusion_displacement.out


#endif

