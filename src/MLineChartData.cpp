#include <afx.h>
#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <windows.h>
#include "MLineChartData.h"
#include <math.h>

MLineChartData::MLineChartData(void)
{

}

MLineChartData::~MLineChartData(void)
{
	Clear();
}

int MLineChartData::Add(float fOrigin, float fDest)
{
	__ITEM* item = new __ITEM;
	item->fOrigin = fOrigin;
	item->fDest = fDest;
	//item->fIntv = 0;
	lstData.Add(item);


	nYMin = 0;
	nYMax = 7;
	// item->fIntv;
	//if(lstData.GetSize() == 1)
	//	nYMin = nYMax = item->fIntv;
	//else if(item->fIntv < nYMin)
	//	nYMin = item->fIntv;
	//else if(item->fIntv > nYMax)
	//	nYMax = item->fIntv;
	return (int)lstData.GetSize();
}

void MLineChartData::Clear()
{
	for(int i=lstData.GetSize()-1; i>=0; i--)
	{
		delete lstData.GetAt(i);
	}

	lstData.RemoveAll();
}