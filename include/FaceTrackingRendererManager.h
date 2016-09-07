#pragma once

#include "FaceTrackingRenderer.h"
#include "FaceTrackingRenderer2D.h"
#include "FaceTrackingRenderer3D.h"

class FaceTrackingRendererManager
{
	enum {
		BrowRaisedLeft,
		BrowRaisedRight,
		BrowLoweredLeft,
		BrowLoweredRight,
		Smile,
		Kiss,
		MouthOpen,
		ClosedEyeLeft,
		ClosedEyeRight,
		EyesTurnLeft,
		EyesTurnRight,
		EyesUp,
		EyesDown,
		TongueOut
	};

public:
	FaceTrackingRendererManager
	(
		FaceTrackingRenderer2D* renderer2D,
		FaceTrackingRenderer3D* renderer3D, 
		HWND window);
	~FaceTrackingRendererManager();

	void SetRendererType(FaceTrackingRenderer::RendererType type);
	void Render();
	void SetSenseManager(PXCSenseManager* senseManager);
	void SetNumberOfLandmarks(int numLandmarks);
	void SetCallback(OnFinishedRenderingCallback callback);
	void DrawBitmap(PXCCapture::Sample* sample, bool ir);
	void SetOutput(PXCFaceData* output);
	void SignalRenderer();
	void DrawDistances();
	void Reset();
	void GetExpIntensity();
	void GetLandmarkPoint();
	void PrepValue();
	void SetThresValue();
	void CvtLandmarkToIntensity();
	void SetExpressionArguments();
	void DetermineExpression();
	void PValueInit();
	void PrepAvgValue(double arg[],int length);
	void CreateTextFile(int cnt);
	void DisplayExpressionUsingEmoji();
	void DisplayAverage(int emoCnt);
	void CaptuerExpression();

	static HANDLE& GetRenderingFinishedSignal();
	static void SignalProcessor();
	void SetActivateEyeCenterCalculations(bool bValue); 
	void InitValue();
	void DisplayExpressionI();
	void DisplayLandmarkI();
	int adj_frameCount;

	BOOL HAPPY, SAD, SURPRISE, FEAR, ANGRY, DISGUST, NEUTRAL;
	BOOL EXP_HAPPY, EXP_SAD, EXP_SURPRISE, EXP_FEAR, EXP_ANGRY, EXP_DISGUST, EXP_NEUTRAL;

	double pAvgValue[6];

	int happyCnt, sadCnt, surpriseCnt, fearCnt, angryCnt, disgustCnt, neutralCnt;

private:
	FaceTrackingRenderer2D* m_renderer2D;
	FaceTrackingRenderer3D* m_renderer3D;
	FaceTrackingRenderer* m_currentRenderer;
	HWND m_window;							// m 						
	HANDLE m_rendererSignal;
	OnFinishedRenderingCallback m_callback;

	Point FacialPoint[78];

	int pOuterBrowRaiserRight;
	int pOuterBrowRaiserLeft;
	int pEyeOpenRight;
	int pEyeOpenLeft;
	int plipCornerRightDown;
	int plipCornerLeftDown;
	int pUpperLipRaiser;
	int pBrowLowerRight;
	int pBrowLowerLeft;
	//14
	int Intensity[14];

	// Threshold
	double tOuterBrowRaiserLeft;
	double tBrowLowerRight;
	double tBrowLowerLeft;
	double tOuterBrowRaiserRight;
	double tOuterBrowDepressorRight;
	double tUpperLipRaiser;
	double tlipCornerRightDown;
	double tlipCornerLeftDown;
	double tEyeOpenRight;
	double tEyeOpenLeft;
	double tLipCornerRightUp;
	//double tlipCornerLeftUp;
	double tMouthOpen;

	//12
	double outerBrowRaiserLeft_LM;
	double outerBrowDepressorLeft_LM;
	double BrowLowerRight_LM;
	double BrowLowerLeft_LM;
	double outerBrowRaiserRight_LM;
	double outerBrowDepressorRight_LM;
	double upperLipRaiser_LM;
	double lipCornerRightDown_LM;
	double lipCornerLeftDown_LM;
	double eyeOpenRight_LM;
	double eyeOpenLeft_LM;
	double lipCornerRightUp_LM;
	double lipCornerLeftUp_LM;
	double mouthOpen_LM;
	/*
	1. 변수추가
	2. 
	//*/
	
};

