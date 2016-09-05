#pragma once

#include "windows.h"

class FaceTrackingFrameRateCalculator
{
private:
	LARGE_INTEGER frequency;
	LARGE_INTEGER previousTime;
	LARGE_INTEGER currentTime;
	int currentlyCalclatedFrameRate;
	int frameRate;
	bool isFrameRateReady;
	// hi 
public:
	FaceTrackingFrameRateCalculator();
	bool IsFrameRateReady();
	int GetFrameRate();
	void Tick();
	int currTime;

};
