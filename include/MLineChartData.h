#pragma once

class MLineChartData
{
public:
	typedef struct __ITEM
	{
		float fOrigin;
		float fDest;
		float fIntv;
	} __ITEM;

public:
	MLineChartData(void);
public:
	~MLineChartData(void);

public:
	CArray<__ITEM*, __ITEM*> lstData;
	float nYMin;
	float nYMax;
public:
	int Add(float fOrigin, float fDest);
	void Clear();
};
