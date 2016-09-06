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
	void CalcAverage(double arg[], int cnt);

	static HANDLE& GetRenderingFinishedSignal();
	static void SignalProcessor();
	void SetActivateEyeCenterCalculations(bool bValue); 
	void InitValue();
	void DisplayExpressionI();
	void DisplayLandmarkI();
	int adj_frameCount;

	BOOL HAPPY, SAD, SURPRISE, FEAR, ANGRY, DISGUST;

	double pAvgValue[6];

	double happy5[5];
	double sad6[6];
	double surprise5[5];
	double fear4[4];
	double angry2[2];
	double disgust5[5];

	int happyCnt, sadCnt, surpriseCnt, fearCnt, angryCnt, disgustCnt;

private:
	FaceTrackingRenderer2D* m_renderer2D;
	FaceTrackingRenderer3D* m_renderer3D;
	FaceTrackingRenderer* m_currentRenderer;
	HWND m_window;							// m 						
	HANDLE m_rendererSignal;
	OnFinishedRenderingCallback m_callback;
	Point FacialPoint[78];

	double tOuterBrowRaiserRight;
	double tOuterBrowRaiserLeft;
	double tEyeOpenRight;
	double tEyeOpenLeft;
	double tlipCornerRightDown;
	double tlipCornerLeftDown;
	double tUpperLipRaiser;


	double tBrowLowerRight;
	double tBrowLowerLeft;


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
	//11

	double outerBrowRaiserLeft_I;
	double outerBrowDepressorLeft_I;
	double BrowLowerRight_I;
	double BrowLowerLeft_I;
	double outerBrowRaiserRight_I;
	double outerBrowDepressorRight_I;
	double upperLipRaiser_I;
	double lipCornerRightDown_I;
	double lipCornerLeftDown_I;
	double eyeOpenRight_I;
	double eyeOpenLeft_I;
	double lipCornerRightUp_I;
	double lipCornerLeftUp_I;
	/*
	1. 변수추가
	2. 
	//*/
	
};

