#include "FaceTrackingRendererManager.h"
#include <string.h>
#include <cstring>
#include <atlstr.h>

extern HANDLE ghMutex;

FaceTrackingRendererManager::FaceTrackingRendererManager(FaceTrackingRenderer2D* renderer2D, FaceTrackingRenderer3D* renderer3D, HWND window) :
	m_window(window), m_renderer2D(renderer2D), m_renderer3D(renderer3D), m_currentRenderer(NULL)
{
	m_rendererSignal = CreateEvent(NULL, FALSE, FALSE, NULL);
	adj_frameCount = 0;
	max1 = 0;
	max2 = 0;
	max3 = 0;
	max4 = 0;
	max5 = 0;
	max6 = 0;
	max7 = 0;
	max8 = 0;
	max9 = 0;
	happy_count=0;
	sad_count=0;
	surprise_count=0;
	fear_count=0;
	angry_count=0;
	disgust_count=0;
	outerBrowRaiserRight_I_sum =0;
	outerBrowRaiserLeft_I_sum =0;
	smile_sum =0;
	lipCornerLeftUp_I_sum =0;
	lipCornerRightUp_I_sum =0;
}

FaceTrackingRendererManager::~FaceTrackingRendererManager(void)
{
	CloseHandle(m_rendererSignal);

	if(m_renderer2D != NULL)
		delete m_renderer2D;

	if(m_renderer3D != NULL)
		delete m_renderer3D;
}

void FaceTrackingRendererManager::SetRendererType(FaceTrackingRenderer::RendererType type)
{
	DWORD dwWaitResult;
	dwWaitResult = WaitForSingleObject(ghMutex,	INFINITE);
	if(dwWaitResult == WAIT_OBJECT_0)
	{
		if(type == FaceTrackingRenderer::R2D)
		{
			m_currentRenderer = m_renderer2D;
		}
		else
		{
			m_currentRenderer = m_renderer3D;
		}
		if(!ReleaseMutex(ghMutex))
		{
			throw std::exception("Failed to release mutex");
			return;
		}
	}
}

void FaceTrackingRendererManager::Render()
{
	WaitForSingleObject(m_rendererSignal, INFINITE);

	m_currentRenderer->Render();

	m_callback();
}

void FaceTrackingRendererManager::SetSenseManager(PXCSenseManager* senseManager)
{
	m_renderer2D->SetSenseManager(senseManager);
	m_renderer3D->SetSenseManager(senseManager);
}

void FaceTrackingRendererManager::SetNumberOfLandmarks(int numLandmarks)
{
	m_renderer2D->SetNumberOfLandmarks(numLandmarks);
	m_renderer3D->SetNumberOfLandmarks(numLandmarks);
}

void FaceTrackingRendererManager::SetCallback(OnFinishedRenderingCallback callback)
{
	m_callback = callback;
}

void FaceTrackingRendererManager::DrawBitmap(PXCCapture::Sample* sample, bool ir)
{
	m_currentRenderer->DrawBitmap(sample, ir);
}

void FaceTrackingRendererManager::SetOutput(PXCFaceData* output)
{
	m_renderer2D->SetOutput(output);
	m_renderer3D->SetOutput(output);
}

void FaceTrackingRendererManager::SignalRenderer()
{
	SetEvent(m_rendererSignal);
}

void FaceTrackingRendererManager::SignalProcessor()
{
	SetEvent(GetRenderingFinishedSignal());
}

HANDLE& FaceTrackingRendererManager::GetRenderingFinishedSignal()
{
	static HANDLE renderingFinishedSignal = CreateEvent(NULL, FALSE, TRUE, NULL);
	return renderingFinishedSignal;
}

void FaceTrackingRendererManager::SetActivateEyeCenterCalculations(bool bValue) 
{
	m_renderer2D->SetActivateEyeCenterCalculations(bValue);
}

void FaceTrackingRendererManager::DrawDistances()
{
	m_renderer2D->CalcDistances();
}

void FaceTrackingRendererManager::Reset()
{
	m_renderer2D->Reset();
}

void FaceTrackingRendererManager::GetExpIntensity()
{

	for (int i = 0; i < sizeof(Intensity) / sizeof(Intensity[0]); i++)
	{
		Intensity[i] = m_renderer2D->Intensity[i];
	}
}
void FaceTrackingRendererManager::GetLandmarkPoint()
{

	for (int i = 0; i < sizeof(FacialPoint) / sizeof(FacialPoint[0]); i++)
	{
		FacialPoint[i] = m_renderer2D->FacialPoint[i];
	}

}
void FaceTrackingRendererManager::InitValue()
{
	adj_frameCount = 0;

	prep_OuterBrowRaiserRight=0;
	prep_OuterBrowRaiserLeft=0;
	prep_EyeOpenRight=0;
	prep_EyeOpenLeft=0;
	prep_lipCornerRightDown=0;
	prep_lipCornerLeftDown=0;
	prep_UpperLipRaiser=0;

	outerBrowRaiserLeft_I = 0;
	outerBrowDepressorLeft_I = 0;
	outerBrowRaiserRight_I = 0;
	outerBrowDepressorRight_I = 0;

	upperLipRaiser_I = 0;

	lipCornerRightUp_I = 0;
	lipCornerLeftUp_I = 0;

	lipCornerRightDown_I = 0;
	lipCornerLeftDown_I= 0;

	eyeOpenRight_I = 0;
	eyeOpenLeft_I = 0;

	max1 = 0;
	max2 = 0;
	max3 = 0;
	max4 = 0;
	max5 = 0;
	max6 = 0;
	max7 = 0;
	max8 = 0;
	max9 = 0;

}
void FaceTrackingRendererManager::PrepValue()
{

	adj_frameCount++;

	prep_OuterBrowRaiserRight += (FacialPoint[14].y - FacialPoint[3].y);
	prep_OuterBrowRaiserLeft += (FacialPoint[22].y - FacialPoint[8].y);

	prep_EyeOpenRight += (FacialPoint[16].y - FacialPoint[12].y);
	prep_EyeOpenLeft += (FacialPoint[24].y - FacialPoint[20].y);

	prep_lipCornerRightDown += (FacialPoint[33].y - FacialPoint[30].y);
	prep_lipCornerLeftDown += (FacialPoint[39].y - FacialPoint[32].y);

	prep_UpperLipRaiser += (FacialPoint[36].y - FacialPoint[26].y);


}

void FaceTrackingRendererManager::SetThresValue()
{

	thres_OuterBrowRaiserRight = (double)prep_OuterBrowRaiserRight/adj_frameCount;
	thres_OuterBrowRaiserLeft = (double)prep_OuterBrowRaiserLeft/adj_frameCount;

	thres_EyeOpenRight = (double)prep_EyeOpenRight/adj_frameCount;
	thres_EyeOpenLeft = (double)prep_EyeOpenLeft/adj_frameCount;

	thres_lipCornerRightDown = (double)prep_lipCornerRightDown/adj_frameCount;
	thres_lipCornerLeftDown = (double)prep_lipCornerLeftDown/adj_frameCount;
	thres_UpperLipRaiser = (double)prep_UpperLipRaiser/adj_frameCount;

}

void FaceTrackingRendererManager::CvtLandmarkToIntensity()
{
	HWND text_I1 = GetDlgItem(m_window, IDC_TEXT_I1);
	HWND text_I2 = GetDlgItem(m_window, IDC_TEXT_I2);
	HWND text_I3 = GetDlgItem(m_window, IDC_TEXT_I3);
	HWND text_I4 = GetDlgItem(m_window, IDC_TEXT_I4);
	HWND text_I5 = GetDlgItem(m_window, IDC_TEXT_I5);
	HWND text_I6 = GetDlgItem(m_window, IDC_TEXT_I6);
	HWND text_I7 = GetDlgItem(m_window, IDC_TEXT_I7);
	HWND text_I8 = GetDlgItem(m_window, IDC_TEXT_I8);
	HWND text_I9 = GetDlgItem(m_window, IDC_TEXT_I9);

	HWND text_Exp1 = GetDlgItem(m_window, IDC_TEXT_EXP1);
	HWND text_Exp2 = GetDlgItem(m_window, IDC_TEXT_EXP2);
	HWND text_Exp3 = GetDlgItem(m_window, IDC_TEXT_EXP3);
	HWND text_Exp4 = GetDlgItem(m_window, IDC_TEXT_EXP4);
	HWND text_Exp5 = GetDlgItem(m_window, IDC_TEXT_EXP5);
	HWND text_Exp6 = GetDlgItem(m_window, IDC_TEXT_EXP6);
	HWND text_Exp7 = GetDlgItem(m_window, IDC_TEXT_EXP7);
	HWND text_Exp8 = GetDlgItem(m_window, IDC_TEXT_EXP8);
	HWND text_Exp9 = GetDlgItem(m_window, IDC_TEXT_EXP9);
	HWND text_Exp10 = GetDlgItem(m_window, IDC_TEXT_EXP10);
	HWND text_Exp11 = GetDlgItem(m_window, IDC_TEXT_EXP11);
	HWND text_Exp12 = GetDlgItem(m_window, IDC_TEXT_EXP12);
	HWND text_Exp13 = GetDlgItem(m_window, IDC_TEXT_EXP13);

	CString str;
	double ratio;

	ratio = ((FacialPoint[14].y - FacialPoint[3].y) / thres_OuterBrowRaiserRight);
	if (ratio > 1)
	{
		outerBrowRaiserRight_I = (ratio * 100) - 100;
		outerBrowDepressorRight_I = 0;//�ݴ� ������ ������ outerBrowDepressorRight_I�� ���� ���� ���°�찡 �����Ƿ� 0���� set����

		if (max1 < outerBrowRaiserRight_I)
		{
			max1 = outerBrowRaiserRight_I;
		}

	}
	else
	{
		outerBrowDepressorRight_I = 100 - (ratio * 100);
		outerBrowRaiserRight_I = 0;

		if (max2 < outerBrowDepressorRight_I)
		{
			max2 = outerBrowDepressorRight_I;
		}
	}
	
	ratio = ((FacialPoint[22].y - FacialPoint[8].y) / thres_OuterBrowRaiserLeft);
	if (ratio > 1)
	{
		outerBrowRaiserLeft_I = (ratio * 100) - 100;
		outerBrowDepressorLeft_I = 0;

		if (max3 < outerBrowRaiserLeft_I)
		{
			max3 = outerBrowRaiserLeft_I;
		}
	}
	else
	{
		outerBrowDepressorLeft_I = 100 - (ratio * 100);
		outerBrowRaiserLeft_I = 0;

		if (max4 < outerBrowDepressorLeft_I)
		{
			max4 = outerBrowDepressorLeft_I;
		}
	}

	//fp36(���Լ�)�� ��ġ�� �ö󰡸� ������ fp31(����)�� fp36(���Լ�) ���� �������� �پ�� �� if(ratio < 1) �� ����
	//**�������� ��Ÿ���µ� ���� �ʿ��Ѱ�
	ratio = ((FacialPoint[36].y - FacialPoint[26].y) / thres_UpperLipRaiser);
	if (ratio < 1)
	{
		upperLipRaiser_I = 100 - (ratio * 100);

		if (max5 < upperLipRaiser_I)
		{
			max5 = upperLipRaiser_I;
		}
	}
	else
	{
		upperLipRaiser_I = 0;
	}

	ratio = ((FacialPoint[33].y - FacialPoint[30].y) / thres_lipCornerRightDown);
	if (ratio > 1)
	{
		lipCornerRightDown_I = (ratio * 100)-100;
		lipCornerRightUp_I = 0;
		if (max6 < lipCornerRightDown_I)
		{
			max6 = lipCornerRightDown_I;
		}
	}
	else
	{
		lipCornerRightUp_I = 100 - (ratio * 100);
		lipCornerRightDown_I = 0;
	}

	ratio = ((FacialPoint[39].y - FacialPoint[32].y) / thres_lipCornerLeftDown);
	if (ratio > 1)
	{
		lipCornerLeftDown_I = (ratio * 100)-100;
		lipCornerLeftUp_I = 0;
		if (max7 < lipCornerLeftDown_I)
		{
			max7 = lipCornerLeftDown_I;
		}
	}
	else
	{
		lipCornerLeftUp_I = 100 - (ratio * 100);
		lipCornerLeftDown_I = 0;
	}

	ratio = ((FacialPoint[24].y - FacialPoint[20].y) / thres_EyeOpenLeft);
	if (ratio >1)
	{
		eyeOpenLeft_I = (ratio * 100)-100;

		if (max8 < eyeOpenLeft_I)
		{
			max8 = eyeOpenLeft_I;
		}
	}
	else
	{
		eyeOpenLeft_I = 0;
	}

	ratio = ((FacialPoint[16].y - FacialPoint[12].y) / thres_EyeOpenRight);
	if (ratio > 1)
	{
		eyeOpenRight_I = (ratio * 100)-100;

		if (max9 < eyeOpenRight_I)
		{
			max9 = eyeOpenRight_I;
		}
	}
	else
	{
		eyeOpenRight_I = 0;
	}
/*
	str.Format(_T("outerBrowRaiserLeft_I: %f"), outerBrowRaiserLeft_I);
	SetWindowTextW(text_I1, str);
	str.Format(_T("outerBrowDepressorLeft_I: %f"), outerBrowDepressorLeft_I);
	SetWindowTextW(text_I2, str);
	
	str.Format(_T("outerBrowRaiserRight_I: %f"), outerBrowRaiserRight_I);
	SetWindowTextW(text_I3, str);
	str.Format(_T("outerBrowDepressorRight_I: %f"), outerBrowDepressorRight_I);
	SetWindowTextW(text_I4, str);

	str.Format(_T("upperLipRaiser_I: %f"), upperLipRaiser_I);
	SetWindowTextW(text_I5, str);

	str.Format(_T("lipCornerRightDown_I: %f"), lipCornerRightDown_I);
	SetWindowTextW(text_I6, str);
	str.Format(_T("lipCornerLeftDown_I: %f"), lipCornerLeftDown_I);
	SetWindowTextW(text_I7, str);

	str.Format(_T("eyeOpenRight_I: %f"), eyeOpenRight_I);
	SetWindowTextW(text_I8, str);
	str.Format(_T("eyeOpenLeft_I: %f"), eyeOpenLeft_I);
	SetWindowTextW(text_I9, str);
*/


	str.Format(_T("outerBrowRaiserLeft_I max1:\t\t  %f"), outerBrowRaiserLeft_I);
	SetWindowTextW(text_I1, str);
	str.Format(_T("outerBrowRaiserRight_I max2:\t  %f"), outerBrowRaiserRight_I);
	SetWindowTextW(text_I2, str);

	str.Format(_T("outerBrowDepressorLeft_I max3:\t  %f"), outerBrowDepressorLeft_I);
	SetWindowTextW(text_I3, str);
	str.Format(_T("outerBrowDepressorRight_I max4:\t  %f"), outerBrowDepressorRight_I);
	SetWindowTextW(text_I4, str);

	str.Format(_T("upperLipRaiser_I max5:\t\t  %f"), upperLipRaiser_I);
	SetWindowTextW(text_I5, str);

	str.Format(_T("lipCornerRightUp_I max6:\t\t  %f"), lipCornerRightUp_I);
	SetWindowTextW(text_I6, str);
	str.Format(_T("lipCornerLeftUp_I max7:\t\t  %f"), lipCornerLeftUp_I);
	SetWindowTextW(text_I7, str);

	str.Format(_T("eyeOpenRight_I max8:\t\t   %f"), eyeOpenRight_I);
	SetWindowTextW(text_I8, str);
	str.Format(_T("eyeOpenLeft_I max9:\t\t   %f"), eyeOpenLeft_I);
	SetWindowTextW(text_I9, str);


	str.Format(_T("BrowRaisedLeft: \t\t %d"),  Intensity[0]);
	SetWindowTextW(text_Exp1, str);

	str.Format(_T("BrowRaisedRight: \t\t %d"),  Intensity[1]);
	SetWindowTextW(text_Exp2, str);

	str.Format(_T("BrowLoweredLeft: \t\t %d"),  Intensity[2]);
	SetWindowTextW(text_Exp3, str);

	str.Format(_T("BrowLoweredRight:\t\t %d"),  Intensity[3]);
	SetWindowTextW(text_Exp4, str);

	str.Format(_T("Smile: \t\t\t %d"),  Intensity[4]);
	SetWindowTextW(text_Exp5, str);

	str.Format(_T("Kiss: \t\t\t %d"),  Intensity[5]);
	SetWindowTextW(text_Exp6, str);

	str.Format(_T("MouthOpen: \t\t %d"),  Intensity[6]);
	SetWindowTextW(text_Exp7, str);

	str.Format(_T("ClosedEyeLeft: \t\t %d"),  Intensity[7]);
	SetWindowTextW(text_Exp8, str);

	str.Format(_T("ClosedEyeRight: \t\t %d"),  Intensity[8]);
	SetWindowTextW(text_Exp9, str);

	str.Format(_T("EyesTurnLeft: \t\t %d"),  Intensity[9]);
	SetWindowTextW(text_Exp10, str);

	str.Format(_T("EyesTurnRight: \t\t %d"),  Intensity[10]);
	SetWindowTextW(text_Exp11, str);

	str.Format(_T("EyesUp: \t\t\t %d"),  Intensity[11]);
	SetWindowTextW(text_Exp12, str);

	str.Format(_T("EyesDown: \t\t %d"),  Intensity[12]);
	SetWindowTextW(text_Exp13, str);

}

void FaceTrackingRendererManager::DertermineExpression()
{
	HWND text_emo = GetDlgItem(m_window, IDC_TEXT_EMO);
	HWND emo = GetDlgItem(m_window, IDC_EMO);
	CString str;
	HBITMAP hBmp;
	//happy
	if (outerBrowRaiserRight_I >10 && outerBrowRaiserLeft_I > 10 &&  
		(Intensity[Smile] > 10 || (lipCornerLeftUp_I>10 && lipCornerRightUp_I>10 ))) 
	{
		happy_count++;
		outerBrowRaiserRight_I_sum += outerBrowRaiserRight_I;
		outerBrowRaiserLeft_I_sum += outerBrowRaiserLeft_I;
		smile_sum += Intensity[Smile];
		lipCornerLeftUp_I_sum += lipCornerLeftUp_I;
		lipCornerRightUp_I_sum += lipCornerRightUp_I;

		if (emo != NULL) { 
			DeleteObject(emo);
		}
		hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_HAPPY), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		str.Format(_TEXT("EXPRESSION: SMILE"));
		SetWindowTextW(text_emo, str );
		SendDlgItemMessage(m_window, IDC_EMO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
	}

	//sad
	if (Intensity[2] > 10 && Intensity[3] > 10 && lipCornerRightDown_I > 5 && lipCornerLeftDown_I > 5) 
	{
		sad_count++;

		if (emo != NULL) {
			DeleteObject(emo);
		}
		hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_SAD), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		str.Format(_T("EXPRESSION: SAD"));
		SetWindowTextW(text_emo, str);
		SendDlgItemMessage(m_window, IDC_EMO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
	}

	//surprise
	if (Intensity[6] > 10 && Intensity[0] > 10 && Intensity[MouthOpen] > 10)
	{
		if (emo != NULL) {
			DeleteObject(emo);
		}
		hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_SURPRISE), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		str.Format(_T("EXPRESSION: SURPRISE"));
		SetWindowTextW(text_emo, str);
		SendDlgItemMessage(m_window, IDC_EMO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
	}

	//fear
	if (NULL)
	{
	}

	//angry
	if (NULL)
	{
	}
}

void FaceTrackingRendererManager::SetAverage()
{
	double outerBrowRaiserRight_I_avg=0;
	double outerBrowRaiserLeft_I_avg = 0;
	double smile_avg = 0;
	double lipCornerLeftUp_I_avg = 0;
	double lipCornerRightUp_I_avg = 0;

	HWND text_I1 = GetDlgItem(m_window, IDC_TEXT_EXP14);
	HWND text_I2 = GetDlgItem(m_window, IDC_TEXT_EXP15);
	HWND text_I3 = GetDlgItem(m_window, IDC_TEXT_EXP16);
	HWND text_I4 = GetDlgItem(m_window, IDC_TEXT_EXP17);
	HWND text_I5 = GetDlgItem(m_window, IDC_TEXT_EXP18);
	CString str;

	outerBrowRaiserRight_I_avg = outerBrowRaiserRight_I_sum / happy_count;
	outerBrowRaiserLeft_I_avg = outerBrowRaiserLeft_I_sum / happy_count;
	smile_avg = smile_sum / happy_count;
	lipCornerLeftUp_I_avg = lipCornerLeftUp_I_sum / happy_count;
	lipCornerRightUp_I_avg = lipCornerRightUp_I_sum / happy_count;

	str.Format(_T("outerBrowRaiserRight_I_avg: %f"), outerBrowRaiserRight_I_avg);
	SetWindowTextW(text_I1, str);
	str.Format(_T("outerBrowRaiserLeft_I_avg: %f"), outerBrowRaiserLeft_I_avg);
	SetWindowTextW(text_I2, str);
	str.Format(_T("smile_avg: %f"), smile_avg);
	SetWindowTextW(text_I3, str);
	str.Format(_T("lipCornerLeftUp_I_avg: %f"), lipCornerLeftUp_I_avg);
	SetWindowTextW(text_I4, str);
	str.Format(_T("lipCornerRightUp_I_avg: %f"), lipCornerRightUp_I_avg);
	SetWindowTextW(text_I5, str);


	happy_count =0 ;
	sad_count = 0;
	surprise_count = 0;
	fear_count = 0;
	angry_count = 0;
	disgust_count = 0;

	outerBrowRaiserRight_I_sum = 0;
	outerBrowRaiserLeft_I_sum = 0;
	smile_sum = 0;
	lipCornerLeftUp_I_sum = 0;
	lipCornerRightUp_I_sum = 0;
}
void InitCount()
{

}