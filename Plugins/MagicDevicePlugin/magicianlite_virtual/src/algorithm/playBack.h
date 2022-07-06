#pragma once

#include "variable.h"

void Playback(float x, float y, float z,  float r,int type);
void ClearSpeed(void);
void PlaybackStop(void);
void Stop(void);
float GetPlaybackTime(float x, float y, float z,  float r,int type);
