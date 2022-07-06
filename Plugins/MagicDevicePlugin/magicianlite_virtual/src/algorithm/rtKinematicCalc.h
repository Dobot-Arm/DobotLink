#pragma once
#include "variable.h"

extern int InverseCal(TCPosition *pPositon, float theta2Ref, TRobotTheta *pTheta);
extern void ForwardCal(TRobotTheta *pTheta, TCPosition *pPostion);
extern unsigned char GetJointLimit(TRobotTheta *pTheta);



