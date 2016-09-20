#include "stdafx.h"
#include "FaceTrackingRendererManager.h"
#include <string.h>
#include <cstring>
#include <atlstr.h>

extern HANDLE ghMutex;

FaceTrackingRendererManager::FaceTrackingRendererManager(FaceTrackingRenderer2D* renderer2D, FaceTrackingRenderer3D* renderer3D, HWND window) :
	m_window(window), m_renderer2D(renderer2D), m_renderer3D(renderer3D), m_currentRenderer(NULL)
{
	enum {
		happy,
		sad,
		surprise,
		fear,
		angry,
		disgust,
		neutral
	};

	m_rendererSignal = CreateEvent(NULL, FALSE, FALSE, NULL);
	adj_frameCount = 0;
	happyCnt = sadCnt = surpriseCnt = fearCnt = angryCnt = disgustCnt = neutralCnt= 0;
	HAPPY = SAD = SURPRISE = FEAR = ANGRY = DISGUST= FALSE;
	EXP_EMO[happy]= EXP_EMO[sad] = EXP_EMO[surprise] = EXP_EMO[fear] = EXP_EMO[angry] = EXP_EMO[disgust] = EXP_EMO[neutral] =FALSE;
	//slidingWindow = NULL;
	cursor = cursor_d=cursor_s= cursor_m = 0 ;
	sizeOfWindow = GetFrameSize(60);
	sizeOfWindow_s = GetFrameSize(10);
	sizeOfWindow_R = GetFrameSize(60);
	sizeOfWindow_M = 10;

	candidEmo[happy] = candidEmo[sad] = candidEmo[surprise] = candidEmo[fear] = candidEmo[angry] = candidEmo[disgust] = candidEmo[neutral] = 0;
	//memset(slidingWindow, 0, SizeOfWindow);
	record = rear = front = 0;
	record_Range = rear_Range =  0;

	initFront = initFront_Range = initFront_M = FALSE;
	curr_r = prev_r = next_f = curr_f = FALSE;

	winner = 0;
	angles.yaw = angles.pitch = hr = 0;

	Int1 = 0;
	hrcnt1=hrcnt2=hrcnt3=hrcnt4=hrcnt5=hrcnt6=0;

	sumEyesTurnLeft=sumEyesTurnRight =gazecnt = 0;
	count1 = count2 = count3 = count4 = count5 = count6 = 0;
	pre_hr1, pre_hr2, pre_hr3, pre_hr4 = 0;
	nCnt = hCnt = sCnt = pCnt = fCnt = aCnt = dCnt = 0;
	preEmo = 0;
	F_FLAG = FALSE;
}

FaceTrackingRendererManager::~FaceTrackingRendererManager(void)
{
	CloseHandle(m_rendererSignal);

	if(m_renderer2D != NULL)
		delete m_renderer2D;

	if(m_renderer3D != NULL)
		delete m_renderer3D;

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

	EXP_EMO[0] = EXP_EMO[1] = EXP_EMO[2] = EXP_EMO[3] = EXP_EMO[4] = EXP_EMO[5] = EXP_EMO[6] = FALSE;
	happyCnt = sadCnt = surpriseCnt = fearCnt = angryCnt = disgustCnt = neutralCnt= 0;

	frequencyEmo[0] = frequencyEmo[1] = frequencyEmo[2] = frequencyEmo[3] = frequencyEmo[4] = frequencyEmo[5] = frequencyEmo[6] = 0;

	cursor =cursor_s= cursor_m =rear = 0;
//	sizeOfWindow = sizeOfWindow_d = 0;
	record = winner = 0;

	sumEyesTurnLeft = 0;
	sumEyesTurnRight = 0;
	Int1 = 0;

	hrcnt1 =hrcnt2 =hrcnt3 = hrcnt4 =hrcnt5 = hrcnt6 = 0;

	count1 = count2 = count3 = count4 = count5 = count6 = 0;

	sumEyeyaw = 0;
	sumEyepitch = 0;

	avgEyeyaw = 0;
	avgEyepitch = 0;
	gazecnt = 0;
	pre_hr1, pre_hr2, pre_hr3, pre_hr4 = 0;
}

void FaceTrackingRendererManager::InitStop()
{
	m_renderer2D->systemcnt = 0;
	m_renderer2D->gazemax = 0;
	m_renderer2D->gazemin = 0;
	m_renderer2D->angleh = 0;
	m_renderer2D->avgangleh = 0;
	m_renderer2D->sumangleh = 0;
	m_renderer2D->gazesumcnt = 0;
	m_renderer2D->gazecnt = 0;
	record_Range = 0;
	nCnt = hCnt = sCnt = pCnt = fCnt = aCnt = dCnt = 0;
	preEmo = 0;
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

void FaceTrackingRendererManager::DrawBitmap2(PXCCapture::Sample* sample, bool ir)
{
	m_currentRenderer->DrawBitmap2(sample, ir);
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
void FaceTrackingRendererManager::GetHeadandPulse()
{
	angles.yaw=m_renderer2D->angles.yaw;
	angles.pitch=m_renderer2D->angles.pitch;
	hr = m_renderer2D->hr;
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

	//pBrowLowerRight += (FacialPoint[12].y - FacialPoint[71].y);
	//pBrowLowerLeft += (FacialPoint[20].y - FacialPoint[74].y);

	pBrowLowerRight += (FacialPoint[29].y - FacialPoint[71].y);
	pBrowLowerLeft += (FacialPoint[29].y - FacialPoint[74].y);
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
		outerBrowDepressorRight_LM = 0;//반대 조건의 변수인 outerBrowDepressorRight_LM가 이전 값을 갖는경우가 있으므로 0으로 set해줌
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

	//fp36(윗입술)의 위치가 올라가면 기준점 fp31(인중)과 fp36(윗입술) 간의 높이차는 줄어듬 즉 if(ratio < 1) 가 적절
	//**혐오감을 나타내는데 굳이 필요한가
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

	//ratio = ((FacialPoint[12].y - FacialPoint[70].y)/ tBrowLowerRight);
	ratio = ((FacialPoint[29].y - FacialPoint[70].y) / tBrowLowerRight);

	if (ratio > 1)
	{
		BrowLowerRight_LM = 0;
	}
	else
	{
		BrowLowerRight_LM = 100-(ratio * 100);
	}

	//ratio = ((FacialPoint[20].y - FacialPoint[74].y)/ tBrowLowerLeft);
	ratio = ((FacialPoint[29].y - FacialPoint[74].y) / tBrowLowerLeft);
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
void FaceTrackingRendererManager::CaptureSubtleExpression() 
{
	enum
	{
		happy,
		sad,
		surprise,
		fear,
		angry,
		disgust,
		neutral
	};
	int Win =max(happyCnt,
					max(sadCnt, 
						max(surpriseCnt, 
							max(fearCnt, 
								max(angryCnt, 
									max(disgustCnt,neutralCnt))))));

	ContinueExpression(Win);

	if (initFront_M && frequencyEmo[slidingWindow_M[cursor_m]] > 0)
	{
		frequencyEmo[slidingWindow_M[cursor_m]]--;
	}

	if (Win == neutralCnt && Win != 0)
	{
		EXP_EMO[neutral] = TRUE;

		slidingWindow_M[cursor_m] = neutral;

		DisplayExpressionUsingEmoji(EXP_EMO);

		EXP_EMO[neutral] = FALSE;
	}
	else if (Win == happyCnt && Win != 0)
	{
		EXP_EMO[happy] = TRUE;

		slidingWindow_M[cursor_m] = happy;

		DisplayExpressionUsingEmoji(EXP_EMO);

		EXP_EMO[happy] = FALSE;
	}
	else if (Win == sadCnt && Win != 0)
	{
		EXP_EMO[sad] = TRUE;

		slidingWindow_M[cursor_m] = sad;
		DisplayExpressionUsingEmoji(EXP_EMO);

		EXP_EMO[sad] = FALSE;
	}
	else if (Win == surpriseCnt && Win != 0)
	{
		EXP_EMO[surprise] = TRUE;

		slidingWindow_M[cursor_m] = surprise;

		DisplayExpressionUsingEmoji(EXP_EMO);

		EXP_EMO[surprise] = FALSE;
	}
	else if (Win == fearCnt && Win != 0)
	{
		EXP_EMO[fear] = TRUE;

		slidingWindow_M[cursor_m] = fear;
		DisplayExpressionUsingEmoji(EXP_EMO);

		EXP_EMO[fear] = FALSE;
	}
	else if (Win == angryCnt && Win != 0)
	{
		EXP_EMO[angry] = TRUE;

		slidingWindow_M[cursor_m] = angry;
		DisplayExpressionUsingEmoji(EXP_EMO);

		EXP_EMO[angry] = FALSE;
	}
	else if (Win == disgustCnt && Win != 0)
	{
		EXP_EMO[disgust] = TRUE;

		slidingWindow_M[cursor_m] = disgust;

		DisplayExpressionUsingEmoji(EXP_EMO);

		EXP_EMO[disgust] = FALSE;
	}
	else
	{
		if (Win != 0)
		{
			EXP_EMO[neutral] = TRUE;

			slidingWindow_M[cursor_m] = neutral;

			DisplayExpressionUsingEmoji(EXP_EMO);

			EXP_EMO[neutral] = FALSE;
		}
	}


	if ((hCnt > 9 || sCnt > 9 || nCnt > 9 || pCnt > 9 || fCnt > 9 || aCnt > 9 || dCnt > 9) && (slidingWindow_M[cursor_m] != slidingWindow_M[cursor_f]) && initFront_M)
	{ 
		F_FLAG = FALSE;
	}
	else
	{
		F_FLAG = TRUE;
	}


	if (Win!=0)
	CalcMicroCount(Win);

	frequencyEmo[slidingWindow_M[cursor_m]]++;// range 0~6

	GetFreqBasedOnEmo();	// 8?

	cursor_m++;

	if ((cursor_m % 10) == 0)
	{
		cursor_m = 0;
		initFront_M = TRUE;
	}

	if (cursor_m != 0)
	{
		cursor_f = cursor_m - 1;
	}
	else if (cursor_m == 0 && initFront_M)
	{
		cursor_f = 9;
	}

	happyCnt = sadCnt = surpriseCnt = fearCnt = angryCnt = disgustCnt = neutralCnt = 0;
}

void FaceTrackingRendererManager::GetFreqBasedOnEmo()
{
	enum
	{
		happy,
		sad,
		surprise,
		fear,
		angry,
		disgust,
		neutral
	};
	int Win1 = max(frequencyEmo[happy],
		max(frequencyEmo[sad],
			max(frequencyEmo[surprise],
				max(frequencyEmo[fear],
					max(frequencyEmo[angry],
						max(frequencyEmo[disgust], frequencyEmo[neutral]))))));
	CString str;

	bool T_FLAG = (hCnt != 9 && sCnt != 9 && nCnt != 9 && pCnt != 9 && fCnt != 9 && aCnt != 9 && dCnt != 9);

	if (T_FLAG && (F_FLAG == TRUE))
	{
		if (Win1 == 9 && initFront_M)
		{
			MICROEXP_FLAG = TRUE;
		}
	}

	HWND text = GetDlgItem(m_window, IDC_MICRO);

	if(Win1 == frequency[happy])
		str.Format(_T("HAPPY"));
	else if(Win1 == frequency[sad])
		str.Format(_T("SAD"));
	else if (Win1 == frequency[surprise])
		str.Format(_T("SURPRISE"));
	else if (Win1 == frequency[fear])
		str.Format(_T("FEAR"));
	else if (Win1 == frequency[angry])
		str.Format(_T("ANGRY"));
	else if (Win1 == frequency[disgust])
		str.Format(_T("DISGUST"));
	else
		str.Format(_T("NONE"));
	
	SetWindowTextW(text, str);
}


void FaceTrackingRendererManager::ContinueExpression(int win)
{
	HWND text = GetDlgItem(m_window, IDC_TEST7);

	if (win == happyCnt && win != 0)
	{
		count1++;
	}
	else if (win != happyCnt)
	{
		count1 = 0;
	}

	if (win == sadCnt && win != 0)
	{
		count2++;
	}
	else if (win != sadCnt)
	{
		count2 = 0;
	}
	if (win == surpriseCnt && win != 0)
	{
		count3++;
	}
	else if (win != surpriseCnt)
	{
		count3 = 0;
	}
	if (win == fearCnt && win != 0)
	{
		count4++;
	}
	else if (win != fearCnt)
	{
		count4 = 0;
	}
	if (win == angryCnt && win != 0)
	{
		count5++;
	}
	else if (win != angryCnt)
	{
		count5 = 0;
	}
	if (win == disgustCnt && win != 0)
	{
		count6++;
	}
	else if (win != disgustCnt)
	{
		count6 = 0;
	}

	CString str;

	str.Format(_T("지속시간:  %2.1f 초"), (float)max(count1, max(count2, max(count3, max(count4, max(count5, count6))))) / 5);
	SetWindowTextW(text, str);

	if (count1 > 25 || count2 > 25 || count3 > 25 || count4 > 25 || count5 > 25 || count6 > 25)
		EXPRESSION_FLAG = TRUE;

}
void FaceTrackingRendererManager::CalcMicroCount(int Win)
{
	if (Win == happyCnt) hCnt++;
	else hCnt = 0;
	if (Win == sadCnt) sCnt++;
	else sCnt = 0;
	if (Win == surpriseCnt)pCnt++;
	else pCnt = 0;
	if (Win == fearCnt) fCnt++;
	else fCnt = 0;
	if (Win == angryCnt) aCnt++;
	else aCnt = 0;
	if (Win == disgustCnt) dCnt++;
	else dCnt = 0;
	if (Win == neutralCnt) nCnt++;
	else nCnt = 0;
}
void FaceTrackingRendererManager::DisplayExpressionUsingEmoji(BOOL EXP_EMO[])
{
	enum {
		happy,
		sad,
		surprise,
		fear,
		angry,
		disgust,
		neutral
	};
	HWND text_emo = GetDlgItem(m_window, IDC_TEXT_EMO);
	HWND emo = GetDlgItem(m_window, IDC_EMO);
	CString str;

	HBITMAP hBmp;
	if (EXP_EMO[happy])
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

	if (EXP_EMO[sad])
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

	if (EXP_EMO[surprise])
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

	if (EXP_EMO[fear])
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

	if (EXP_EMO[angry])
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
	if (EXP_EMO[disgust])
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
	if (EXP_EMO[neutral])
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

double FaceTrackingRendererManager::GetDuration(int frame)
{
	return (double)frame / 30;
}
int FaceTrackingRendererManager::GetFrameSize(double second)
{
	return second * 30;			//60초 들어옴
}


void FaceTrackingRendererManager::DetermineExpression()
{
	enum
	{
		happy,
		sad,
		surprise,
		fear,
		angry,
		disgust,
		neutral
	};

	if (cursor == sizeOfWindow)
	{
		cursor = 0;
	}

	if (
		//(outerBrowRaiserLeft_LM + outerBrowRaiserRight_LM > 1) &&
		(Intensity[Smile] > 10 ||
		(lipCornerLeftUp_LM + lipCornerRightUp_LM >= 25)) &&
			(eyeOpenRight_LM < 10 && eyeOpenLeft_LM < 10)
		)
	{
		HAPPY = TRUE;
		happyCnt++;
	}
	

	if (((outerBrowDepressorLeft_LM + outerBrowDepressorRight_LM >5) ||
		BrowLowerRight_LM + BrowLowerLeft_LM > 1 ||
		(Intensity[BrowLoweredLeft] + Intensity[BrowLoweredRight] > 0)) &&
		(lipCornerRightDown_LM + lipCornerLeftDown_LM > 15) &&
		mouthOpen_LM<20)
	{
		SAD = TRUE;
		sadCnt++;
	}

	if (outerBrowRaiserLeft_LM > 1 && outerBrowRaiserRight_LM > 1 &&
		eyeOpenRight_LM > 10 && eyeOpenLeft_LM > 10 &&
		mouthOpen_LM > 20)
	{
		SURPRISE = TRUE;
		surpriseCnt++;
	}

	if ((BrowLowerRight_LM + BrowLowerLeft_LM > 1) &&
		(mouthOpen_LM>25 &&
		(lipCornerRightDown_LM + lipCornerLeftDown_LM>5)))
	{
		DISGUST = TRUE;
		disgustCnt++;
	}
	else if ((lipCornerRightDown_LM + lipCornerLeftDown_LM < 5) &&
		(BrowLowerRight_LM + BrowLowerLeft_LM>10) &&
		(eyeOpenLeft_LM + eyeOpenRight_LM>5))
	{
		FEAR = TRUE;
		fearCnt++;
	}
	else if (BrowLowerLeft_LM>10 && BrowLowerRight_LM>10 &&
		(lipCornerRightDown_LM + lipCornerLeftDown_LM < 5))
	{
		ANGRY = TRUE;
		angryCnt++;
	}

	if (HAPPY == FALSE && SAD == FALSE &&
		SURPRISE == FALSE && FEAR == FALSE &&
		ANGRY == FALSE && DISGUST == FALSE)
	{
		neutralCnt++;
	}

	HAPPY = SAD = SURPRISE = FEAR = ANGRY = DISGUST = FALSE;
}

void FaceTrackingRendererManager::CircularQueue1800()
{
	enum {
		smile,
		notsmile
	};
	if (cursor_s == sizeOfWindow)
	{
		cursor_s = 0;

		//initFront = true;
	}

	// update value...
	if (mouthOpen_LM > 10 && Intensity[MouthOpen] > 5)
	{
		if (cursor_s % 6 == 0)
		{
			if (frequency[smile] > frequency[notsmile])
			{
				winner = 1;								// 0.2초마다 smile? winner = 1 else winner = 0
			}
			else
			{
				winner = 0;
			}
			frequency[0] = frequency[1] = 0;
		}
		else
		{
			frequency[smile]++;		
									
		}
		ws_smile[cursor_s] = TRUE;

		cursor_s++;

	}
	else if (Intensity[MouthOpen]> 3 && (lipCornerLeftUp_LM + lipCornerRightUp_LM) > 1)
	{
		if (cursor_s % 6 == 0)
		{
			if (frequency[smile] > frequency[notsmile])
			{
				winner = 1;
			}
			else
			{
				winner = 0;
			}
			frequency[0] = frequency[1] = 0;
		}
		else
		{
			frequency[smile]++;
		}
		ws_smile[cursor_s] = TRUE;

		cursor_s++;
	}
	else
	{
		if (cursor_s % 6 == 0)
		{
			if (frequency[smile] > frequency[notsmile])
			{
				winner = 1;
			}
			else
			{
				winner = 0;
			}
			frequency[0] = frequency[1] = 0;
		}
		else
		{
			frequency[notsmile]++;
		}
		ws_smile[cursor_s] = FALSE;
		//QueuingFunc();
		cursor_s++;
	}

	//SubFunc();


}

void FaceTrackingRendererManager::CircularQueue300()
{
	CircularQueue1800();
	if (rear == sizeOfWindow_s)
	{
		rear = 0;
		initFront = true;
	}

	if (cursor_s % 6 == 0) // per 0.2 second. 
	{
		ws_subtleSmile[rear]= winner;// 1 or 0 
		Recording();
		rear++;
	}

}

void FaceTrackingRendererManager::Recording()
{

	if (!initFront)
	{
		if (rear == 0)
		{
			if (ws_subtleSmile[rear] == TRUE)
			{
				record++;
			}
		}
		else
		{
			if (IsChanged_r())
			{
				if (ws_subtleSmile[rear] == TRUE)
				{
					record++;
				}
			}
		}
	}
	else
	{
		if (rear == 0)
		{
			if (ws_subtleSmile[sizeOfWindow_s - 1] == FALSE && ws_subtleSmile[0] == TRUE)
			{
				if (ws_subtleSmile[rear] == TRUE)
				{
					record++;
				}
			}
		}
		else
		{
			if (IsChanged_r())
			{
				if (ws_subtleSmile[rear] == TRUE)
				{
					record++;
				}
			}
		}

		// front init
		if (IsChanged_f())
		{
			if (next_f == FALSE)
			{
				if (record > 0)
				{
					record--;
				}
			}
		}
	}

}

int FaceTrackingRendererManager::IsChanged_r()
{
	curr_r = ws_subtleSmile[rear];
	prev_r = ws_subtleSmile[rear-1];	//init? ws[front] : false 
	if (curr_r == prev_r)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int FaceTrackingRendererManager::IsChanged_f()
{
	curr_f = ws_subtleSmile[rear+1];
	next_f = ws_subtleSmile[rear+2];
	if (curr_f == next_f)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void FaceTrackingRendererManager::ShowHeartRate()
{
	HWND pulse1 = GetDlgItem(m_window, IDC_PULSE);
	WCHAR tempLine[64];
	swprintf_s<sizeof(tempLine) / sizeof(WCHAR) >(tempLine, L"실시간 심박수: %2.1f", hr);
	SetWindowTextW(pulse1, tempLine);
	HWND text = GetDlgItem(m_window, IDC_TEST6);
	CString str;

	//심박수 100이상이 1분간 지속될때
	if (hr > 99) hrcnt1++;
	else hrcnt1 = 0;
	if (hrcnt1 > 1800) PULSE_FLAG = TRUE;

	//심박수가 0~4초동안 정상치의 2배가 될때
	if (hrcnt2 == 0) {
		if (hr > 59 && hr < 100) 
		{
			pre_hr1 = hr;
		}
	}
	else if (hrcnt2 < 120)
	{
		if (hr >= 2 * pre_hr1 && pre_hr1 > 10 )
		{
			PULSE_FLAG = TRUE;
		}
	}
	else if(hrcnt2 > 120) 
	{
		hrcnt2 = 0;
	}
	hrcnt2++;

	//심박수가 1~5초동안 정상치의 2배가 될때
	if (hrcnt3 == 30) {
		if (hr > 59 && hr < 100)
		{
			pre_hr2 = hr;
		}
	}
	else if (hrcnt3 < 150)
	{
		if (hr >= 2 * pre_hr2 && pre_hr2 > 10)
		{
			PULSE_FLAG = TRUE;
		}
	}
	else if (hrcnt3 > 150)
	{
		hrcnt3 = 0;
	}
	hrcnt3++;

	//심박수가 2~6초동안 정상치의 2배가 될때
	if (hrcnt4 == 60) {
		if (hr > 59 && hr < 100)
		{
			pre_hr3 = hr;
		}
	}
	else if (hrcnt4 < 180)
	{
		if (hr >= 2 * pre_hr3 && pre_hr3 > 10)
		{
			PULSE_FLAG = TRUE;
		}
	}
	else if (hrcnt4 > 180)
	{
		hrcnt4 = 0;
	}
	hrcnt4++;

	//심박수가 3~7초동안 정상치의 2배가 될때
	if (hrcnt5 == 90) {
		if (hr > 59 && hr < 100)
		{
			pre_hr4 = hr;
		}
	}
	else if (hrcnt5 < 210)
	{
		if (hr >= 2 * pre_hr4 && pre_hr4 > 10)
		{
			PULSE_FLAG = TRUE;
		}
	}
	else if (hrcnt5 > 210)
	{
		hrcnt5 = 0;
	}
	hrcnt5++;

	str.Format(_T("기준심박수: %2.1f"), max(pre_hr1,max(pre_hr2,max(pre_hr3, pre_hr4))));
	SetWindowTextW(text, str);

	if (hr != 0) {
		if (hrcnt6 == 0)compare_hr1 = hr;
		else if (hrcnt6 == 150)compare_hr2 = hr;
		else if (hrcnt6 == 300)compare_hr3 = hr;
		else if (hrcnt6 > 300)hrcnt6 = 0;

		hrcnt6++;

		if (compare_hr1 == compare_hr2 && compare_hr2 == compare_hr3)
			PULSE_FLAG = FALSE;
	}
}

void FaceTrackingRendererManager::DetermineFlagOutOfHeadPos()
{
	enum
	{
		pitch,
		yaw
	};
	if ((10 <= angles.pitch && angles.pitch <= 15) || (-15 <= angles.pitch && angles.pitch< -10))
	{
		PITCH = TRUE;
	}
	if (10 <= angles.yaw && angles.yaw <= 15 || (-15 <= angles.yaw && angles.yaw< -10))
	{
		YAW = TRUE;
	}

	if (PITCH == TRUE || YAW == TRUE)
	{
		slidingWindow_Range[rear_Range] = TRUE;
	}
	else
	{
		slidingWindow_Range[rear_Range] = FALSE;
	}

	PITCH = YAW = FALSE;
}

void FaceTrackingRendererManager::RecordingOutOfRange()
{
	if (rear_Range == sizeOfWindow_R)
	{
		rear_Range = 0;
		initFront_Range = true;
	}
	DetermineFlagOutOfHeadPos();

	if (!initFront_Range)
	{
		if (rear_Range == 0)
		{
			if (slidingWindow_Range[rear_Range] == TRUE)
			{
				record_Range++;
			}
		}
		else
		{
			if (IsChangedRange_r())
			{
				if (slidingWindow_Range[rear_Range] == TRUE)
				{
					record_Range++;
				}
			}
		}
	}
	else
	{
		if (rear_Range == 0)
		{
			if (slidingWindow_Range[sizeOfWindow_R - 1] == FALSE && slidingWindow_Range[0] == TRUE)
			{
				if (slidingWindow_Range[rear_Range] == TRUE)
				{
					record_Range++;
				}
			}
		}
		else
		{
			if (IsChangedRange_r())
			{
				if (slidingWindow_Range[rear_Range] == TRUE)
				{
					record_Range++;
				}
			}
		}

		// front init
		if (IsChangedRange_f())
		{
			if (slidingWindow_Range[rear_Range + 2] == FALSE)
			{
				if (record_Range > 0)
				{
					record_Range--;
				}
			}
		}

	}

	rear_Range++;

	ShowHeadMovementRecord();
}

void FaceTrackingRendererManager::ShowHeadMovementRecord()
{
	HWND text = GetDlgItem(m_window, IDC_RECORD_RANGE);
	HWND text1 = GetDlgItem(m_window, IDC_POSE1);
	CString str;
	str.Format(_T("분당횟수:  %d"), record_Range);
	SetWindowTextW(text, str);


	if (angles.yaw<5 && angles.yaw>-5) {
		str.Format(_T("정  면"));
	}
	else if (angles.yaw < -5) {
		str.Format(_T("좌  측"));
	}
	else if (angles.yaw > 5) {
		str.Format(_T("우  측"));
	}

	SetWindowTextW(text1, str);
}
int FaceTrackingRendererManager::IsChangedRange_r()
{
	if (slidingWindow_Range[rear_Range] == slidingWindow_Range[rear_Range - 1])
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
int FaceTrackingRendererManager::IsChangedRange_f()
{
	if (slidingWindow_Range[rear_Range + 1] == slidingWindow_Range[rear_Range + 2])
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
void FaceTrackingRendererManager::FlagOnOff()
{
	if ((5 <= record_Range && record_Range <= 8) || ( 18 <= record_Range && record_Range <=28) )
	{
		HEADMOTION_FLAG = TRUE;
	}

	if ((2 <= record && record <= 6))
	{
		SMILE_FLAG = TRUE;
	}

	if ((outerBrowDepressorLeft_LM + outerBrowDepressorRight_LM) == 0 && (Intensity[ClosedEyeLeft] + Intensity[ClosedEyeRight])>150)
	{
		BLINK_FLAG = TRUE;
	}

	if (count1 > 25 || count2 > 25 || count3 > 25 || count4 > 25 || count5 > 25 || count6 > 25)
	{
		EXPRESSION_FLAG = TRUE;
	}

	HWND text = GetDlgItem(m_window, IDC_TEST2);
	CString str;
	str.Format(_T("분당 횟수:  %d"), record);
	SetWindowTextW(text, str);
}
