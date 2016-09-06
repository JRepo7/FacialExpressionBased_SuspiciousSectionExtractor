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
	happy_count = sad_count = surprise_count = fear_count = angry_count = disgust_count = 0;
	arg=arg1 = arg2 = arg3 = arg4 = arg5 =0;
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

	prep_OuterBrowRaiserRight = prep_OuterBrowRaiserLeft = prep_EyeOpenRight = prep_EyeOpenLeft = prep_lipCornerRightDown 
		= prep_lipCornerLeftDown= prep_UpperLipRaiser= prep_BrowLowerRight = prep_BrowLowerLeft=0;

	outerBrowRaiserLeft_I = outerBrowDepressorLeft_I = 0;
	outerBrowRaiserRight_I = outerBrowDepressorRight_I = 0;
	
	upperLipRaiser_I = 0;
	
	lipCornerRightUp_I = lipCornerRightDown_I = 0;
	lipCornerLeftUp_I = lipCornerLeftDown_I= 0;

	eyeOpenRight_I = 0;
	eyeOpenLeft_I = 0;

	BrowLowerRight_I = 0;
	BrowLowerLeft_I = 0;


	happy_count = sad_count = surprise_count = fear_count = angry_count = disgust_count = 0;
	arg = arg1 = arg2 = arg3 = arg4 = 0;
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

	prep_BrowLowerRight += (FacialPoint[12].y - FacialPoint[71].y);
	prep_BrowLowerLeft += (FacialPoint[20].y - FacialPoint[74].y);

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

	thres_BrowLowerRight = (double)prep_BrowLowerRight /adj_frameCount;
	thres_BrowLowerLeft = (double)prep_BrowLowerLeft /adj_frameCount;

}

void FaceTrackingRendererManager::CvtLandmarkToIntensity()
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
	double ratio;

	ratio = ((FacialPoint[14].y - FacialPoint[3].y) / thres_OuterBrowRaiserRight);
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
	
	ratio = ((FacialPoint[22].y - FacialPoint[8].y) / thres_OuterBrowRaiserLeft);
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
	ratio = ((FacialPoint[36].y - FacialPoint[26].y) / thres_UpperLipRaiser);
	if (ratio < 1)
	{
		upperLipRaiser_I = 100 - (ratio * 100);

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
	}
	else
	{
		eyeOpenLeft_I = 0;
	}

	ratio = ((FacialPoint[16].y - FacialPoint[12].y) / thres_EyeOpenRight);
	if (ratio > 1)
	{
		eyeOpenRight_I = (ratio * 100)-100;


	}
	else
	{
		eyeOpenRight_I = 0;
	}

	ratio = ((FacialPoint[12].y - FacialPoint[70].y)/ thres_BrowLowerRight);
	if (ratio > 1)
	{
		BrowLowerRight_I = 0;
	}
	else
	{
		BrowLowerRight_I = 100-(ratio * 100);
	}

	ratio = ((FacialPoint[20].y - FacialPoint[74].y)/ thres_BrowLowerLeft);
	if (ratio > 1)
	{
		BrowLowerLeft_I = 0;
	}
	else
	{
		BrowLowerLeft_I = 100-(ratio * 100);
	}

	//ratio = ((FacialPoint[70].y - FacialPoint[3].y) / thres_BrowDeclineRight);
	//if (ratio > 1)
	//{
	//	BrowDeclineRight_I = (ratio * 100)-100;
	//}
	//else
	//{
	//	BrowDeclineRight_I = 0;
	//}

	//ratio = ((FacialPoint[73].y - FacialPoint[8].y) / thres_BrowDeclineLeft);
	//if (ratio > 1)
	//{
	//	BrowDeclineLeft_I = (ratio * 100) - 100;
	//}
	//else
	//{
	//	BrowDeclineLeft_I = 0;
	//}

	str.Format(_T("outerBrowRaiserLeft_I sad:\t\t  %f"), outerBrowRaiserLeft_I);
	SetWindowTextW(text_I1, str);
	str.Format(_T("outerBrowRaiserRight_I sad:\t  %f"), outerBrowRaiserRight_I);
	SetWindowTextW(text_I2, str);

	str.Format(_T("BrowLowerRight_I disgust:\t  %f"), BrowLowerRight_I);
	SetWindowTextW(text_I3, str);
	str.Format(_T("BrowLowerLeft_I disgust:\t  %f"), BrowLowerLeft_I);
	SetWindowTextW(text_I4, str);

	str.Format(_T("upperLipRaiser_I disgust:\t\t  %f"), upperLipRaiser_I);
	SetWindowTextW(text_I5, str);

	str.Format(_T("lipCornerRightDown_I disgust:\t\t  %f"), lipCornerRightDown_I);
	SetWindowTextW(text_I6, str);
	str.Format(_T("lipCornerLeftDown_I disgust:\t\t  %f"), lipCornerLeftDown_I);
	SetWindowTextW(text_I7, str);

	str.Format(_T("eyeOpenRight_I max8:\t\t   %f"), eyeOpenRight_I);
	SetWindowTextW(text_I8, str);
	str.Format(_T("eyeOpenLeft_I max9:\t\t   %f"), eyeOpenLeft_I);
	SetWindowTextW(text_I9, str);

	str.Format(_T("lipCornerRightUp_I max6:\t\t  %f"), lipCornerRightUp_I);
	SetWindowTextW(text_I10, str);
	str.Format(_T("lipCornerLeftUp_I max7:\t\t  %f"), lipCornerLeftUp_I);
	SetWindowTextW(text_I11, str);



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

void FaceTrackingRendererManager::DetermineExpression()
{
	HWND text_emo = GetDlgItem(m_window, IDC_TEXT_EMO);
	HWND emo = GetDlgItem(m_window, IDC_EMO);
	HWND text_I1 = GetDlgItem(m_window, IDC_TEXT_EXP14);

	CString str;
	HBITMAP hBmp;
	/*
	//smile
	if (outerBrowRaiserRight_I >1 && outerBrowRaiserLeft_I >1 &&  
		(Intensity[Smile] > 10 || (lipCornerLeftUp_I>5 && lipCornerRightUp_I>5 ))) 
	{
		happy_count++;
		arg  += outerBrowRaiserRight_I;
		arg1 += outerBrowRaiserLeft_I;
		arg2 += Intensity[Smile];
		arg3 += lipCornerLeftUp_I;
		arg4 += lipCornerRightUp_I;
		if (emo != NULL) 
		{ 
			DeleteObject(emo);
		}
		hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_HAPPY), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		str.Format(_TEXT("EXPRESSION: smile"));
		SetWindowTextW(text_emo, str );
		str.Format(_TEXT("%d"), happy_count);
		SetWindowTextW(text_I1, str);
		SendDlgItemMessage(m_window, IDC_EMO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
	}
	if (happy_count == 2000)
	{
		FILE *fp = fopen("data.txt", "a");
		fprintf(fp, "¿À¸¥ÂÊ ¹Ù±ù´«½ç  : \t%f\n", arg / happy_count);
		fprintf(fp, "¿ÞÂÊ ¹Ù±ù´«½ç :  \t%f\n", arg1 / happy_count);
		fprintf(fp, "¿ôÀ½ Á¤µµ :\t%f\n", arg2 / happy_count);
		fprintf(fp, "¿ÞÂÊ ÀÔ¼úÄÚ³Ê   : \t%f\n", arg3 / happy_count);
		fprintf(fp, "¿À¸¥ÂÊ ÀÔ¼úÄÚ³Ê  : \t%f\n", arg4 / happy_count);

		happy_count = sad_count = surprise_count = fear_count = angry_count = disgust_count = 0;
		arg = arg1 = arg2 = arg3 = arg4 = 0;

		fclose(fp);
	}
	//*/

	//sad
	/*
	if (( (outerBrowDepressorLeft_I + outerBrowDepressorRight_I >1) ||(Intensity[BrowLoweredLeft]>10 && Intensity[BrowLoweredRight] > 10) )&& 
		(lipCornerRightDown_I + lipCornerLeftDown_I > 5))
	{
		sad_count++;
		arg += Intensity[BrowLoweredLeft];
		arg1 += Intensity[BrowLoweredRight];
		arg2 += lipCornerRightDown_I;
		arg3 += lipCornerLeftDown_I;
		arg4 += outerBrowDepressorRight_I;
		arg5 += outerBrowDepressorLeft_I;

		if (emo != NULL) 
		{
			DeleteObject(emo);
		}

		hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_SAD), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		str.Format(_T("EXPRESSION: sad"));
		SetWindowTextW(text_emo, str);
		str.Format(_TEXT("%d"), sad_count);
		SetWindowTextW(text_I1, str);
		SendDlgItemMessage(m_window, IDC_EMO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);

		if (sad_count == 1000)
		{
		FILE *fp = fopen("data.txt", "a");
		fprintf(fp, "¿À¸¥ÂÊ ´«½ç  : \t%f\n", arg / sad_count);
		fprintf(fp, "¿ÞÂÊ ´«½ç :  \t%f\n", arg1 / sad_count);
		fprintf(fp, "¿ÞÂÊ ÀÔ¼úÄÚ³Ê   : \t%f\n", arg2 / sad_count);
		fprintf(fp, "¿À¸¥ÂÊ ÀÔ¼úÄÚ³Ê  : \t%f\n", arg3 / sad_count);
		fprintf(fp, "¿À¸¥ÂÊ ¹Ù±ù´«½ç  : \t%f\n", arg4 / sad_count);
		fprintf(fp, "¿ÞÂÊ ¹Ù±ù´«½ç  : \t%f\n\n", arg5 / sad_count);

		happy_count = sad_count = surprise_count = fear_count = angry_count = disgust_count = 0;
		arg = arg1 = arg2 = arg3 = arg4 = 0;

		fclose(fp);
	}
	}
	//*/
	
	//surprise
	/*
	if (outerBrowRaiserLeft_I > 1 && outerBrowRaiserRight_I > 1 && eyeOpenRight_I > 10 && eyeOpenLeft_I > 10 &&
		Intensity[MouthOpen] > 0)
	{
		surprise_count++;;
		arg += outerBrowRaiserLeft_I;
		arg1 += outerBrowRaiserRight_I;
		arg2 += eyeOpenRight_I;
		arg3 += eyeOpenLeft_I;
		arg4 += Intensity[MouthOpen];

		if (emo != NULL) 
		{
			DeleteObject(emo);
		}

		hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_SURPRISE), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		str.Format(_T("EXPRESSION: surprise"));
		SetWindowTextW(text_emo, str);
		str.Format(_TEXT("%d"), surprise_count);
		SetWindowTextW(text_I1, str);
		SendDlgItemMessage(m_window, IDC_EMO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);

		if (surprise_count == 1000)
		{
			FILE *fp = fopen("data.txt", "a");
			fprintf(fp, "¿À¸¥ÂÊ ¹Ù±ù´«½ç  : \t%f\n", arg / surprise_count);
			fprintf(fp, "¿ÞÂÊ ¹Ù±ù´«½ç :  \t%f\n", arg1 / surprise_count);
			fprintf(fp, "¿À¸¥ÂÊ ´«²¨Ç®  : \t%f\n", arg2 / surprise_count);
			fprintf(fp, "¿ÞÂÊ ´«²¨Ç® : \t%f\n", arg3 / surprise_count);
			fprintf(fp, "ÀÔ ¹ú¸²  : \t%f\n", arg4 / surprise_count);

			happy_count = sad_count = surprise_count = fear_count = angry_count = disgust_count = 0;
			arg = arg1 = arg2 = arg3 = arg4 = 0;

			fclose(fp);
		}
	}
	//*/

	//fear&angry
	/*
	if (
		(BrowLowerRight_I>10 && BrowLowerLeft_I>10) &&
		(eyeOpenLeft_I>1 && eyeOpenRight_I>1))
		//outerBrowRaiserRight_I >1 && outerBrowRaiserLeft_I>1)
	{
		fear_count++;
		arg += BrowLowerRight_I;
		arg1 += BrowLowerLeft_I;
		arg2 += eyeOpenRight_I;
		arg3 += eyeOpenLeft_I;
		//arg4 += outerBrowRaiserRight_I;
		//arg5 += outerBrowRaiserLeft_I;

		if (emo != NULL)
		{
			DeleteObject(emo);
		}

		hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_FEAR), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		str.Format(_T("EXPRESSION: fear"));
		SetWindowTextW(text_emo, str);
		str.Format(_TEXT("%d"), fear_count);
		SetWindowTextW(text_I1, str);
		SendDlgItemMessage(m_window, IDC_EMO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);

		if (fear_count == 1000)
		{
			FILE *fp = fopen("data.txt", "a");
			fprintf(fp, "InnerBrowDepressorRight_I  : \t%f\n", arg / fear_count);
			fprintf(fp, "InnerBrowDepressorLeft_I :  \t%f\n", arg1 / fear_count);
			fprintf(fp, "eyeOpenLeft_I  : \t%f\n", arg2 / fear_count);
			fprintf(fp, "eyeOpenRight_I : \t%f\n", arg3 / fear_count);
			//fprintf(fp, "outerBrowRaiserRight_I  : \t%f\n", arg4 / fear_count);
			//fprintf(fp, "outerBrowRaiserLeft_I  : \t%f\n", arg5 / fear_count);

			happy_count = sad_count = surprise_count = fear_count = angry_count = disgust_count = 0;
			arg = arg1 = arg2 = arg3 = arg4 = 0;

			fclose(fp);
		}
	}
	//*/

	
	//angry
	/*
	if ((BrowDeclineRight_I + BrowDeclineLeft_I)>1)
		//Intensity[BrowLoweredLeft]>1 && Intensity[BrowLoweredRight]>10 && 
		//outerBrowRaiserLeft_I>10 && outerBrowRaiserRight_I&&
		//eyeOpenLeft_I>10&&eyeOpenRight_I>10)
	{
		angry_count++;
		arg += BrowDeclineRight_I;
		arg1 += BrowDeclineLeft_I;
		//arg2 += eyeOpenRight_I;
		//arg3 += eyeOpenLeft_I;
		//arg4 += outerBrowRaiserRight_I;
		//arg5 += outerBrowRaiserLeft_I;

		hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_ANGRY), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		str.Format(_T("EXPRESSION: angry"));
		SetWindowTextW(text_emo, str);
		str.Format(_TEXT("%d"), angry_count);
		SetWindowTextW(text_I1, str);
		SendDlgItemMessage(m_window, IDC_EMO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);

		if (angry_count == 1000)
		{
			FILE *fp = fopen("data.txt", "a");
			fprintf(fp, "BrowDeclineRight_I  : \t%f\n", arg / fear_count);
			fprintf(fp, "BrowDeclineLeft_I :  \t%f\n\n", arg1 / fear_count);
			//fprintf(fp, "eyeOpenLeft_I  : \t%f\n", arg2 / fear_count);
			//fprintf(fp, "eyeOpenRight_I : \t%f\n", arg3 / fear_count);
			//fprintf(fp, "outerBrowRaiserRight_I  : \t%f\n", arg4 / fear_count);
			//fprintf(fp, "outerBrowRaiserLeft_I  : \t%f\n", arg5 / fear_count);

			happy_count = sad_count = surprise_count = fear_count = angry_count = disgust_count = 0;
			arg = arg1 = arg2 = arg3 = arg4 = 0;

			fclose(fp);
		}
	}
	//*/

	//disgust
	if ((BrowLowerRight_I>10 && BrowLowerLeft_I>10) &&
		upperLipRaiser_I>1 &&
		(lipCornerRightDown_I + lipCornerLeftDown_I>5))
	{
		disgust_count++;
		arg += BrowLowerRight_I;
		arg1 += BrowLowerLeft_I;
		arg2 += upperLipRaiser_I;
		arg3 += lipCornerRightDown_I;
		arg4 += lipCornerLeftDown_I;
		//arg5 += outerBrowRaiserLeft_I;

		if (emo != NULL)
		{
			DeleteObject(emo);
		}

		hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_DISGUST), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		str.Format(_T("EXPRESSION: disgust"));
		SetWindowTextW(text_emo, str);
		str.Format(_TEXT("%d"), disgust_count);
		SetWindowTextW(text_I1, str);
		SendDlgItemMessage(m_window, IDC_EMO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);

		if (disgust_count == 1000)
		{
			FILE *fp = fopen("data.txt", "a");
			fprintf(fp, "BrowLowerRight_I  : \t%f\n", arg / disgust_count);
			fprintf(fp, "BrowLowerLeft_I :  \t%f\n", arg1 / disgust_count);
			fprintf(fp, "upperLipRaiser_I  : \t%f\n", arg2 / disgust_count);
			fprintf(fp, "lipCornerRightDown_I : \t%f\n", arg3 / disgust_count);
			fprintf(fp, "lipCornerLeftDown_I  : \t%f\n\n", arg4 / disgust_count);
			//fprintf(fp, "outerBrowRaiserLeft_I  : \t%f\n", arg5 / fear_count);

			happy_count = sad_count = surprise_count = fear_count = angry_count = disgust_count = 0;
			arg = arg1 = arg2 = arg3 = arg4 = 0;

			fclose(fp);
		}
	}
	//*/
}


void FaceTrackingRendererManager::CalcAvg()
{
	
	double avg[6] = { 0, };
	//outerBrowRaiserRight_A, outerBrowRaiserLeft_A, smile_A, lipCornerLeftUp_A, lipCornerRightUp_A=0;

	HWND text_I1 = GetDlgItem(m_window, IDC_TEXT_EXP13);
	HWND text_I2 = GetDlgItem(m_window, IDC_TEXT_EXP14);
	HWND text_I3 = GetDlgItem(m_window, IDC_TEXT_EXP15);
	HWND text_I4 = GetDlgItem(m_window, IDC_TEXT_EXP16);
	HWND text_I5 = GetDlgItem(m_window, IDC_TEXT_EXP17);
	HWND text_I6 = GetDlgItem(m_window, IDC_TEXT_EXP18);

	CString str;
	// outerBrowRaiserRight_I_sum , outerBrowRaiserLeft_I_sum, smile_sum, lipCornerLeftUp_I_sum, lipCornerRightUp_I_sum
	if(avg[0]!=0)
	{
	avg[0] = arg / happy_count;
	}
	if (avg[1] != 0)
	{
		avg[1] = arg1 / happy_count;
	}
	if (avg[2] != 0)
	{
		avg[2] = arg2 / happy_count;
	}
	if (avg[3] != 0)
	{
		avg[3] = arg3 / happy_count;
	}
	if (avg[4] != 0)
	{
		avg[4] = arg4 / happy_count;
	}
	if (avg[5] != 0)
	{
		avg[5] = arg5 / happy_count;
	}

	str.Format(_T("avg[0]: %f"), avg[0]);
	SetWindowTextW(text_I1, str);
	str.Format(_T("avg[1]: %f"), avg[1]);
	SetWindowTextW(text_I2, str);
	str.Format(_T("avg[2]: %f"), avg[2]);
	SetWindowTextW(text_I3, str);
	str.Format(_T("avg[3]: %f"), avg[3]);
	SetWindowTextW(text_I4, str);
	str.Format(_T("avg[4]: %f"), avg[4]);
	SetWindowTextW(text_I5, str);
	str.Format(_T("avg[5]: %f"), avg[5]);
	SetWindowTextW(text_I6, str);

	happy_count = sad_count = surprise_count = fear_count = angry_count = disgust_count = 0;
	avg[0] = avg[1] = avg[2] = avg[3] = avg[4] = avg[5]= 0;
}
//*/