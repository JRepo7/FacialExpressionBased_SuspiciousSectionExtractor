// MLineChartCtrl.cpp : 구현 파일입니다.
//

#include <afx.h>
#include <afxwin.h>  
#include "MLineChartCtrl.h"


// MLineChartCtrl

IMPLEMENT_DYNAMIC(MLineChartCtrl, CWnd)

MLineChartCtrl::MLineChartCtrl()
{
	m_hMemBitmap = NULL;
	m_hMemDC = NULL;
}

MLineChartCtrl::~MLineChartCtrl()
{
}


BEGIN_MESSAGE_MAP(MLineChartCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// MLineChartCtrl 메시지 처리기입니다.



void MLineChartCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	HDC hDC = dc.m_hDC;
	
	GetClientRect(rcWindow);

	DrawChart(m_hMemDC);

	BitBlt(hDC, 0, 0, rcWindow.Width(), rcWindow.Height(), m_hMemDC, 0, 0, SRCCOPY);

}

void MLineChartCtrl::DrawChart(HDC hDC)
{

	// CREATE FONT
	{
		//LOGFONT lf;
		//memset(&lf, 0, sizeof(lf));
		//strcpy(lf.lfFaceName, "Verdana");
		//lf.lfHeight = 13;
		//hFont = CreateFontIndirect(&lf);
		HFONT hFont = CreateFont(15, 0, 0, 0, FW_LIGHT, 0, 0, 0, 0, 0, 0, 5, 0, L"MONOSPACE");
		hOldFont = (HFONT)SelectObject(hDC, hFont);
	}

	CalcDataRect();
	CalcAxis();

	DrawBG(hDC);
	DrawAxisX(hDC);
	DrawAxisY(hDC);

	DrawAxisLine(hDC);
	DrawData(hDC);

	// DELETE FONT
	{
		SelectObject(hDC, hFont);
		DeleteObject(hFont);
	}
}
void MLineChartCtrl::CalcDataRect()
{
	// CALC DATA RECT
	{
		rcData.left = rcWindow.left + m_ChartConfig.nMarginLeft;
		rcData.right = rcWindow.right - m_ChartConfig.nMarginRight;
		rcData.top = rcWindow.top + m_ChartConfig.nMarginTop;
		rcData.bottom = rcWindow.bottom - m_ChartConfig.nMarginBottom;
	}
}

void MLineChartCtrl::CalcAxis()
{
	// X축 계산
	{
		if(m_ChartData.lstData.GetSize()>m_ChartConfig.nInitWidth)
			nXDataSize = m_ChartConfig.nInitWidth + ((m_ChartData.lstData.GetSize()-m_ChartConfig.nInitWidth)/(double)m_ChartConfig.nWidthExtent)*m_ChartConfig.nWidthExtent;
		else
			nXDataSize = m_ChartConfig.nInitWidth;

		dblXIntv = rcData.Width()/(double)nXDataSize;
	}


	// Y축 계산
	{
		dblYCenter = (int)(m_ChartData.nYMax+m_ChartData.nYMin)/2;

		if(m_ChartData.nYMax-m_ChartData.nYMin<1)
		{
			dblYMin = m_ChartData.nYMin - 1;
			dblYMax = m_ChartData.nYMax + 1;
			dblYDataSize = m_ChartData.nYMax-m_ChartData.nYMin + 2;
		}
		else
		{
			dblYDataSize = m_ChartData.nYMax - m_ChartData.nYMin;
			dblYMin = m_ChartData.nYMin - dblYDataSize*.2;
			dblYMax = m_ChartData.nYMax + dblYDataSize*.2;
			dblYDataSize = dblYMax - dblYMin;
		}

		dblYPixelSize = rcData.Height()/dblYDataSize;
	}
}

void MLineChartCtrl::DrawBG(HDC hDC)
{
	HBRUSH hBrush = CreateSolidBrush(m_ChartConfig.crBG);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	FillRect(hDC, rcWindow, hBrush);

	SelectObject(hDC, hOldBrush);
	DeleteObject(hBrush);
}

void MLineChartCtrl::DrawAxisLine(HDC hDC)
{
	// DRAW AXIS LINE
	{
		HPEN hAxisLine = CreatePen(PS_SOLID, 2, m_ChartConfig.crAxis);
		HPEN hOldPen = (HPEN)SelectObject(hDC, hAxisLine);
		
		MoveToEx(hDC, rcData.left, rcData.top, NULL);
		LineTo(hDC, rcData.left, rcData.bottom);
		LineTo(hDC, rcData.right, rcData.bottom);
		
		SelectObject(hDC, hOldPen);
		DeleteObject(hAxisLine);
	}

}

void MLineChartCtrl::DrawAxisX(HDC hDC)
{
	// X축 그리기
	{
		HPEN hAxisLine = CreatePen(PS_SOLID, 1, m_ChartConfig.crGuideLine);
		HPEN hOldPen = (HPEN)SelectObject(hDC, hAxisLine);

		int nAxisRight = rcData.left;
		CString szText;
		for(int i=0; i<nXDataSize; i++)
		{
			MoveToEx(hDC, rcData.left + dblXIntv*i, rcData.bottom, NULL);
			LineTo(hDC, rcData.left + dblXIntv*i, rcData.bottom+5);

			//szText.Format(_T("%d"), i+1);

			//if(i == 0 || nAxisRight+5 < rcData.left + dblXIntv*i)
			//{
			//	TextOut(hDC, rcData.left + dblXIntv*i, rcData.bottom +5, szText, szText.GetLength());

			//	SIZE size;
			//	GetTextExtentPoint32(hDC, szText, szText.GetLength(), &size);

			//	nAxisRight = rcData.left + dblXIntv*i + size.cx;
			//}
		}

		SIZE sizeText;
		GetTextExtentPoint32(hDC, m_ChartConfig.szAxisXNM, m_ChartConfig.szAxisXNM.GetLength(), &sizeText);

		TextOut(hDC, rcWindow.right - sizeText.cx - 5, rcWindow.bottom - sizeText.cy - 5, m_ChartConfig.szAxisXNM, m_ChartConfig.szAxisXNM.GetLength());

		SelectObject(hDC, hOldPen);
		DeleteObject(hAxisLine);
	}
}

void MLineChartCtrl::DrawAxisY(HDC hDC)
{
	// Y축 그리기
	{
		HPEN hAxisLine = CreatePen(PS_SOLID, 1, m_ChartConfig.crGuideLine);
		HPEN hOldPen = (HPEN)SelectObject(hDC, hAxisLine);
		CString szText;
		SIZE size;

		{
			// MIN, CENTER, MAX
			MoveToEx(hDC, rcData.left - 4, rcData.top, NULL);
			LineTo(hDC, rcData.right, rcData.top);

			szText.Format(_T("100%%"));
			GetTextExtentPoint32(hDC, szText, szText.GetLength(), &size);
			TextOut(hDC, rcData.left - 6 - size.cx, rcData.top - size.cy/2, szText, szText.GetLength());

			MoveToEx(hDC, rcData.left - 4, rcData.CenterPoint().y, NULL);
			LineTo(hDC, rcData.right, rcData.CenterPoint().y);

			szText.Format(_T("50%%"));
			GetTextExtentPoint32(hDC, szText, szText.GetLength(), &size);
			TextOut(hDC, rcData.left - 6 - size.cx, rcData.CenterPoint().y - size.cy/2, szText, szText.GetLength());

			MoveToEx(hDC, rcData.left - 4, rcData.bottom, NULL);
			LineTo(hDC, rcData.right, rcData.bottom);

			szText.Format(_T("0%%"));
			GetTextExtentPoint32(hDC, szText, szText.GetLength(), &size);
			TextOut(hDC, rcData.left - 6 - size.cx, rcData.bottom - size.cy/2, szText, szText.GetLength());
		}

		if(rcData.Height() > 200)
		{
			MoveToEx(hDC, rcData.left - 4, (rcData.CenterPoint().y+rcData.top)/2, NULL);
			LineTo(hDC, rcData.right, (rcData.CenterPoint().y+rcData.top)/2);

			szText.Format(_T("75%%"));
			GetTextExtentPoint32(hDC, szText, szText.GetLength(), &size);
			TextOut(hDC, rcData.left - 6 - size.cx, (rcData.top+rcData.CenterPoint().y)/2 - size.cy/2, szText, szText.GetLength());

			MoveToEx(hDC, rcData.left - 4, (rcData.CenterPoint().y+rcData.bottom)/2, NULL);
			LineTo(hDC, rcData.right, (rcData.CenterPoint().y+rcData.bottom)/2);

			szText.Format(_T("25%%"));
			GetTextExtentPoint32(hDC, szText, szText.GetLength(), &size);
			TextOut(hDC, rcData.left - 6 - size.cx, (rcData.bottom+rcData.CenterPoint().y)/2 - size.cy/2, szText, szText.GetLength());
		}

		TextOut(hDC, 5, 5, m_ChartConfig.szAxisYNM, m_ChartConfig.szAxisYNM.GetLength());

		SelectObject(hDC, hOldPen);
		DeleteObject(hAxisLine);
	}
}

void MLineChartCtrl::DrawData(HDC hDC)
{
	// DRAW DATA
	{
		int x, y;

		HPEN hDotPen = CreatePen(PS_SOLID, 1, m_ChartConfig.crDot);
		HPEN hLinePen = CreatePen(PS_SOLID, 1, m_ChartConfig.crDataLine);
		HPEN hOldPen = (HPEN)SelectObject(hDC, hDotPen);

		for(int i=0; i<m_ChartData.lstData.GetSize(); i++)
		{
			y = rcData.bottom+ 295-(m_ChartData.lstData.GetAt(i)->fDest) * 42;
			//y = 720 - (m_ChartData.lstData.GetAt(i)->fDest) * 50;
			x = rcData.left + dblXIntv*(i);

			DrawDotRect(hDC, x, y, 2, 2);
		}

		SelectObject(hDC, hLinePen);

		if(m_ChartConfig.bShowDataLine)
		{
			for(int i=0; i<m_ChartData.lstData.GetSize(); i++)
			{
				y = rcData.bottom + 295 - (m_ChartData.lstData.GetAt(i)->fDest) * 42;
				//y = 720 - (m_ChartData.lstData.GetAt(i)->fDest) * 50;
				x = rcData.left + dblXIntv*(i);

				if(i == 0)
					MoveToEx(hDC, x, y, NULL);
				else
					LineTo(hDC, x, y);
			}
		}

		SelectObject(hDC, hOldPen);
		DeleteObject(hDotPen);
	}
}

void MLineChartCtrl::DrawDotRect(HDC hDC, int x, int y, int nW, int nH)
{
	Rectangle(hDC, x-nW, y-nH, x+nW, y+nH);
}

void MLineChartCtrl::OnSize(UINT nType, int cx, int cy)
{
	CreateMemResource();

	CWnd::OnSize(nType, cx, cy);
}

void MLineChartCtrl::CreateMemResource()
{
	CRect r;
	CDC * pDC = GetDC();

	GetClientRect(r);

	if(m_hMemDC == NULL)
		m_hMemDC = CreateCompatibleDC(pDC->m_hDC);

	if(m_hMemBitmap != NULL)
		DeleteObject(m_hMemBitmap);

	m_hMemBitmap = CreateCompatibleBitmap(pDC->m_hDC, r.Width(), r.Height());
	SelectObject(m_hMemDC, m_hMemBitmap);

	ReleaseDC(pDC);
}
