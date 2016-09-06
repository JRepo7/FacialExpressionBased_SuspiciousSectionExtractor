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
	happyCnt = sadCnt = surpriseCnt = fearCnt = angryCnt = disgustCnt = 0;
	PValueInit();
	HAPPY = SAD = SURPRISE = FEAR = ANGRY = DISGUST= FALSE;
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
	outerBrowRaiserLeft_I = outerBrowDepressorLeft_I = outerBrowRaiserRight_I = outerBrowDepressorRight_I = 0;
	upperLipRaiser_I = 0;
	lipCornerRightUp_I = lipCornerRightDown_I = lipCornerLeftUp_I = lipCornerLeftDown_I= 0;
	eyeOpenRight_I =eyeOpenLeft_I = 0;
	BrowLowerRight_I =BrowLowerLeft_I = 0;

	happyCnt = sadCnt = surpriseCnt = fearCnt = angryCnt = disgustCnt = 0; 
	PValueInit();
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

	CString str;

	str.Format(_T("outerBrowRaiserLeft_I :\t\t  %f"), outerBrowRaiserLeft_I);
	SetWindowTextW(text_I1, str);
	str.Format(_T("outerBrowRaiserRight_I :\t\t  %f"), outerBrowRaiserRight_I);
	SetWindowTextW(text_I2, str);

	str.Format(_T("BrowLowerRight_I :\t\t  %f"), BrowLowerRight_I);
	SetWindowTextW(text_I3, str);
	str.Format(_T("BrowLowerLeft_I :\t\t  %f"), BrowLowerLeft_I);
	SetWindowTextW(text_I4, str);

	str.Format(_T("upperLipRaiser_I:\t\t  %f"), upperLipRaiser_I);
	SetWindowTextW(text_I5, str);

	str.Format(_T("lipCornerRightDown_I:\t  %f"), lipCornerRightDown_I);
	SetWindowTextW(text_I6, str);
	str.Format(_T("lipCornerLeftDown_I:\t\t  %f"), lipCornerLeftDown_I);
	SetWindowTextW(text_I7, str);

	str.Format(_T("eyeOpenRight_I:\t\t   %f"), eyeOpenRight_I);
	SetWindowTextW(text_I8, str);
	str.Format(_T("eyeOpenLeft_I:\t\t   %f"), eyeOpenLeft_I);
	SetWindowTextW(text_I9, str);

	str.Format(_T("lipCornerRightUp_I:\t\t  %f"), lipCornerRightUp_I);
	SetWindowTextW(text_I10, str);
	str.Format(_T("lipCornerLeftUp_I:\t\t  %f"), lipCornerLeftUp_I);
	SetWindowTextW(text_I11, str);
}
void FaceTrackingRendererManager::CvtLandmarkToIntensity()
{
	double ratio;

	ratio = (FacialPoint[14].y - FacialPoint[3].y) / tOuterBrowRaiserRight;
	if (ratio > 1)
	{
		outerBrowRaiserRight_I = (ratio * 100) - 100;
		outerBrowDepressorRight_I = 0;//¹Ý´ë Á¶°ÇÀÇ º¯¼öÀÎ outerBrowDepressorRight_I°¡ ÀÌÀü °ªÀ» °®´Â°æ¿ì°¡ ÀÖÀ¸¹Ç·Î 0À¸·Î setÇØÁÜ
	}
	else
	{
		outerBrowDepressorRight_I = 100 - (ratio * 100);
		outerBrowRaiserRight_I = 0;
	}
	
	ratio = ((FacialPoint[22].y - FacialPoint[8].y) / tOuterBrowRaiserLeft);
	if (ratio > 1)
	{
		outerBrowRaiserLeft_I = (ratio * 100) - 100;
		outerBrowDepressorLeft_I = 0;
	}
	else
	{
		outerBrowDepressorLeft_I = 100 - (ratio * 100);
		outerBrowRaiserLeft_I = 0;
	}

	//fp36(À­ÀÔ¼ú)ÀÇ À§Ä¡°¡ ¿Ã¶ó°¡¸é ±âÁØÁ¡ fp31(ÀÎÁß)°ú fp36(À­ÀÔ¼ú) °£ÀÇ ³ôÀÌÂ÷´Â ÁÙ¾îµë Áï if(ratio < 1) °¡ ÀûÀý
	//**Çø¿À°¨À» ³ªÅ¸³»´Âµ¥ ±»ÀÌ ÇÊ¿äÇÑ°¡
	ratio = ((FacialPoint[36].y - FacialPoint[26].y) / tUpperLipRaiser);
	if (ratio < 1)
	{
		upperLipRaiser_I = 100 - (ratio * 100);

	}
	else
	{
		upperLipRaiser_I = 0;
	}

	ratio = ((FacialPoint[33].y - FacialPoint[30].y) / tlipCornerRightDown);
	if (ratio > 1)
	{
		lipCornerRightDown_I = (ratio * 100)-100;
		lipCornerRightUp_I = 0;
	}
	else
	{
		lipCornerRightUp_I = 100 - (ratio * 100);
		lipCornerRightDown_I = 0;
	}

	ratio = ((FacialPoint[39].y - FacialPoint[32].y) / tlipCornerLeftDown);
	if (ratio > 1)
	{
		lipCornerLeftDown_I = (ratio * 100)-100;
		lipCornerLeftUp_I = 0;
	}
	else
	{
		lipCornerLeftUp_I = 100 - (ratio * 100);
		lipCornerLeftDown_I = 0;
	}

	ratio = ((FacialPoint[24].y - FacialPoint[20].y) / tEyeOpenLeft);
	if (ratio >1)
	{
		eyeOpenLeft_I = (ratio * 100)-100;
	}
	else
	{
		eyeOpenLeft_I = 0;
	}

	ratio = ((FacialPoint[16].y - FacialPoint[12].y) / tEyeOpenRight);
	if (ratio > 1)
	{
		eyeOpenRight_I = (ratio * 100)-100;
	}
	else
	{
		eyeOpenRight_I = 0;
	}

	ratio = ((FacialPoint[12].y - FacialPoint[70].y)/ tBrowLowerRight);

	if (ratio > 1)
	{
		BrowLowerRight_I = 0;
	}
	else
	{
		BrowLowerRight_I = 100-(ratio * 100);
	}

	ratio = ((FacialPoint[20].y - FacialPoint[74].y)/ tBrowLowerLeft);
	if (ratio > 1)
	{
		BrowLowerLeft_I = 0;
	}
	else
	{
		BrowLowerLeft_I = 100-(ratio * 100);
	}
	DisplayExpressionI();
	DisplayLandmarkI();
}
void FaceTrackingRendererManager::SetExpressionArguments()
{
	happy5[0] = { outerBrowRaiserRight_I };
	happy5[1] = { outerBrowRaiserLeft_I };
	happy5[2] = { (double)Intensity[Smile] };
	happy5[3] = { lipCornerLeftUp_I };
	happy5[4] = { lipCornerRightUp_I };

	sad6[0] = { (double)Intensity[BrowLoweredLeft] };
	sad6[1] = { (double)Intensity[BrowLoweredRight] };
	sad6[2] = { lipCornerRightDown_I };
	sad6[3] = { lipCornerLeftDown_I };
	sad6[4] = { outerBrowDepressorRight_I };
	sad6[5] = { outerBrowDepressorLeft_I };

	surprise5[0] = { outerBrowRaiserLeft_I };
	surprise5[1] = { outerBrowRaiserRight_I };
	surprise5[2] = { eyeOpenRight_I };
	surprise5[3] = { eyeOpenLeft_I };
	surprise5[4] = { (double)Intensity[MouthOpen] };

	fear4[0] = { BrowLowerRight_I };
	fear4[1] = { BrowLowerLeft_I };
	fear4[2] = { upperLipRaiser_I };
	fear4[3] = { lipCornerRightDown_I };
	//arg4 += outerBrowRaiserRight_I;
	//arg5 += outerBrowRaiserLeft_I;

	angry2[0] = { BrowLowerLeft_I };
	angry2[1] = { BrowLowerRight_I };
	//eyeOpenRight_I,
	//outerBrowRaiserRight_I,
	//outerBrowRaiserLeft_I

	disgust5[0] = { BrowLowerRight_I };
	disgust5[1] = { BrowLowerLeft_I };
	disgust5[2] = { upperLipRaiser_I };
	disgust5[3] = { lipCornerRightDown_I };
	disgust5[4] = { lipCornerLeftDown_I };
}
void FaceTrackingRendererManager::DetermineExpression()
{
	
	if (outerBrowRaiserRight_I > 1 && outerBrowRaiserLeft_I > 1 &&
		(Intensity[Smile] > 10 || (lipCornerLeftUp_I > 5 && lipCornerRightUp_I > 5)))
	{
		HAPPY = TRUE;
		happyCnt++;

		PrepAvgValue(happy5, sizeof(happy5) / sizeof(happy5[0]));

		DisplayExpressionUsingEmoji();

		if (happyCnt == 2000)
		{
			CreateTextFile(happyCnt);
			happyCnt = 0;
			PValueInit();
		}
		HAPPY = FALSE;
	}
	/*
	if (( (outerBrowDepressorLeft_I + outerBrowDepressorRight_I >1) ||(Intensity[BrowLoweredLeft]>10 && Intensity[BrowLoweredRight] > 10) )&& 
		(lipCornerRightDown_I + lipCornerLeftDown_I > 5))
	{
		SAD = TRUE;
		sadCnt++;
		
		PrepAvgValue(sad6, sizeof(sad6) / sizeof(sad6[0]));

		DisplayExpressionUsingEmoji();

		if (sadCnt == 1000)
		{
			CreateTextFile(sadCnt);
			sadCnt=0;
			PValueInit();
		}

		SAD = FALSE;
	}

	if (outerBrowRaiserLeft_I > 1 && outerBrowRaiserRight_I > 1 && eyeOpenRight_I > 10 && eyeOpenLeft_I > 10 &&
		Intensity[MouthOpen] > 0)
	{
		SURPRISE = TRUE;
		surpriseCnt++;

		PrepAvgValue(surprise5, sizeof(surprise5) / sizeof(surprise5[0]));

		DisplayExpressionUsingEmoji();

		if (surpriseCnt == 1000)
		{
			CreateTextFile(surpriseCnt);

			surpriseCnt= 0;
			PValueInit();
		}
		SURPRISE = FALSE;
	}

	if (
		(BrowLowerRight_I>10 && BrowLowerLeft_I>10) &&
		(eyeOpenLeft_I>1 && eyeOpenRight_I>1))
		//outerBrowRaiserRight_I >1 && outerBrowRaiserLeft_I>1)
	{
		FEAR = TRUE;
		fearCnt++;

		PrepAvgValue(fear4, sizeof(fear4) / sizeof(fear4[0]));

		if (fearCnt == 1000)
		{
			CreateTextFile(fearCnt);

			fearCnt= 0;
			PrepValue();
		}
		FEAR = FALSE;
	}
	
	if ((BrowLowerLeft_I + BrowLowerRight_I)>1)
		//Intensity[BrowLoweredLeft]>1 && Intensity[BrowLoweredRight]>10 && 
		//outerBrowRaiserLeft_I>10 && outerBrowRaiserRight_I&&
		//eyeOpenLeft_I>10&&eyeOpenRight_I>10)
	{
		ANGRY = TRUE;
		angryCnt++;

		PrepAvgValue(angry2, sizeof(angry2) / sizeof(angry2[0]));

		DisplayExpressionUsingEmoji();

		if (angryCnt == 1000)
		{
			CreateTextFile(angryCnt);
			angryCnt= 0;
			PValueInit();
		}
	}
	

	if ((BrowLowerRight_I>10 && BrowLowerLeft_I>10) &&
		upperLipRaiser_I>1 &&
		(lipCornerRightDown_I + lipCornerLeftDown_I>5))
	{
		DISGUST = TRUE;
		disgustCnt++;

		PrepAvgValue(disgust5, sizeof(disgust5) / sizeof(disgust5[0]));

		DisplayExpressionUsingEmoji();

		if (disgustCnt == 1000)
		{
			CreateTextFile(disgustCnt);

			disgustCnt = 0;
			PValueInit();
		}
		DISGUST = FALSE;
	}
	//*/
}
void FaceTrackingRendererManager::PValueInit()
{
	for (int i = 0; i < 6; i++)
	{
		pAvgValue[i] = 0;
	}
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
		fprintf(fp, "BrowDeclineRight_I  : \t%f\n", pAvgValue[0]/ fearCnt);
		fprintf(fp, "BrowDeclineLeft_I :  \t%f\n", pAvgValue[1]/ fearCnt);
		//fprintf(fp, "eyeOpenLeft_I  : \t%f\n", arg2 / fear_count);
		//fprintf(fp, "eyeOpenRight_I : \t%f\n", arg3 / fear_count);
		//fprintf(fp, "outerBrowRaiserRight_I  : \t%f\n", arg4 / fear_count);
		//fprintf(fp, "outerBrowRaiserLeft_I  : \t%f\n", arg5 / fear_count);
	}
	if (FEAR)
	{
		fprintf(fp, "InnerBrowDepressorRight_I  : \t%f\n", pAvgValue[0]/ cnt);
		fprintf(fp, "InnerBrowDepressorLeft_I :  \t%f\n", pAvgValue[1]/ cnt);
		fprintf(fp, "eyeOpenLeft_I  : \t%f\n", pAvgValue[2]/ cnt);
		fprintf(fp, "eyeOpenRight_I : \t%f\n", pAvgValue[3]/ cnt);
		//fprintf(fp, "outerBrowRaiserRight_I  : \t%f\n", arg4 / fear_count);
		//fprintf(fp, "outerBrowRaiserLeft_I  : \t%f\n", arg5 / fear_count);
	}
	if (DISGUST)
	{
		fprintf(fp, "BrowLower_R  : \t%f\n", pAvgValue[0]/ cnt);
		fprintf(fp, "BrowLower_L  :  \t%f\n", pAvgValue[1]/ cnt);
		fprintf(fp, "upperLip_Raiser  : \t%f\n", pAvgValue[2]/ cnt);
		fprintf(fp, "lipCorner_R_Down : \t%f\n", pAvgValue[3]/ cnt);
		fprintf(fp, "lipCorner_L_Down  : \t%f\n", pAvgValue[4]/ cnt);
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
		if (arg[i] != 0)
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
