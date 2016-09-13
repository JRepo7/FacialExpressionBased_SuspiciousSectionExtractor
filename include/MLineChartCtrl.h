#pragma once

#include "MLineChartConfig.h"
#include "MLineChartData.h"


// MLineChartCtrl

class MLineChartCtrl : public CWnd
{
	DECLARE_DYNAMIC(MLineChartCtrl)

public:
	MLineChartCtrl();
	virtual ~MLineChartCtrl();

public:
	MLineChartConfig m_ChartConfig;
	MLineChartData m_ChartData;

	HBITMAP m_hMemBitmap;
	HDC m_hMemDC;

	CRect rcWindow;
	CRect rcData;
	HFONT hFont, hOldFont;
	double dblXIntv, dblYIntv;
	int nXDataSize;
	int dblYDataSize, dblYCenter, dblYMin, dblYMax, dblYPixelSize, dblYDataPerP;

public:
	void DrawChart(HDC hDC);
	void CalcDataRect();
	void CalcAxis();
	void DrawBG(HDC hDC);
	void DrawAxisLine(HDC hDC);
	void DrawAxisX(HDC hDC);
	void DrawAxisY(HDC hDC);
	void DrawData(HDC hDC);
	void DrawDotRect(HDC hDC, int x, int y, int nW, int nH);

	void CreateMemResource();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
