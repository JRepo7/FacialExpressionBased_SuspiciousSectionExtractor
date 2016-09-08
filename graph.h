#pragma once

#include <windows.h>

class graphManager
{
public:
	graphManager(HWND panel);
	void DrawGraph();
	void DrawPoint();

	int x;
	int y;

private:
	HWND m_panel;

};