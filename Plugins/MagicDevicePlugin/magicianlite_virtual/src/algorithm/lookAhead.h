#pragma once

#include "variable.h"
int CPLookAhead(void);
void CPMotion(float vi, float ve, float vm, float dx, float dy,float dz,float isLaserOn,float state, uint8_t cpMode);
