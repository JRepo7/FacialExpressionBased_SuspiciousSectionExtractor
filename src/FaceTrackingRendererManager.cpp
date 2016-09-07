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
	happyCnt = sadCnt = surpriseCnt = fearCnt = angryCnt = disgustCnt = neutralCnt= 0;
	HAPPY = SAD = SURPRISE = FEAR = ANGRY = DISGUST= NEUTRAL= FALSE;
	EXP_HAPPY= EXP_SAD= EXP_SURPRISE= EXP_FEAR= EXP_ANGRY= EXP_DISGUST= EXP_NEUTRAL=FALSE;
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
	//9
	pOuterBrowRaiserRight = pOuterBrowRaiserLeft = pEyeOpenRight = pEyeOpenLeft = 0;
	plipCornerRightDown = plipCornerLeftDown = 0;
	pUpperLipRaiser = 0;
	//add 
	pBrowLowerRight = pBrowLowerLeft = 0;
	//13
	outerBrowRaiserLeft_LM = outerBrowDepressorLeft_LM = outerBrowRaiserRight_LM = outerBrowDepressorRight_LM = 0;
	upperLipRaiser_LM = 0;
	eyeOpenRight_LM =eyeOpenLeft_LM = 0;
	BrowLowerRight_LM =BrowLowerLeft_LM = 0;
	lipCornerRightUp_LM = lipCornerRightDown_LM = lipCornerLeftUp_LM = lipCornerLeftDown_LM= 0;
	mouthOpen_LM = 0;

	happyCnt = sadCnt = surpriseCnt = fearCnt = angryCnt = disgustCnt = neutralCnt= 0;
}

void FaceTrackingRendererManager::PrepValue()
{
	adj_frameCount++;

	pOuterBrowRaiserRight += (FacialPoint[14].y - FacialPoint[3].y);
	pOuterBrowRaiserLeft += (FacialPoint[22].y - FacialPoint[8].y);

	pEyeOpenRight += (FacialPoint[16].y - FacialPoint[12].y);
	pEyeOpenLeft += (FacialPoint[24].y - FacialPoint[20].y);

	plipCornerRightDown += (FacialPoint[33].y - FacialPoint[30].y);
	plipCornerLeftDown += (FacialPoint[39].y - FacialPoint[32].y);

	pUpperLipRaiser += (FacialPoint[36].y - FacialPoint[26].y);

	pBrowLowerRight += (FacialPoint[12].y - FacialPoint[71].y);
	pBrowLowerLeft += (FacialPoint[20].y - FacialPoint[74].y);

}

void FaceTrackingRendererManager::SetThresValue()
{
	tOuterBrowRaiserRight = (double)pOuterBrowRaiserRight/adj_frameCount;
	tOuterBrowRaiserLeft = (double)pOuterBrowRaiserLeft/adj_frameCount;
	tEyeOpenRight = (double)pEyeOpenRight/adj_frameCount;
	tEyeOpenLeft = (double)pEyeOpenLeft/adj_frameCount;
	tlipCornerRightDown = (double)plipCornerRightDown/adj_frameCount;
	tlipCornerLeftDown = (double)plipCornerLeftDown/adj_frameCount;
	tUpperLipRaiser = (double)pUpperLipRaiser/adj_frameCount;
	tBrowLowerRight = (double)pBrowLowerRight /adj_frameCount;
	tBrowLowerLeft = (double)pBrowLowerLeft /adj_frameCount;
	tMouthOpen = 45;//maxVal=45
}

void FaceTrackingRendererManager::CvtLandmarkToIntensity()
{
	double ratio;

	ratio = (FacialPoint[14].y - FacialPoint[3].y) / tOuterBrowRaiserRight;
	if (ratio > 1)
	{
		outerBrowRaiserRight_LM = (ratio * 100) - 100;
		outerBrowDepressorRight_LM = 0;//�ݴ� ������ ������ outerBrowDepressorRight_LM�� ���� ���� ���°�찡 �����Ƿ� 0���� set����
	}
	else
	{
		outerBrowDepressorRight_LM = 100 - (ratio * 100);
		outerBrowRaiserRight_LM = 0;
	}
	
	ratio = ((FacialPoint[22].y - FacialPoint[8].y) / tOuterBrowRaiserLeft);
	if (ratio > 1)
	{
		outerBrowRaiserLeft_LM = (ratio * 100) - 100;
		outerBrowDepressorLeft_LM = 0;
	}
	else
	{
		outerBrowDepressorLeft_LM = 100 - (ratio * 100);
		outerBrowRaiserLeft_LM = 0;
	}

	//fp36(���Լ�)�� ��ġ�� �ö󰡸� ������ fp31(����)�� fp36(���Լ�) ���� �������� �پ�� �� if(ratio < 1) �� ����
	//**�������� ��Ÿ���µ� ���� �ʿ��Ѱ�
	ratio = ((FacialPoint[36].y - FacialPoint[26].y) / tUpperLipRaiser);
	if (ratio < 1)
	{
		upperLipRaiser_LM = 100 - (ratio * 100);

	}
	else
	{
		upperLipRaiser_LM = 0;
	}

	ratio = ((FacialPoint[33].y - FacialPoint[30].y) / tlipCornerRightDown);
	if (ratio > 1)
	{
		lipCornerRightDown_LM = (ratio * 100)-100;
		lipCornerRightUp_LM = 0;
	}
	else
	{
		lipCornerRightUp_LM = 100 - (ratio * 100);
		lipCornerRightDown_LM = 0;
	}

	ratio = ((FacialPoint[39].y - FacialPoint[32].y) / tlipCornerLeftDown);
	if (ratio > 1)
	{
		lipCornerLeftDown_LM = (ratio * 100)-100;
		lipCornerLeftUp_LM = 0;
	}
	else
	{
		lipCornerLeftUp_LM = 100 - (ratio * 100);
		lipCornerLeftDown_LM = 0;
	}

	ratio = ((FacialPoint[24].y - FacialPoint[20].y) / tEyeOpenLeft);
	if (ratio >1)
	{
		eyeOpenLeft_LM = (ratio * 100)-100;
	}
	else
	{
		eyeOpenLeft_LM = 0;
	}

	ratio = ((FacialPoint[16].y - FacialPoint[12].y) / tEyeOpenRight);
	if (ratio > 1)
	{
		eyeOpenRight_LM = (ratio * 100)-100;
	}
	else
	{
		eyeOpenRight_LM = 0;
	}

	ratio = ((FacialPoint[12].y - FacialPoint[70].y)/ tBrowLowerRight);

	if (ratio > 1)
	{
		BrowLowerRight_LM = 0;
	}
	else
	{
		BrowLowerRight_LM = 100-(ratio * 100);
	}

	ratio = ((FacialPoint[20].y - FacialPoint[74].y)/ tBrowLowerLeft);
	if (ratio > 1)
	{
		BrowLowerLeft_LM = 0;
	}
	else
	{
		BrowLowerLeft_LM = 100-(ratio * 100);
	}

	ratio = ((FacialPoint[51].y - FacialPoint[47].y) /tMouthOpen );
	if (ratio > 1)
	{
		mouthOpen_LM = 100;
	}
	else
	{
		if (ratio < 0) 
		{
			mouthOpen_LM = 0;
		}
		else
		{
			mouthOpen_LM = ratio * 100;
		}
	}
}

void FaceTrackingRendererManager::DetermineExpression()
{
	if (
		(outerBrowRaiserLeft_LM + outerBrowRaiserRight_LM > 1) &&
		(Intensity[Smile] > 10 || (lipCornerLeftUp_LM > 5 && lipCornerRightUp_LM > 5)) && 
		(eyeOpenRight_LM < 10 && eyeOpenLeft_LM < 10)
		)
	{
		HAPPY = TRUE;
		happyCnt++;

		DisplayExpressionUsingEmoji();
	}
	//*/

	if (( (outerBrowDepressorLeft_LM + outerBrowDepressorRight_LM >1) ||(Intensity[BrowLoweredLeft]>10 && Intensity[BrowLoweredRight] > 10) )&& 
		(lipCornerRightDown_LM + lipCornerLeftDown_LM > 3) && mouthOpen_LM<25)
	{
		SAD = TRUE;
		sadCnt++;

		DisplayExpressionUsingEmoji();
	}
	//*/
	
	if (outerBrowRaiserLeft_LM > 1 && outerBrowRaiserRight_LM > 1 && eyeOpenRight_LM > 10 && eyeOpenLeft_LM > 10 &&
		mouthOpen_LM > 20)
	{
		SURPRISE = TRUE;
		surpriseCnt++;

		DisplayExpressionUsingEmoji();
	}
	//*/

	if ((BrowLowerRight_LM + BrowLowerLeft_LM > 10) &&
		(mouthOpen_LM>25 &&(lipCornerRightDown_LM + lipCornerLeftDown_LM>5))
		)
	{
		DISGUST = TRUE;
		disgustCnt++;

		DisplayExpressionUsingEmoji();


	}
	else if (
		(lipCornerRightDown_LM + lipCornerLeftDown_LM < 5) &&
		(BrowLowerRight_LM + BrowLowerLeft_LM>10) &&
		(eyeOpenLeft_LM + eyeOpenRight_LM>5))
	{
		FEAR = TRUE;
		fearCnt++;

		DisplayExpressionUsingEmoji();

	}
	else if (BrowLowerLeft_LM>10 && BrowLowerRight_LM>10 && 
		(lipCornerRightDown_LM + lipCornerLeftDown_LM < 5))
	{
		ANGRY = TRUE;
		angryCnt++;

		DisplayExpressionUsingEmoji();
	}
	//*/

	if (HAPPY == FALSE && SAD == FALSE &&
		SURPRISE == FALSE && FEAR == FALSE &&
		ANGRY == FALSE && DISGUST == FALSE)
	{
		NEUTRAL = TRUE;
		neutralCnt++;

		DisplayExpressionUsingEmoji();
	}
	//*/

	HAPPY = SAD = SURPRISE = FEAR = ANGRY = DISGUST = NEUTRAL = NEUTRAL = FALSE;
}

void FaceTrackingRendererManager::CaptuerExpression() {

	HWND text = GetDlgItem(m_window, IDC_CAP_EXP);
	HWND text1 = GetDlgItem(m_window, IDC_EXP_CNT1);
	HWND text2 = GetDlgItem(m_window, IDC_EXP_CNT2);
	HWND text3 = GetDlgItem(m_window, IDC_EXP_CNT3);
	HWND text4 = GetDlgItem(m_window, IDC_EXP_CNT4);
	HWND text5 = GetDlgItem(m_window, IDC_EXP_CNT5);
	HWND text6 = GetDlgItem(m_window, IDC_EXP_CNT6);
	HWND text7 = GetDlgItem(m_window, IDC_EXP_CNT7);

	CString str;

	int i;
	i=max(happyCnt, 
		max(sadCnt, 
			max(surpriseCnt, 
				max(fearCnt, 
					max(angryCnt, 
						max(disgustCnt,neutralCnt))))));
	
	if (i == neutralCnt)
	{
		EXP_NEUTRAL = TRUE;
		EXP_SAD = EXP_HAPPY = EXP_FEAR = EXP_ANGRY = EXP_SURPRISE = EXP_DISGUST = FALSE;
		str.Format(_T("N E U T R A L"));
		SetWindowTextW(text, str);
	}
	else if (i==happyCnt) 
	{
		EXP_HAPPY = TRUE;
		EXP_SAD = EXP_SURPRISE = EXP_FEAR = EXP_ANGRY = EXP_DISGUST = EXP_NEUTRAL = FALSE;
		str.Format(_T("H A P P Y"));
		SetWindowTextW(text, str);
	}
	else if (i == sadCnt)
	{
		EXP_SAD = TRUE;
		EXP_HAPPY = EXP_SURPRISE = EXP_FEAR = EXP_ANGRY = EXP_DISGUST = EXP_NEUTRAL = FALSE;
		str.Format(_T("S  A  D"));
		SetWindowTextW(text, str);
	}
	else if (i == surpriseCnt)
	{
		EXP_SURPRISE = TRUE;
		EXP_SAD = EXP_HAPPY = EXP_FEAR = EXP_ANGRY = EXP_DISGUST = EXP_NEUTRAL=FALSE;
		str.Format(_T("S U R P R I S E"));
		SetWindowTextW(text, str);
	}
	else if (i == fearCnt)
	{
		EXP_FEAR = TRUE;
		EXP_SAD = EXP_HAPPY = EXP_SURPRISE = EXP_ANGRY = EXP_DISGUST = EXP_NEUTRAL=FALSE;
		str.Format(_T("F  E  A  R"));
		SetWindowTextW(text, str);
	}
	else if (i == angryCnt)
	{
		EXP_ANGRY = TRUE;
		EXP_SAD = EXP_HAPPY = EXP_FEAR = EXP_SURPRISE = EXP_DISGUST = EXP_NEUTRAL=FALSE;
		str.Format(_T("A N G R Y"));
		SetWindowTextW(text, str);
	}
	else if (i == disgustCnt)
	{
		EXP_DISGUST = TRUE;
		EXP_SAD = EXP_HAPPY = EXP_FEAR = EXP_ANGRY = EXP_SURPRISE = EXP_NEUTRAL=FALSE;
		str.Format(_T("D I S G U S T"));
		SetWindowTextW(text, str);
	}

	str.Format(_T("happyCnt:    %d"), happyCnt);
	SetWindowTextW(text1, str);
	str.Format(_T("sadCnt:      %d"), sadCnt);
	SetWindowTextW(text2, str);
	str.Format(_T("surpriseCnt: %d"), surpriseCnt);
	SetWindowTextW(text3, str);
	str.Format(_T("fearCnt:     %d"), fearCnt);
	SetWindowTextW(text4, str);
	str.Format(_T("angryCnt:    %d"), angryCnt);
	SetWindowTextW(text5, str);
	str.Format(_T("disgustCnt:  %d"), disgustCnt);
	SetWindowTextW(text6, str);
	str.Format(_T("neutralCnt:  %d"), neutralCnt);
	SetWindowTextW(text7, str);

	happyCnt= sadCnt= surpriseCnt= fearCnt= angryCnt= disgustCnt= neutralCnt=0;
}

void FaceTrackingRendererManager::DisplayExpressionUsingEmoji()
{
	HWND text_emo = GetDlgItem(m_window, IDC_TEXT_EMO);
	HWND emo = GetDlgItem(m_window, IDC_EMO);
	CString str;
	HBITMAP hBmp;
	if (EXP_HAPPY)
	{
		if (emo != NULL)
		{
			DeleteObject(emo);
		}
		hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_HAPPY), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		str.Format(_TEXT("EXPRESSION: smile"));
		SetWindowTextW(text_emo, str);
		SendDlgItemMessage(m_window, IDC_EMO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
	}

	if (EXP_SAD)
	{
		if (emo != NULL)
		{
			DeleteObject(emo);
		}

		hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_SAD), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		str.Format(_T("EXPRESSION: sad"));
		SetWindowTextW(text_emo, str);
		SendDlgItemMessage(m_window, IDC_EMO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
	}

	if (EXP_SURPRISE)
	{
		if (emo != NULL)
		{
			DeleteObject(emo);
		}

		hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_SURPRISE), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		str.Format(_T("EXPRESSION: surprise"));
		SetWindowTextW(text_emo, str);
		SendDlgItemMessage(m_window, IDC_EMO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);

	}

	if (EXP_FEAR)
	{
		if (emo != NULL)
		{
			DeleteObject(emo);
		}

		hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_FEAR), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		str.Format(_T("EXPRESSION: fear"));
		SetWindowTextW(text_emo, str);
		SendDlgItemMessage(m_window, IDC_EMO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
	}

	if (EXP_ANGRY)
	{
		if (emo)
		{
			DeleteObject(emo);
		}
		hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_ANGRY), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		str.Format(_T("EXPRESSION: angry"));
		SetWindowTextW(text_emo, str);
		SendDlgItemMessage(m_window, IDC_EMO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);

	}
	if (EXP_DISGUST)
	{
		if (emo != NULL)
		{
			DeleteObject(emo);
		}
		hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_DISGUST), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		str.Format(_T("EXPRESSION: disgust"));
		SetWindowTextW(text_emo, str);
		SendDlgItemMessage(m_window, IDC_EMO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
	}
	if (EXP_NEUTRAL)
	{
		if (emo != NULL)
		{
			DeleteObject(emo);
		}
		hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_NEUTRAL), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		str.Format(_T("EXPRESSION: neutral"));
		SetWindowTextW(text_emo, str);
		SendDlgItemMessage(m_window, IDC_EMO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
	}
}