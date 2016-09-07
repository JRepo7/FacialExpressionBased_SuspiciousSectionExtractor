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
	PValueInit();
	HAPPY = SAD = SURPRISE = FEAR = ANGRY = DISGUST= NEUTRAL= FALSE;
	EXP_HAPPY= EXP_SAD= EXP_SURPRISE= EXP_FEAR= EXP_ANGRY= EXP_DISGUST= EXP_NEUTRAL=FALSE;
	SetExpressionArguments();
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
	PValueInit();
}

void FaceTrackingRendererManager::PValueInit()
{
	for (int i = 0; i < 6; i++)
	{
		pAvgValue[i] = 0;
	}
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

void FaceTrackingRendererManager::DisplayExpressionI()
{
	// 14
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

	str.Format(_T("BrowRaised_L: \t\t %d"), Intensity[0]);
	SetWindowTextW(text_Exp1, str);
	str.Format(_T("BrowRaised_R: \t\t %d"), Intensity[1]);
	SetWindowTextW(text_Exp2, str);
	str.Format(_T("BrowLowered_L: \t\t %d"), Intensity[2]);
	SetWindowTextW(text_Exp3, str);
	str.Format(_T("BrowLowered_R:\t\t %d"), Intensity[3]);
	SetWindowTextW(text_Exp4, str);
	str.Format(_T("Smile: \t\t\t %d"), Intensity[4]);
	SetWindowTextW(text_Exp5, str);
	str.Format(_T("Kiss: \t\t\t %d"), Intensity[5]);
	SetWindowTextW(text_Exp6, str);
	str.Format(_T("MouthOpen: \t\t %d"), Intensity[6]);
	SetWindowTextW(text_Exp7, str);
	str.Format(_T("ClosedEyeLeft: \t\t %d"), Intensity[7]);
	SetWindowTextW(text_Exp8, str);
	str.Format(_T("ClosedEyeRight: \t\t %d"), Intensity[8]);
	SetWindowTextW(text_Exp9, str);
	str.Format(_T("EyesTurnLeft: \t\t %d"), Intensity[9]);
	SetWindowTextW(text_Exp10, str);
	str.Format(_T("EyesTurnRight: \t\t %d"), Intensity[10]);
	SetWindowTextW(text_Exp11, str);
	str.Format(_T("EyesUp: \t\t\t %d"), Intensity[11]);
	SetWindowTextW(text_Exp12, str);
	str.Format(_T("EyesDown: \t\t %d"), Intensity[12]);
	SetWindowTextW(text_Exp13, str);
}

void FaceTrackingRendererManager::DisplayLandmarkI()
{
	// 11 
	HWND text_I1 = GetDlgItem(m_window, IDC_TEXT_I1);
	HWND text_I2 = GetDlgItem(m_window, IDC_TEXT_I2);
	HWND text_I3 = GetDlgItem(m_window, IDC_TEXT_I3);
	HWND text_I4 = GetDlgItem(m_window, IDC_TEXT_I4);
	HWND text_I5 = GetDlgItem(m_window, IDC_TEXT_I5);
	HWND text_I6 = GetDlgItem(m_window, IDC_TEXT_I6);
	HWND text_I7 = GetDlgItem(m_window, IDC_TEXT_I7);
	HWND text_I8 = GetDlgItem(m_window, IDC_TEXT_I8);
	HWND text_I9 = GetDlgItem(m_window, IDC_TEXT_I9);
	HWND text_I10 = GetDlgItem(m_window, IDC_TEXT_I10);
	HWND text_I11 = GetDlgItem(m_window, IDC_TEXT_I11);
	HWND text_I12 = GetDlgItem(m_window, IDC_TEXT_I12);

	CString str;

	str.Format(_T("outerBrowRaiserLeft_LM :\t\t  %f"), outerBrowRaiserLeft_LM);
	SetWindowTextW(text_I1, str);
	str.Format(_T("outerBrowRaiserRight_LM :\t\t  %f"), outerBrowRaiserRight_LM);
	SetWindowTextW(text_I2, str);

	str.Format(_T("BrowLowerRight_LM :\t\t\t  %f"), BrowLowerRight_LM);
	SetWindowTextW(text_I3, str);
	str.Format(_T("BrowLowerLeft_LM :\t\t\t  %f"), BrowLowerLeft_LM);
	SetWindowTextW(text_I4, str);

	str.Format(_T("upperLipRaiser_LM:\t\t\t  %f"), upperLipRaiser_LM);
	SetWindowTextW(text_I5, str);

	str.Format(_T("lipCornerRightDown_LM:\t  %f"), lipCornerRightDown_LM);
	SetWindowTextW(text_I6, str);
	str.Format(_T("lipCornerLeftDown_LM:\t\t  %f"), lipCornerLeftDown_LM);
	SetWindowTextW(text_I7, str);

	str.Format(_T("eyeOpenRight_LM:\t\t\t   %f"), eyeOpenRight_LM);
	SetWindowTextW(text_I8, str);
	str.Format(_T("eyeOpenLeft_LM:\t\t\t   %f"), eyeOpenLeft_LM);
	SetWindowTextW(text_I9, str);

	str.Format(_T("lipCornerRightUp_LM:\t\t  %f"), lipCornerRightUp_LM);
	SetWindowTextW(text_I10, str);
	str.Format(_T("lipCornerLeftUp_LM:\t\t  %f"), lipCornerLeftUp_LM);
	SetWindowTextW(text_I11, str);

	str.Format(_T("mouthOpen_LM:\t\t\t  %f"), mouthOpen_LM);
	SetWindowTextW(text_I12, str);
}

void FaceTrackingRendererManager::CvtLandmarkToIntensity()
{
	double ratio;

	ratio = (FacialPoint[14].y - FacialPoint[3].y) / tOuterBrowRaiserRight;
	if (ratio > 1)
	{
		outerBrowRaiserRight_LM = (ratio * 100) - 100;
		outerBrowDepressorRight_LM = 0;//¹Ý´ë Á¶°ÇÀÇ º¯¼öÀÎ outerBrowDepressorRight_LM°¡ ÀÌÀü °ªÀ» °®´Â°æ¿ì°¡ ÀÖÀ¸¹Ç·Î 0À¸·Î setÇØÁÜ
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

	//fp36(À­ÀÔ¼ú)ÀÇ À§Ä¡°¡ ¿Ã¶ó°¡¸é ±âÁØÁ¡ fp31(ÀÎÁß)°ú fp36(À­ÀÔ¼ú) °£ÀÇ ³ôÀÌÂ÷´Â ÁÙ¾îµë Áï if(ratio < 1) °¡ ÀûÀý
	//**Çø¿À°¨À» ³ªÅ¸³»´Âµ¥ ±»ÀÌ ÇÊ¿äÇÑ°¡
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

	DisplayExpressionI();
	DisplayLandmarkI();
}

void FaceTrackingRendererManager::SetExpressionArguments()
{
//	double happy5[5], sad6[5], surprise5[5], fear4;
//	happy5[0] = { outerBrowRaiserRight_LM };
//	happy5[1] = { outerBrowRaiserLeft_LM };
//	happy5[2] = { (double)Intensity[Smile] };
//	happy5[3] = { lipCornerLeftUp_LM };
//	happy5[4] = { lipCornerRightUp_LM };
//
//	sad6[0] = { (double)Intensity[BrowLoweredLeft] };
//	sad6[1] = { (double)Intensity[BrowLoweredRight] };
//	sad6[2] = { lipCornerRightDown_LM };
//	sad6[3] = { lipCornerLeftDown_LM };
//	sad6[4] = { outerBrowDepressorRight_LM };
//	sad6[5] = { outerBrowDepressorLeft_LM };
//
//	surprise5[0] = { outerBrowRaiserLeft_LM };
//	surprise5[1] = { outerBrowRaiserRight_LM };
//	surprise5[2] = { eyeOpenRight_LM };
//	surprise5[3] = { eyeOpenLeft_LM };
//	surprise5[4] = { (double)Intensity[MouthOpen] };
//
//	fear4[0] = { BrowLowerRight_LM };
//	fear4[1] = { BrowLowerLeft_LM };
//	fear4[2] = { upperLipRaiser_LM };
//	fear4[3] = { lipCornerRightDown_LM };
//	//arg4 += outerBrowRaiserRight_LM;
//	//arg5 += outerBrowRaiserLeft_LM;
//
//	angry2[0] = { BrowLowerLeft_LM };
//	angry2[1] = { BrowLowerRight_LM };
//	//eyeOpenRight_LM,
//	//outerBrowRaiserRight_LM,
//	//outerBrowRaiserLeft_LM
//
//	disgust5[0] = { BrowLowerRight_LM };
//	disgust5[1] = { BrowLowerLeft_LM };
//	disgust5[2] = { upperLipRaiser_LM };
//	disgust5[3] = { lipCornerRightDown_LM };
//	disgust5[4] = { lipCornerLeftDown_LM };
}

void FaceTrackingRendererManager::DetermineExpression()
{
//	double happy5[5], sad6[5], surprise5[5], fear4[4], angry2[2], disgust5[5];
//
//	happy5[0] = { outerBrowRaiserRight_LM };
//	happy5[1] = { outerBrowRaiserLeft_LM };
//	happy5[2] = { (double)Intensity[Smile] };
//	happy5[3] = { lipCornerLeftUp_LM };
//	happy5[4] = { lipCornerRightUp_LM };
//
//	sad6[0] = { (double)Intensity[BrowLoweredLeft] };
//	sad6[1] = { (double)Intensity[BrowLoweredRight] };
//	sad6[2] = { lipCornerRightDown_LM };
//	sad6[3] = { lipCornerLeftDown_LM };
//	sad6[4] = { outerBrowDepressorRight_LM };
//	sad6[5] = { outerBrowDepressorLeft_LM };
//
//	surprise5[0] = { outerBrowRaiserLeft_LM };
//	surprise5[1] = { outerBrowRaiserRight_LM };
//	surprise5[2] = { eyeOpenRight_LM };
//	surprise5[3] = { eyeOpenLeft_LM };
//	surprise5[4] = { (double)Intensity[MouthOpen] };
//
//	fear4[0] = { BrowLowerRight_LM };
//	fear4[1] = { BrowLowerLeft_LM };
//	fear4[2] = { upperLipRaiser_LM };
//	fear4[3] = { lipCornerRightDown_LM };
//	//arg4 += outerBrowRaiserRight_LM;
//	//arg5 += outerBrowRaiserLeft_LM;
//
//	angry2[0] = { BrowLowerLeft_LM };
//	angry2[1] = { BrowLowerRight_LM };
//	//eyeOpenRight_LM,
//	//outerBrowRaiserRight_LM,
//	//outerBrowRaiserLeft_LM
//
//	disgust5[0] = { BrowLowerRight_LM };
//	disgust5[1] = { BrowLowerLeft_LM };
//	disgust5[2] = { upperLipRaiser_LM };
//	disgust5[3] = { lipCornerRightDown_LM };
//	disgust5[4] = { lipCornerLeftDown_LM };

	if (
		//outerBrowRaiserRight_LM > 1 && outerBrowRaiserLeft_LM > 1 &&
		(Intensity[Smile] > 10 || (lipCornerLeftUp_LM > 5 && lipCornerRightUp_LM > 5)) && 
		(eyeOpenRight_LM < 10 && eyeOpenLeft_LM < 10)
		)
	{
		HAPPY = TRUE;
		double arg[]
			= { 
			outerBrowRaiserRight_LM,
			outerBrowRaiserLeft_LM,
			Intensity[Smile],
			lipCornerLeftUp_LM,
			lipCornerRightUp_LM
		};

		happyCnt++;

		PrepAvgValue(arg, sizeof(arg) / sizeof(arg[0]));

		DisplayExpressionUsingEmoji();

		if (happyCnt == 100)
		{
			CreateTextFile(happyCnt);
			happyCnt = 0;
			PValueInit();
		}
		//HAPPY = FALSE;
	}
	//*/

	if (( (outerBrowDepressorLeft_LM + outerBrowDepressorRight_LM >1) ||(Intensity[BrowLoweredLeft]>10 && Intensity[BrowLoweredRight] > 10) )&& 
		(lipCornerRightDown_LM + lipCornerLeftDown_LM > 5) && mouthOpen_LM<25)
	{
		SAD = TRUE;
		sadCnt++;
		
		//PrepAvgValue(arg, sizeof(arg) / sizeof(arg[0]));

		DisplayExpressionUsingEmoji();

		if (sadCnt == 1000)
		{
			CreateTextFile(sadCnt);
			sadCnt=0;
			PValueInit();
		}

		//SAD = FALSE;
	}
	//*/
	
	if (outerBrowRaiserLeft_LM > 1 && outerBrowRaiserRight_LM > 1 && eyeOpenRight_LM > 10 && eyeOpenLeft_LM > 10 &&
		mouthOpen_LM > 20)
	{
		SURPRISE = TRUE;
		surpriseCnt++;

		//PrepAvgValue(surprise5, sizeof(surprise5) / sizeof(surprise5[0]));

		DisplayExpressionUsingEmoji();

		if (surpriseCnt == 1000)
		{
			CreateTextFile(surpriseCnt);

			surpriseCnt= 0;
			PValueInit();
		}
		//SURPRISE = FALSE;
	}
	//*/

	if ((BrowLowerRight_LM + BrowLowerLeft_LM > 10) &&
		(mouthOpen_LM>25 &&(lipCornerRightDown_LM + lipCornerLeftDown_LM>5))
		)
	{
		DISGUST = TRUE;
		disgustCnt++;
		double arg[]=
		{
			BrowLowerRight_LM,
			BrowLowerLeft_LM,
			Intensity[MouthOpen],
			lipCornerRightDown_LM,
			lipCornerLeftDown_LM
		};
		PrepAvgValue(arg, sizeof(arg) / sizeof(arg[0]));

		DisplayExpressionUsingEmoji();

		if (disgustCnt == 500)
		{
			CreateTextFile(disgustCnt);

			disgustCnt = 0;
			PValueInit();
		}
		//DISGUST = FALSE;
	}
	else if (
		(lipCornerRightDown_LM + lipCornerLeftDown_LM < 5) &&
		(BrowLowerRight_LM + BrowLowerLeft_LM>10) &&
		(eyeOpenLeft_LM + eyeOpenRight_LM>5)&&
	)

		//&&(EXP_NEUTRAL==TRUE||EXP_FEAR==TRUE)) 
		//outerBrowRaiserRight_LM >1 && outerBrowRaiserLeft_LM>1)
	{
		FEAR = TRUE;
		fearCnt++;

		//PrepAvgValue(fear4, sizeof(fear4) / sizeof(fear4[0]));

		DisplayExpressionUsingEmoji();

		if (fearCnt == 1000)
		{
			CreateTextFile(fearCnt);

			fearCnt = 0;
			PrepValue();
		}
	}
	else if (BrowLowerLeft_LM>10 && BrowLowerRight_LM>10 && 
		(lipCornerRightDown_LM + lipCornerLeftDown_LM < 5))
		//Intensity[BrowLoweredLeft]>1 && Intensity[BrowLoweredRight]>10 && 
		//outerBrowRaiserLeft_LM>10 && outerBrowRaiserRight_LM&&
		//eyeOpenLeft_LM>10&&eyeOpenRight_LM>10)
	{
		ANGRY = TRUE;
		angryCnt++;

		//PrepAvgValue(angry2, sizeof(angry2) / sizeof(angry2[0]));

		DisplayExpressionUsingEmoji();

		if (angryCnt == 1000)
		{
			CreateTextFile(angryCnt);
			angryCnt = 0;
			PValueInit();
		}
	}

	//*/

	//Áß¸³
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

void FaceTrackingRendererManager::PrepAvgValue(double arg[], int length)
{
	for (int i = 0; i < length; i++)
	{
		pAvgValue[i] += arg[i];
	}
}

void FaceTrackingRendererManager::CreateTextFile(int cnt)
{
	FILE *fp = fopen("data.txt", "a");
	if (HAPPY)
	{
		fprintf(fp, "¿À¸¥ÂÊ ¹Ù±ù´«½ç  : \t%f\n", pAvgValue[0] / cnt);
		fprintf(fp, "¿ÞÂÊ ¹Ù±ù´«½ç :  \t%f\n", pAvgValue[1] / cnt);
		fprintf(fp, "¿ôÀ½ Á¤µµ :\t%f\n", pAvgValue[2] / cnt);
		fprintf(fp, "¿ÞÂÊ ÀÔ¼úÄÚ³Ê   : \t%f\n", pAvgValue[3]/ cnt);
		fprintf(fp, "¿À¸¥ÂÊ ÀÔ¼úÄÚ³Ê  : \t%f\n", pAvgValue[4]/ cnt);
	}
	if (SAD)
	{
		fprintf(fp, "¿À¸¥ÂÊ ´«½ç  : \t%f\n", pAvgValue[0] / cnt);
		fprintf(fp, "¿ÞÂÊ ´«½ç :  \t%f\n", pAvgValue[1] / cnt);
		fprintf(fp, "¿ÞÂÊ ÀÔ¼úÄÚ³Ê   : \t%f\n", pAvgValue[2] / cnt);
		fprintf(fp, "¿À¸¥ÂÊ ÀÔ¼úÄÚ³Ê  : \t%f\n", pAvgValue[3] / cnt);
		fprintf(fp, "¿À¸¥ÂÊ ¹Ù±ù´«½ç  : \t%f\n", pAvgValue[4] / cnt);
		fprintf(fp, "¿ÞÂÊ ¹Ù±ù´«½ç  : \t%f\n", pAvgValue[5] / cnt);
	}
	if (SURPRISE)
	{
		fprintf(fp, "¿À¸¥ÂÊ ¹Ù±ù´«½ç  : \t%f\n", pAvgValue[0]/ cnt);
		fprintf(fp, "¿ÞÂÊ ¹Ù±ù´«½ç :  \t%f\n", pAvgValue[1]/ cnt);
		fprintf(fp, "¿À¸¥ÂÊ ´«²¨Ç®  : \t%f\n", pAvgValue[2]/ cnt);
		fprintf(fp, "¿ÞÂÊ ´«²¨Ç® : \t%f\n", pAvgValue[3]/ cnt);
		fprintf(fp, "ÀÔ ¹ú¸²  : \t%f\n", pAvgValue[4]/ cnt);

	}
	if (ANGRY)
	{
		fprintf(fp, "BrowDeclineRight_LM  : \t%f\n", pAvgValue[0]/ fearCnt);
		fprintf(fp, "BrowDeclineLeft_LM :  \t%f\n", pAvgValue[1]/ fearCnt);
		//fprintf(fp, "eyeOpenLeft_LM  : \t%f\n", arg2 / fear_count);
		//fprintf(fp, "eyeOpenRight_LM : \t%f\n", arg3 / fear_count);
		//fprintf(fp, "outerBrowRaiserRight_LM  : \t%f\n", arg4 / fear_count);
		//fprintf(fp, "outerBrowRaiserLeft_LM  : \t%f\n", arg5 / fear_count);
	}
	if (FEAR)
	{
		fprintf(fp, "InnerBrowDepressorRight_LM  : \t%f\n", pAvgValue[0]/ cnt);
		fprintf(fp, "InnerBrowDepressorLeft_LM :  \t%f\n", pAvgValue[1]/ cnt);
		fprintf(fp, "eyeOpenLeft_LM  : \t%f\n", pAvgValue[2]/ cnt);
		fprintf(fp, "eyeOpenRight_LM : \t%f\n", pAvgValue[3]/ cnt);
		//fprintf(fp, "outerBrowRaiserRight_LM  : \t%f\n", arg4 / fear_count);
		//fprintf(fp, "outerBrowRaiserLeft_LM  : \t%f\n", arg5 / fear_count);
	}
	if (DISGUST)
	{
		fprintf(fp, "BrowLower_R  : \t%f\n", pAvgValue[0]/ cnt);
		fprintf(fp, "BrowLower_L  :  \t%f\n", pAvgValue[1]/ cnt);
		fprintf(fp, "MouthOpen  : \t%f\n", pAvgValue[2]/ cnt);
		fprintf(fp, "lipCorner_R_Down : \t%f\n", pAvgValue[3]/ cnt);
		fprintf(fp, "lipCorner_L_Down  : \t%f\n", pAvgValue[4]/ cnt);
	}
	if (NEUTRAL)
	{
		fprintf(fp, "BrowLower_R  : \t%f\n", pAvgValue[0] / cnt);
		fprintf(fp, "BrowLower_L  :  \t%f\n", pAvgValue[1] / cnt);
		fprintf(fp, "MouthOpen  : \t%f\n", pAvgValue[2] / cnt);
		fprintf(fp, "lipCorner_R_Down : \t%f\n", pAvgValue[3] / cnt);
		fprintf(fp, "lipCorner_L_Down  : \t%f\n", pAvgValue[4] / cnt);
	}
	fprintf(fp, "\n");
	fclose(fp);
}

void FaceTrackingRendererManager::DisplayExpressionUsingEmoji()
{
	HWND text_emo = GetDlgItem(m_window, IDC_TEXT_EMO);
	HWND emo = GetDlgItem(m_window, IDC_EMO);
	HWND text_I1 = GetDlgItem(m_window, IDC_TEXT_EXP14);
	CString str;
	HBITMAP hBmp;
	if (HAPPY)
	{
		if (emo != NULL)
		{
			DeleteObject(emo);
		}
		hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_HAPPY), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		str.Format(_TEXT("EXPRESSION: smile"));
		SetWindowTextW(text_emo, str);
		str.Format(_TEXT("%d"), happyCnt);
		SetWindowTextW(text_I1, str);
		SendDlgItemMessage(m_window, IDC_EMO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
	}

	if (SAD)
	{
		if (emo != NULL)
		{
			DeleteObject(emo);
		}

		hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_SAD), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		str.Format(_T("EXPRESSION: sad"));
		SetWindowTextW(text_emo, str);
		str.Format(_TEXT("%d"), sadCnt);
		SetWindowTextW(text_I1, str);
		SendDlgItemMessage(m_window, IDC_EMO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
	}

	if (SURPRISE)
	{
		if (emo != NULL)
		{
			DeleteObject(emo);
		}

		hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_SURPRISE), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		str.Format(_T("EXPRESSION: surprise"));
		SetWindowTextW(text_emo, str);
		str.Format(_TEXT("%d"), surpriseCnt);
		SetWindowTextW(text_I1, str);
		SendDlgItemMessage(m_window, IDC_EMO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);

	}

	if (FEAR)
	{
		if (emo != NULL)
		{
			DeleteObject(emo);
		}

		hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_FEAR), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		str.Format(_T("EXPRESSION: fear"));
		SetWindowTextW(text_emo, str);
		str.Format(_TEXT("%d"), fearCnt);
		SetWindowTextW(text_I1, str);
		SendDlgItemMessage(m_window, IDC_EMO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
	}

	if (ANGRY)
	{
		if (emo)
		{
			DeleteObject(emo);
		}
		hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_ANGRY), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		str.Format(_T("EXPRESSION: angry"));
		SetWindowTextW(text_emo, str);
		str.Format(_TEXT("%d"), angryCnt);
		SetWindowTextW(text_I1, str);
		SendDlgItemMessage(m_window, IDC_EMO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);

	}
	if (DISGUST)
	{
		if (emo != NULL)
		{
			DeleteObject(emo);
		}
		hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_DISGUST), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		str.Format(_T("EXPRESSION: disgust"));
		SetWindowTextW(text_emo, str);
		str.Format(_TEXT("%d"), disgustCnt);
		SetWindowTextW(text_I1, str);
		SendDlgItemMessage(m_window, IDC_EMO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
	}
	if (NEUTRAL)
	{
		if (emo != NULL)
		{
			DeleteObject(emo);
		}
		hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_NEUTRAL), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		str.Format(_T("EXPRESSION: neutral"));
		SetWindowTextW(text_emo, str);
		str.Format(_TEXT("%d"), neutralCnt);
		SetWindowTextW(text_I1, str);
		SendDlgItemMessage(m_window, IDC_EMO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
	}
}

void FaceTrackingRendererManager::DisplayAverage(int emoCnt)
{

	double arg[6] = { 0, };

	HWND text_I1 = GetDlgItem(m_window, IDC_TEXT_AVG1);
	HWND text_I2 = GetDlgItem(m_window, IDC_TEXT_AVG2);
	HWND text_I3 = GetDlgItem(m_window, IDC_TEXT_AVG3);
	HWND text_I4 = GetDlgItem(m_window, IDC_TEXT_AVG4);
	HWND text_I5 = GetDlgItem(m_window, IDC_TEXT_AVG5);
	HWND text_I6 = GetDlgItem(m_window, IDC_TEXT_AVG6);
	
	CString str;

	for (int i = 0; i < 6; i++)
	{
		if (pAvgValue[i] != 0)
		{
			arg[i] = pAvgValue[i] / emoCnt;
		}
	}

	str.Format(_T("arg[0]: %f"), arg[0]);
	SetWindowTextW(text_I1, str);
	str.Format(_T("arg[1]: %f"), arg[1]);
	SetWindowTextW(text_I2, str);
	str.Format(_T("arg[2]: %f"), arg[2]);
	SetWindowTextW(text_I3, str);
	str.Format(_T("arg[3]: %f"), arg[3]);
	SetWindowTextW(text_I4, str);
	str.Format(_T("arg[4]: %f"), arg[4]);
	SetWindowTextW(text_I5, str);
	str.Format(_T("arg[5]: %f"), arg[5]);
	SetWindowTextW(text_I6, str);
}
