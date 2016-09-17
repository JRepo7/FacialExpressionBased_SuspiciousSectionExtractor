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
	cursor = cursor_d=cursor_s=0 ;
	sizeOfWindow = GetFrameSize(6);
	sizeOfWindow_d = GetFrameSize(6);
	sizeOfWindow_s = GetFrameSize(1);
	sizeOfWindow_R = GetFrameSize(6);

	candidEmo[happy] = candidEmo[sad] = candidEmo[surprise] = candidEmo[fear] = candidEmo[angry] = candidEmo[disgust] = candidEmo[neutral] = 0;
	//memset(slidingWindow, 0, SizeOfWindow);
	record = rear = front = 0;
	record_Range = rear_Range = front_Range = 0;

	initFront = false;
	curr_r = prev_r = next_f = curr_f = false;

	winner = 0;
	angles.yaw = angles.pitch = hr = 0;

	Int1 = 0;
	hrcnt1=hrcnt2=hrcnt3=hrcnt4=hrcnt5=hrcnt6=0;

	sumEyesTurnLeft=sumEyesTurnRight =gazecnt = 0;
	count1 = count2 = count3 = count4 = count5 = count6 = 0;
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

	cursor =cursor_s= rear = 0;
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

void FaceTrackingRendererManager::CaptureSubtleExpression() {
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
	MicroExpression(Win);
	if (Win == neutralCnt)
	{
		EXP_EMO[neutral] = TRUE;

		DisplayExpressionUsingEmoji(EXP_EMO);

		EXP_EMO[neutral] = FALSE;
	}
	else if (Win ==happyCnt)
	{
		EXP_EMO[happy] = TRUE;

		DisplayExpressionUsingEmoji(EXP_EMO);

		EXP_EMO[happy] = FALSE;
	}
	else if (Win == sadCnt)
	{
		EXP_EMO[sad] = TRUE;

		DisplayExpressionUsingEmoji(EXP_EMO);

		EXP_EMO[sad] = FALSE;
	}
	else if (Win == surpriseCnt)
	{
		EXP_EMO[surprise] = TRUE;

		DisplayExpressionUsingEmoji(EXP_EMO);

		EXP_EMO[surprise] = FALSE;
	}
	else if (Win == fearCnt)
	{
		EXP_EMO[fear] = TRUE;

		DisplayExpressionUsingEmoji(EXP_EMO);

		EXP_EMO[fear] = FALSE;
	}
	else if (Win == angryCnt)
	{
		EXP_EMO[angry] = TRUE;

		DisplayExpressionUsingEmoji(EXP_EMO);

		EXP_EMO[angry] = FALSE;
	}
	else if (Win == disgustCnt)
	{
		EXP_EMO[disgust] = TRUE;

		DisplayExpressionUsingEmoji(EXP_EMO);

		EXP_EMO[disgust] = FALSE;
	}

	happyCnt= sadCnt= surpriseCnt= fearCnt= angryCnt= disgustCnt= neutralCnt=0;
}

void FaceTrackingRendererManager::ContinueExpression(int win)
{
	if (win == happyCnt && win != 0) count1++;
	else if (win != happyCnt) count1 = 0;

	if (win == sadCnt && win != 0) count2++;
	else if (win != sadCnt) count2 = 0;

	if (win == surpriseCnt && win != 0) count3++;
	else if (win != surpriseCnt) count3 = 0;

	if (win == fearCnt && win != 0) count4++;
	else if (win != fearCnt) count4 = 0;

	if (win == angryCnt && win != 0) count5++;
	else if (win != angryCnt) count5 = 0;

	if (win == disgustCnt && win != 0) count6++;
	else if (win != disgustCnt) count6 = 0;

	if (count1 > 25 || count2 > 25 || count3 > 25 || count4 > 25 || count5 > 25 || count6 > 25)
		EXPRESSION_FLAG = TRUE;

}
void FaceTrackingRendererManager::MicroExpression(int win)
{

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
	return second * 30;			//60�� ����
}

int FaceTrackingRendererManager::VotingUsingSlidingWindow(int duration)
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

	// voting
	// make a histogram
	for (int i = 1; i++; duration)				
	{
		if (slidingWindow_d[cursor_d] == happy)
		{
			candidEmo[happy]++;
		}
		else if (slidingWindow_d[cursor_d] == sad)
		{
			candidEmo[sad]++;
		}
		else if (slidingWindow_d[cursor_d] == surprise)
		{
			candidEmo[surprise]++;
		}
		else if (slidingWindow_d[cursor_d] == fear)

		{
			candidEmo[fear]++;
		}
		else if (slidingWindow_d[cursor_d] == angry)
		{
			candidEmo[angry]++;
		}
		else if (slidingWindow_d[cursor_d] == disgust)
		{
			candidEmo[disgust]++;
		}
		else
		{
			slidingWindow_d[neutral]++;
		}

	}
	//WhoIsWinner? 
	int Win = max(candidEmo[happy],
		max(candidEmo[sad],
			max(candidEmo[surprise],
				max(candidEmo[fear],
					max(candidEmo[angry],
						max(candidEmo[disgust], candidEmo[neutral]))))));

		if (candidEmo[happy] == Win)
		{
			return happy;
		}
		else if (candidEmo[sad] == Win)
		{
			return sad;
		}
		else if (candidEmo[fear] == Win)
		{
			return fear;
		}
		else if (candidEmo[angry] == Win)
		{
			return angry;
		}
		else if (candidEmo[disgust] == Win)
		{
			return disgust;
		}
		else if (candidEmo[angry] == Win)
		{
			return angry;
		}
		else
		{
			return neutral;
		}

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
		(outerBrowRaiserLeft_LM + outerBrowRaiserRight_LM > 1) &&
		(Intensity[Smile] > 10 ||
		(lipCornerLeftUp_LM > 5 && lipCornerRightUp_LM > 5)) &&
			(eyeOpenRight_LM < 10 && eyeOpenLeft_LM < 10))
	{
		HAPPY = TRUE;
		happyCnt++;
		slidingWindow[cursor] = happy;
	}
	

	if (((outerBrowDepressorLeft_LM + outerBrowDepressorRight_LM >1) ||
		(Intensity[BrowLoweredLeft]>10 && Intensity[BrowLoweredRight] > 10)) &&
		(lipCornerRightDown_LM + lipCornerLeftDown_LM > 3) &&
		mouthOpen_LM<25)
	{
		SAD = TRUE;
		sadCnt++;
		slidingWindow[cursor] = sad;
	}

	if (outerBrowRaiserLeft_LM > 1 && outerBrowRaiserRight_LM > 1 &&
		eyeOpenRight_LM > 10 && eyeOpenLeft_LM > 10 &&
		mouthOpen_LM > 20)
	{
		SURPRISE = TRUE;
		surpriseCnt++;
		slidingWindow[cursor] = surprise;
	}

	if ((BrowLowerRight_LM + BrowLowerLeft_LM > 10) &&
		(mouthOpen_LM>25 &&
		(lipCornerRightDown_LM + lipCornerLeftDown_LM>5)))
	{
		DISGUST = TRUE;
		disgustCnt++;
		slidingWindow[cursor] = disgust;
	}
	else if ((lipCornerRightDown_LM + lipCornerLeftDown_LM < 5) &&
		(BrowLowerRight_LM + BrowLowerLeft_LM>10) &&
		(eyeOpenLeft_LM + eyeOpenRight_LM>5))
	{
		FEAR = TRUE;
		fearCnt++;
		slidingWindow[cursor] = fear;
	}
	else if (BrowLowerLeft_LM>10 && BrowLowerRight_LM>10 &&
		(lipCornerRightDown_LM + lipCornerLeftDown_LM < 5))
	{
		ANGRY = TRUE;
		angryCnt++;
		slidingWindow[cursor] = angry;
	}

	if (HAPPY == FALSE && SAD == FALSE &&
		SURPRISE == FALSE && FEAR == FALSE &&
		ANGRY == FALSE && DISGUST == FALSE)
	{
		neutralCnt++;
		slidingWindow[neutral] = angry;
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
				winner = 1;								// 0.2�ʸ��� smile? winner = 1 else winner = 0
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

		//QueuingFunc();
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

		//QueuingFunc();
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
		ws_subtleSmile[rear]= winner;
		Recording();
		rear++;
	}


	SubFunc();
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

void FaceTrackingRendererManager::SubFunc()
{
	HWND text = GetDlgItem(m_window, IDC_RECORD);
	CString str;
	str.Format(_T("����������: %d"), record);
	SetWindowTextW(text, str);
}

void FaceTrackingRendererManager::Avoidgaze()
{
	HWND pose1 = GetDlgItem(m_window, IDC_POSE1);
	HWND pose2 = GetDlgItem(m_window, IDC_POSE2);
	WCHAR tempLine[64];
	swprintf_s<sizeof(tempLine) / sizeof(WCHAR) >(tempLine, L"Yaw : %.0f", angles.yaw);
	SetWindowTextW(pose1, tempLine);
	swprintf_s<sizeof(tempLine) / sizeof(WCHAR) >(tempLine, L"Pitch: %.0f", angles.pitch);
	SetWindowTextW(pose2, tempLine);
	HWND text1 = GetDlgItem(m_window, IDC_TEST1);
	HWND text2 = GetDlgItem(m_window, IDC_TEST2);
	HWND text3 = GetDlgItem(m_window, IDC_TEST3);
	HWND text4 = GetDlgItem(m_window, IDC_TEST4);
	HWND text5 = GetDlgItem(m_window, IDC_TEST5);
	HWND text6 = GetDlgItem(m_window, IDC_TEST6);
	HWND text7 = GetDlgItem(m_window, IDC_TEST7);
	HWND text8 = GetDlgItem(m_window, IDC_TEST8);

	

	CString str;

	str.Format(_T("EyesTurnLeft:      %d"), Intensity[EyesTurnLeft]);
	SetWindowTextW(text1, str);
	str.Format(_T("EyesTurnRight:      %d"), Intensity[EyesTurnRight]);
	SetWindowTextW(text2, str);
	str.Format(_T("gazecnt: %d"), gazecnt);
	SetWindowTextW(text3, str);
	str.Format(_T("EyesDown:     %d"), Intensity[EyesDown]);
	SetWindowTextW(text4, str);



	if (Int1<15)
	{
		sumEyesTurnLeft += Intensity[EyesTurnLeft];
		sumEyesTurnRight += Intensity[EyesTurnRight];
		sumEyeyaw += angles.yaw;
		sumEyepitch += angles.pitch;

		Int1++;
	}
	else if (Int1 == 15)
	{
		avgEyesTurnLeft =(int) sumEyesTurnLeft / 15;
		avgEyesTurnRight = (int)sumEyesTurnRight / 15;
		avgEyeyaw = (int)sumEyeyaw / 15;
		avgEyepitch = (int)sumEyepitch / 15;

		str.Format(_T("avgEyesTurnLeft %d"), avgEyesTurnLeft);
		SetWindowTextW(text5, str);
		str.Format(_T("avgEyesTurnRight %d"), avgEyesTurnRight);
		SetWindowTextW(text6, str);
		str.Format(_T("avgEyeyaw %d"), avgEyeyaw);
		SetWindowTextW(text7, str);
		str.Format(_T("avgEyepitch %d"), avgEyepitch);
		SetWindowTextW(text8, str);
		Int1++;
	}
	else
	{
		if (abs(avgEyeyaw - angles.yaw) < 5 && abs(avgEyepitch - angles.pitch) < 5)
		{
			if (avgEyesTurnLeft == 0) {
				if (abs(avgEyesTurnRight - Intensity[EyesTurnRight]) > 40)
					gazecnt++;
				else
					gazecnt = 0;
			}
			else if (avgEyesTurnRight == 0) {
				if (abs(avgEyesTurnLeft - Intensity[EyesTurnLeft]) > 40)
					gazecnt++;
				else
					gazecnt = 0;
			}
			else
			{
				sumEyesTurnLeft = 0;
				sumEyesTurnRight = 0;
				Int1 = 0;
			}

		}
		else
		{
			gazecnt = 0;
		}
	}

	if(gazecnt>120) GAZE_FLAG = TRUE;
}

void FaceTrackingRendererManager::ShowHeartRate()
{
	HWND pulse1 = GetDlgItem(m_window, IDC_PULSE);
	WCHAR tempLine[64];
	swprintf_s<sizeof(tempLine) / sizeof(WCHAR) >(tempLine, L"HR: %f", hr);
	SetWindowTextW(pulse1, tempLine);

	//�ɹڼ� 100�̻��� 1�а� ���ӵɶ�
	if (hr > 99) hrcnt1++;
	else hrcnt1 = 0;
	if (hrcnt1 > 1800) PULSE_FLAG = TRUE;

	//�ɹڼ��� 0~4�ʵ��� ����ġ�� 2�谡 �ɶ�
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

	//�ɹڼ��� 1~5�ʵ��� ����ġ�� 2�谡 �ɶ�
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

	//�ɹڼ��� 2~6�ʵ��� ����ġ�� 2�谡 �ɶ�
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

	//�ɹڼ��� 3~7�ʵ��� ����ġ�� 2�谡 �ɶ�
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
	CString str;
	str.Format(_T("DetectLyingOutOfMovement: %d"), record_Range);
	SetWindowTextW(text, str);
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

	if ((outerBrowDepressorLeft_LM + outerBrowDepressorRight_LM) == 0 && (Intensity[ClosedEyeLeft] + Intensity[ClosedEyeRight])>190)
	{
		BLINK_FLAG = TRUE;
	}
}