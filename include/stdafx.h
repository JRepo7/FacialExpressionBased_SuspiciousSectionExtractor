// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.

#pragma once

#include <afx.h>
#include <afxwin.h>         // MFC �ٽ� �� ǥ�� ���� ����Դϴ�.

#include <windows.h>


extern int srcx;
extern int srcy;

//������ �ܼ� �÷��� 7��
extern volatile bool SMILE_FLAG;
extern volatile bool GAZE_FLAG;
extern volatile bool BLINK_FLAG;
extern volatile bool HEADMOTION_FLAG;
extern volatile bool PULSE_FLAG;
extern volatile bool MICROEXP_FLAG;
extern volatile bool EXPRESSION_FLAG;
extern volatile bool AUTOADJUST;
