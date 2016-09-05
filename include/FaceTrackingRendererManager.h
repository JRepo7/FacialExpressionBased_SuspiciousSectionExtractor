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
	void DetermineExpression();
	void CalcAvg();

	static HANDLE& GetRenderingFinishedSignal();
	static void SignalProcessor();
	void SetActivateEyeCenterCalculations(bool bValue); 
	void InitValue();

	int adj_frameCount;
	//Ç¥Á¤
	int expression[5];

private:
	FaceTrackingRenderer2D* m_renderer2D;
	FaceTrackingRenderer3D* m_renderer3D;
	FaceTrackingRenderer* m_currentRenderer;
	HWND m_window;									// m
	HANDLE m_rendererSignal;
	OnFinishedRenderingCallback m_callback;
	Point FacialPoint[78];
	double thres_OuterBrowRaiserRight;
	double thres_OuterBrowRaiserLeft;
	double thres_EyeOpenRight;
	double thres_EyeOpenLeft;
	double thres_lipCornerRightDown;
	double thres_lipCornerLeftDown;
	double thres_UpperLipRaiser;
	int prep_OuterBrowRaiserRight;
	int prep_OuterBrowRaiserLeft;
	int prep_EyeOpenRight;
	int prep_EyeOpenLeft;
	int prep_lipCornerRightDown;
	int prep_lipCornerLeftDown;
	int prep_UpperLipRaiser;
	
	
	//14
	int Intensity[14];
	//11
	double outerBrowRaiserLeft_I;
	double outerBrowDepressorLeft_I;
	double outerBrowRaiserRight_I;
	double outerBrowDepressorRight_I;
	double upperLipRaiser_I;
	double lipCornerRightUp_I;
	double lipCornerLeftUp_I;
	double lipCornerRightDown_I;
	double lipCornerLeftDown_I;
	double eyeOpenRight_I;
	double eyeOpenLeft_I;

	double max1;
	double max2;
	double max3;
	double max4;
	double max5;
	double max6;
	double max7;
	double max8;
	double max9;
	double max10;
	double max11;

	int happy_count;
	int sad_count;
	int surprise_count;
	int fear_count;
	int angry_count;
	int disgust_count;	

public:
	double arg;
	double arg1;
	double arg2;
	double arg3;
	double arg4;
	double arg5;

};

