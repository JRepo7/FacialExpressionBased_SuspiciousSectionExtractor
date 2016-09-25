// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// ChartTest02.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"

int srcx=0;
int srcy=0;
int Framenumber = 0;
extern fData *DataSet = NULL;
extern FILE *fp = NULL;

extern bool flagSet[7] = {0,};

volatile bool SMILE_FLAG = FALSE;
volatile bool GAZE_FLAG = FALSE;
volatile bool BLINK_FLAG = FALSE;
volatile bool HEADMOTION_FLAG = FALSE;
volatile bool PULSE_FLAG = FALSE;
volatile bool MICROEXP_FLAG = FALSE;
volatile bool EXPRESSION_FLAG = FALSE;
volatile bool AUTOADJUST = FALSE;
volatile bool STOPRENDERING = FALSE;
volatile bool INIT_FLAG = TRUE;

pxcI32 gIndex = 0;