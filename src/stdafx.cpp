// stdafx.cpp : ǥ�� ���� ���ϸ� ��� �ִ� �ҽ� �����Դϴ�.
// ChartTest02.pch�� �̸� �����ϵ� ����� �˴ϴ�.
// stdafx.obj���� �̸� �����ϵ� ���� ������ ���Ե˴ϴ�.

#include "stdafx.h"

int srcx=0;
int srcy=0;

extern FILE *fp = fopen("data.txt","a");

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