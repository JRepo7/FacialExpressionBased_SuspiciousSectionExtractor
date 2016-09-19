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

	void InitValue();
	void InitStop();
	void SetRendererType(FaceTrackingRenderer::RendererType type);
	void Render();
	void SetSenseManager(PXCSenseManager* senseManager);
	void SetNumberOfLandmarks(int numLandmarks);
	void SetCallback(OnFinishedRenderingCallback callback);
	void DrawBitmap(PXCCapture::Sample* sample, bool ir);
	void DrawBitmap2(PXCCapture::Sample* sample, bool ir);
	void SetOutput(PXCFaceData* output);
	void SignalRenderer();
	void DrawDistances();
	void Reset();
	void GetExpIntensity();
	void GetLandmarkPoint();
	void GetHeadandPulse();

	void PrepValue();
	void SetThresValue();
	void CvtLandmarkToIntensity();
	void DisplayExpressionUsingEmoji(BOOL EXP_EMO[]);
	void CaptureSubtleExpression();
	
	void GetFreqBasedOnEmo();
	
	void ContinueExpression(int win);
	void MicroExpression(int win);

	double GetDuration(int frame);
	int GetFrameSize(double second);
	void DetermineExpression();
	void CircularQueue1800();
	void CircularQueue300();

	void Recording();
	int IsChanged_r();
	int IsChanged_f();

	void SubFunc();
	void ShowHeartRate();

	void DetermineFlagOutOfHeadPos();
	void RecordingOutOfRange();
	BOOL IsChangedRange_r();
	BOOL IsChangedRange_f();
	void ShowHeadMovementRecord();
	void FlagOnOff();

	static HANDLE& GetRenderingFinishedSignal();
	static void SignalProcessor();
	void SetActivateEyeCenterCalculations(bool bValue); 
	int adj_frameCount;

	BOOL HAPPY, SAD, SURPRISE, FEAR, ANGRY, DISGUST;		//for Neutral
	BOOL EXP_EMO[7];
	//BOOL EXP_HAPPY, EXP_SAD, EXP_SURPRISE, EXP_FEAR, EXP_ANGRY, EXP_DISGUST, EXP_NEUTRAL;

	int happyCnt, sadCnt, surpriseCnt, fearCnt, angryCnt, disgustCnt, neutralCnt;

	BOOL slidingWindow[1800] = { TRUE, };
	BOOL ws_smile[1800] = { FALSE, };
	BOOL slidingWindow_d[180] = { 0, };
	BOOL ws_subtleSmile[30] = {FALSE,};
	BOOL slidingWindow_Range[900] = { FALSE, };
	int slidingWindow_M[10] = { 0, }; // 0.2frame * 10 = 2s ... 
	int amountOfHappy, amountOfSad, amountOfSurprise, amountOfFear, amountOfAngry, amountOfDisgust, amountOfNeutral;
	int currentEmo;

	int sizeOfWindow;
	int sizeOfWindow_s;
	int sizeOfWindow_R;
	int sizeOfWindow_M;

	int cursor;
	int cursor_d;
	int cursor_s;
	int cursor_m;

	int numOfFrame;
	int candidEmo[7];	// for voting
	int mayor;
	int rear, front, record;

	int rear_Range, record_Range;
	BOOL prev_r, curr_r, curr_f, next_f;

	BOOL initFront;
	BOOL initFront_M;
	BOOL initFront_Range;
	BOOL PITCH, YAW;

	int frequency[2] = {0,};
	int frequencyRange[2] = { 0, };
	int frequencyEmo[7] = { 0, };
	int winner;
	int winnerRange;

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

	//Headpose
	PXCFaceData::PoseEulerAngles angles;
	pxcF32 hr;

	int Int1;
	int hrcnt1, hrcnt2, hrcnt3, hrcnt4, hrcnt5, hrcnt6;
	pxcF32 pre_hr1, pre_hr2, pre_hr3,pre_hr4;
	int compare_hr1, compare_hr2, compare_hr3;

	int sumEyesTurnLeft, sumEyesTurnRight, sumEyeyaw, sumEyepitch;
	int avgEyesTurnLeft, avgEyesTurnRight, avgEyeyaw, avgEyepitch;
	int gazecnt;

	int count1, count2, count3, count4, count5, count6;

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
	
	
};

