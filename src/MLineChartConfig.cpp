#include <afx.h>
#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <windows.h>
#include "MLineChartConfig.h"

MLineChartConfig::MLineChartConfig(void)
{
	nInitWidth = 10;
	nWidthExtent = 10;

	crDot = RGB(255, 0, 0);
	crDataLine = RGB(255, 100, 100);
	crBG = RGB(255, 255, 255);
	crGuideLine = RGB(128, 128, 128);
	crAxis = 0;

	bShowGuideLine = FALSE;
	bShowDataLine = TRUE;

	bAxisYSensitive = TRUE;
	dblAxisYMin = 0;
	dblAxisYMax = 0;

	nMarginTop = 30;
	nMarginLeft = 80;
	nMarginBottom = 40;
	nMarginRight = 20;

	szAxisXNM = "시간(1초)";
	szAxisYNM = "거짓말정도";
}

MLineChartConfig::~MLineChartConfig(void)
{
}
