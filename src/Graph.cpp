#include <graph.h>

GraphManager::GraphManager(HWND window, HWND graph) :
	m_graph(graph), m_window(window)
{
	DrawGraph();
	x = y = 0;
}

void GraphManager::DrawGraph()
{
	HBITMAP hBmp;


	HDC dc = GetDC(m_graph);

	hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_HAPPY), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
	SendDlgItemMessage(m_window, IDC_EMO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);

	SetBkColor(dc, RGB(0, 0, 0));

	ReleaseDC(m_graph, dc);
}

void GraphManager::DrawPoint()
{

}