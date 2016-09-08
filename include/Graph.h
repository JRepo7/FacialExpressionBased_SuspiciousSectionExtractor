#pragma once

#include <windows.h>
#include "resource.h"

class GraphManager
{
public:
	GraphManager(HWND window, HWND graph);
	void DrawGraph();
	void DrawPoint();

	int x;
	int y;

private:
	HWND m_graph;
	HWND m_window;

};