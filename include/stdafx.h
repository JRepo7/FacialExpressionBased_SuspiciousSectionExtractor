// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.

#pragma once

#include <afx.h>
#include <afxwin.h>         // MFC �ٽ� �� ǥ�� ���� ����Դϴ�.

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

//������ �ܼ� �÷��� 7��
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