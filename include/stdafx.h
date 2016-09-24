// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#include <afx.h>
#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.

#include "pxcfacedata.h"
#include <windows.h>

struct fData
{
	int time;
	bool exp;
	bool micro;
	bool smile;
	bool gaze;
	bool blink;
	bool head;
	bool pulse;
};

extern int srcx;
extern int srcy;

extern fData *DataSet;
extern FILE *fp;

extern pxcI32 gIndex;
extern int gFrame;

//거짓말 단서 플래그 7개
extern volatile bool SMILE_FLAG;
extern volatile bool GAZE_FLAG;
extern volatile bool BLINK_FLAG;
extern volatile bool HEADMOTION_FLAG;
extern volatile bool PULSE_FLAG;
extern volatile bool MICROEXP_FLAG;
extern volatile bool EXPRESSION_FLAG;
extern volatile bool AUTOADJUST;
extern volatile bool STOPRENDERING;

// Prep Val GetPlayBack 
extern volatile bool INIT_FLAG;