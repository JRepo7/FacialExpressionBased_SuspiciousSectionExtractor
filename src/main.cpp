	/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2012-2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/
#include "afxwin.h"
#include <Windows.h>
#include <WindowsX.h>
#include <commctrl.h>
#include <map>
#include "math.h"
#include "resource.h"
#include "pxcfacemodule.h"
#include "pxcfacedata.h"
#include "pxcvideomodule.h"  
#include "pxcfaceconfiguration.h"
#include "pxcmetadata.h"
#include "service/pxcsessionservice.h"
#include "FaceTrackingFrameRateCalculator.h"
#include "FaceTrackingRendererManager.h"
#include "FaceTrackingRenderer2D.h"
#include "FaceTrackingRenderer3D.h"
#include "FaceTrackingUtilities.h"
#include "FaceTrackingProcessor.h"
#include "graph.h"
#define CAPTURE 8282
#define ADJUST 5252
#define EXP_TIMER 9292

pxcCHAR fileName[1024] = { 0 };
PXCSession* session = NULL;
FaceTrackingRendererManager* renderer = NULL;
FaceTrackingProcessor* processor = NULL;
GraphManager* graph = NULL;

HANDLE ghMutex;

volatile bool isRunning = false;
volatile bool isStopped = false;
volatile bool isActiveApp = true;
//�߰��� �÷���
volatile bool ADJ_FLAG = false;
volatile bool RVS_ADJ_FLAG = false;

static int controls[] = {ID_START, ID_STOP, ID_REGISTER, ID_UNREGISTER, IDC_DISTANCES,
						ID_ADJUST,IDC_TEXT_I10,IDC_TEXT_I11,IDC_TEXT_I12, IDC_CAP_EXP, 
						IDC_EXP_CNT1, IDC_EXP_CNT2, IDC_EXP_CNT3, IDC_EXP_CNT4, IDC_EXP_CNT5, IDC_EXP_CNT6, IDC_EXP_CNT7
};
static RECT layout[3 + sizeof(controls) / sizeof(controls[0])];

std::map<int, PXCFaceConfiguration::TrackingModeType> CreateProfileMap()
{
	std::map<int, PXCFaceConfiguration::TrackingModeType> map;
	map[0] = PXCFaceConfiguration::TrackingModeType::FACE_MODE_COLOR_PLUS_DEPTH;
	map[1] = PXCFaceConfiguration::TrackingModeType::FACE_MODE_IR;
	map[2] = PXCFaceConfiguration::TrackingModeType::FACE_MODE_COLOR;
	return map;
}
std::map<int, PXCFaceConfiguration::TrackingModeType> s_profilesMap = CreateProfileMap();

std::map<int, PXCCapture::DeviceInfo> CreateDeviceInfoMap()
{
	std::map<int, PXCCapture::DeviceInfo> map;	
	return map;
}
std::map<int, PXCCapture::DeviceInfo> g_deviceInfoMap = CreateDeviceInfoMap();

pxcCHAR* GetStringFromFaceMode(PXCFaceConfiguration::TrackingModeType mode)
{
	switch (mode) 
	{
	case PXCFaceConfiguration::TrackingModeType::FACE_MODE_COLOR_STILL:
		return L"2D_STILL";
	case PXCFaceConfiguration::TrackingModeType::FACE_MODE_COLOR:
		return L"2D";
	case PXCFaceConfiguration::TrackingModeType::FACE_MODE_COLOR_PLUS_DEPTH:
		return L"3D";
	case PXCFaceConfiguration::TrackingModeType::FACE_MODE_IR:
		return L"3D IR";
	}

	return L"";
}

void GetPlaybackFile(void) 
{
	OPENFILENAME filename;
	memset(&filename, 0, sizeof(filename));
	filename.lStructSize = sizeof(filename);
	filename.lpstrFilter = L"RSSDK clip (*.rssdk)\0*.rssdk\0Old format clip (*.pcsdk)\0*.pcsdk\0All Files (*.*)\0*.*\0\0";
	filename.lpstrFile = fileName; 
	fileName[0] = 0;
	filename.nMaxFile = sizeof(fileName) / sizeof(pxcCHAR);
	filename.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_EXPLORER;
	if (!GetOpenFileName(&filename)) 
		fileName[0] = 0;
}

void GetRecordFile(void) 
{
	OPENFILENAME filename;
	memset(&filename, 0, sizeof(filename));
	filename.lStructSize = sizeof(filename);
	filename.lpstrFilter = L"RSSDK clip (*.rssdk)\0*.rssdk\0All Files (*.*)\0*.*\0\0";
	filename.lpstrFile = fileName; 
	fileName[0] = 0;
	filename.nMaxFile = sizeof(fileName) / sizeof(pxcCHAR);
	filename.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_EXPLORER;
	if (GetSaveFileName(&filename)) {
		if (filename.nFilterIndex==1 && filename.nFileExtension==0) {
			size_t len = std::char_traits<wchar_t>::length(fileName);
			if (len>1 && len<sizeof(fileName)/sizeof(pxcCHAR)-7) {
				wcscpy_s(&fileName[len], rsize_t(7), L".rssdk\0");
			}
		}
	} else fileName[0] = 0;
}

void PopulateDevice(HMENU menu)
{
	DeleteMenu(menu, 0, MF_BYPOSITION);

	PXCSession::ImplDesc desc;
	memset(&desc, 0, sizeof(desc)); 
	desc.group = PXCSession::IMPL_GROUP_SENSOR;
	desc.subgroup = PXCSession::IMPL_SUBGROUP_VIDEO_CAPTURE;
	HMENU menu1 = CreatePopupMenu();
	int itemPosition = 0;
	for (int i = 0, k = ID_DEVICEX; ; ++i)
	{
		PXCSession::ImplDesc desc1;
		if (session->QueryImpl(&desc, i, &desc1) < PXC_STATUS_NO_ERROR)
			break;

		PXCCapture *capture;
		if (session->CreateImpl<PXCCapture>(&desc1, &capture) < PXC_STATUS_NO_ERROR) 
			continue;

		for (int j = 0; ; ++j) {
			PXCCapture::DeviceInfo deviceInfo;
			if (capture->QueryDeviceInfo(j, &deviceInfo) < PXC_STATUS_NO_ERROR) 
				break;
			g_deviceInfoMap[itemPosition++] = deviceInfo;
			AppendMenu(menu1, MF_STRING, k++, deviceInfo.name);
		}

		capture->Release();
	}
	CheckMenuRadioItem(menu1, 0, GetMenuItemCount(menu1), 0, MF_BYPOSITION);
	InsertMenu(menu, 0, MF_BYPOSITION | MF_POPUP, (UINT_PTR)menu1, L"Device");
}

void PopulateModule(HMENU menu) 
{
	DeleteMenu(menu, 1, MF_BYPOSITION);

	PXCSession::ImplDesc desc, desc1;
	memset(&desc, 0, sizeof(desc));
	desc.cuids[0] = PXCFaceModule::CUID;
	HMENU menu1 = CreatePopupMenu();

	for (int i = 0; ; ++i)
	{
		if (session->QueryImpl(&desc, i, &desc1) < PXC_STATUS_NO_ERROR) 
			break;
		AppendMenu(menu1, MF_STRING, ID_MODULEX + i, desc1.friendlyName);
	}

	CheckMenuRadioItem(menu1, 0, GetMenuItemCount(menu1), 0, MF_BYPOSITION);
	InsertMenu(menu, 1, MF_BYPOSITION | MF_POPUP, (UINT_PTR)menu1, L"Module");
}

//winApi  
//dialog Management 
void PopulateProfile(HWND dialogWindow)
{
	HMENU menu = GetMenu(dialogWindow);
	DeleteMenu(menu, 2, MF_BYPOSITION);
	HMENU menu1 = CreatePopupMenu();

	PXCSession::ImplDesc desc;
	memset(&desc, 0, sizeof(desc));
	desc.cuids[0] = PXCFaceModule::CUID;
	wcscpy_s<sizeof(desc.friendlyName) / sizeof(pxcCHAR)>(desc.friendlyName, FaceTrackingUtilities::GetCheckedModule(dialogWindow));

	PXCFaceModule *faceModule = NULL;
	if (session->CreateImpl<PXCFaceModule>(&desc, &faceModule) >= PXC_STATUS_NO_ERROR) 
	{
		for (unsigned int i = 0; i < s_profilesMap.size(); ++i)
		{
			WCHAR line[256];
			swprintf_s<sizeof(line) / sizeof(WCHAR)>(line, L"%s", GetStringFromFaceMode(s_profilesMap[i]));
			AppendMenu(menu1, MF_STRING, ID_PROFILEX + i, line);
		}

		CheckMenuRadioItem(menu1, 0, GetMenuItemCount(menu1), 0, MF_BYPOSITION);
    
        faceModule->Release();
    }
	InsertMenu(menu, 2, MF_BYPOSITION | MF_POPUP, (UINT_PTR)menu1, L"Profile");
}

void SaveLayout(HWND dialogWindow) 
{
	GetClientRect(dialogWindow, &layout[0]);
	ClientToScreen(dialogWindow, (LPPOINT)&layout[0].left);
	ClientToScreen(dialogWindow, (LPPOINT)&layout[0].right);
	GetWindowRect(GetDlgItem(dialogWindow, IDC_PANEL), &layout[1]);
	GetWindowRect(GetDlgItem(dialogWindow, IDC_STATUS), &layout[2]);
	for (int i = 0; i < sizeof(controls) / sizeof(controls[0]); ++i)
		GetWindowRect(GetDlgItem(dialogWindow, controls[i]), &layout[3 + i]);
}

void RedoLayout(HWND dialogWindow)
{
	RECT rectangle;
	GetClientRect(dialogWindow, &rectangle);

	/* Status */
	SetWindowPos(GetDlgItem(dialogWindow, IDC_STATUS), dialogWindow, 
		0,
		rectangle.bottom - (layout[2].bottom - layout[2].top),
		rectangle.right - rectangle.left,
		(layout[2].bottom - layout[2].top),
		SWP_NOZORDER);

	/* Panel */
	SetWindowPos(
		GetDlgItem(dialogWindow,IDC_PANEL), dialogWindow,
		(layout[1].left - layout[0].left),
		(layout[1].top - layout[0].top),
		rectangle.right - (layout[1].left-layout[0].left) - (layout[0].right - layout[1].right),
		rectangle.bottom - (layout[1].top - layout[0].top) - (layout[0].bottom - layout[1].bottom),
		SWP_NOZORDER);

	/* Buttons & CheckBoxes */
	for (int i = 0; i < sizeof(controls) / sizeof(controls[0]); ++i)
	{
		SetWindowPos(
			GetDlgItem(dialogWindow,controls[i]), dialogWindow,
			rectangle.right - (layout[0].right - layout[3 + i].left),
			(layout[3 + i].top - layout[0].top),
			(layout[3 + i].right - layout[3 + i].left),
			(layout[3 + i].bottom - layout[3 + i].top),
			SWP_NOZORDER);
	}
}

static DWORD WINAPI RenderingThread(LPVOID arg)
{
	while (true)
	{
		renderer->Render();
		renderer->GetExpIntensity();
		renderer->GetLandmarkPoint();

		if (ADJ_FLAG == TRUE)
		{
			renderer->PrepValue();
		}
		
		if(ADJ_FLAG == FALSE && renderer->adj_frameCount != 0)
		{
			renderer->SetThresValue();
			renderer->InitValue();
		}

		if (ADJ_FLAG == FALSE)
		{
			renderer->CvtLandmarkToIntensity();
			renderer->DetermineExpression();
		}
	}
}

static DWORD WINAPI ProcessingThread(LPVOID arg)
{
	HWND window = (HWND)arg;
	processor->Process(window);

	isRunning = false;
	PostMessage(window, WM_COMMAND, ID_STOP, 0);
	return 0;
}

INT_PTR CALLBACK MessageLoopThread(HWND dialogWindow, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
	HMENU menu1 = GetMenu(dialogWindow);
	HMENU menu2;
	pxcCHAR* deviceName;

	switch (message) 
	{ 
		case WM_INITDIALOG:
			PopulateDevice(menu1);
			deviceName = FaceTrackingUtilities::GetCheckedDevice(dialogWindow);

			if (wcsstr(deviceName, L"R200") == NULL && wcsstr(deviceName, L"DS4") == NULL)
			{
				Button_Enable(GetDlgItem(dialogWindow, ID_REGISTER), false);
				Button_Enable(GetDlgItem(dialogWindow, ID_UNREGISTER), false);
			}

			PopulateModule(menu1);
			PopulateProfile(dialogWindow);
			SaveLayout(dialogWindow);

			ShowWindow(dialogWindow, SW_MAXIMIZE);


			graph = new GraphManager(dialogWindow, GetDlgItem(dialogWindow, IDC_GRAPH));
			graph->DrawGraph();


			//���������ð�����
			SetTimer(dialogWindow, EXP_TIMER, 200, NULL);//0.2s

			return TRUE; 

		case WM_TIMER:
			switch (wParam) {
			case ADJUST:
				ADJ_FLAG = false;
				KillTimer(dialogWindow, ADJUST);
				break;

			case EXP_TIMER:
				renderer->CaptureSubtleExpression();
				renderer->DisplayExpressionUsingEmoji();
			}
			return TRUE;

		case WM_COMMAND: 
			menu2 = GetSubMenu(menu1, 0);
			if (LOWORD(wParam) >= ID_DEVICEX && LOWORD(wParam) < ID_DEVICEX + GetMenuItemCount(menu2)) 
			{
				CheckMenuRadioItem(menu2, 0, GetMenuItemCount(menu2), LOWORD(wParam) - ID_DEVICEX, MF_BYPOSITION);	
				deviceName = FaceTrackingUtilities::GetCheckedDevice(dialogWindow);
				bool disable = (wcsstr(deviceName, L"R200") != NULL || wcsstr(deviceName, L"DS4") != NULL);
				return TRUE;
			}

			menu2 = GetSubMenu(menu1, 1);
			if (LOWORD(wParam) >= ID_MODULEX && LOWORD(wParam) < ID_MODULEX + GetMenuItemCount(menu2)) 
			{
				CheckMenuRadioItem(menu2, 0, GetMenuItemCount(menu2), LOWORD(wParam) - ID_MODULEX,MF_BYPOSITION);
				PopulateProfile(dialogWindow);
				return TRUE;
			}

			menu2 = GetSubMenu(menu1, 2);
			if (LOWORD(wParam) >= ID_PROFILEX && LOWORD(wParam) < ID_PROFILEX + GetMenuItemCount(menu2)) 
			{
				CheckMenuRadioItem(menu2, 0, GetMenuItemCount(menu2), LOWORD(wParam) - ID_PROFILEX,MF_BYPOSITION);
				if (FaceTrackingUtilities::GetCheckedProfile(dialogWindow) == PXCFaceConfiguration::FACE_MODE_COLOR)
				{
					
					renderer->SetRendererType(FaceTrackingRenderer::R2D);
				}
				else if (FaceTrackingUtilities::GetCheckedProfile(dialogWindow) == PXCFaceConfiguration::FACE_MODE_COLOR_PLUS_DEPTH || FaceTrackingUtilities::GetCheckedProfile(dialogWindow) == PXCFaceConfiguration::FACE_MODE_IR)
				{
					renderer->SetRendererType(FaceTrackingRenderer::R3D);
				}
				return TRUE;
			}

			switch (LOWORD(wParam)) 
			{
			case ID_ADJUST:
				renderer->InitValue();
				ADJ_FLAG = TRUE;
				SetTimer(dialogWindow, ADJUST, 2000, NULL);// 2s
				
				return TRUE;

			case IDCANCEL:
				isStopped = true;
				if (isRunning) 
				{
					PostMessage(dialogWindow, WM_COMMAND, IDCANCEL, 0);
				} 
				else 
				{
					DestroyWindow(dialogWindow); 
					PostQuitMessage(0);
				}  
				return TRUE;
			case ID_START:

				Button_Enable(GetDlgItem(dialogWindow, ID_START), false);
				Button_Enable(GetDlgItem(dialogWindow, ID_STOP), true);
				//������ 2D 3D
				renderer->SetRendererType(FaceTrackingRenderer::R2D);

				for (int i = 0;i < GetMenuItemCount(menu1); ++i)
					EnableMenuItem(menu1, i, MF_BYPOSITION | MF_GRAYED);

				DrawMenuBar(dialogWindow);
				isStopped = false;
				isRunning = true;

				if (processor) 
					delete processor;

				processor = new FaceTrackingProcessor(dialogWindow);

				CreateThread(0, 0, ProcessingThread, dialogWindow, 0, 0);
				////if (FaceTrackingUtilities::IsModuleSelected(dialogWindow, IDC_RECOGNITION))
				////{
				//	Button_Enable(GetDlgItem(dialogWindow, ID_REGISTER), true);
				//	Button_Enable(GetDlgItem(dialogWindow, ID_UNREGISTER), true);
				////}

				renderer->InitValue();
				ADJ_FLAG = TRUE;
				SetTimer(dialogWindow, ADJUST, 2000, NULL);

				Sleep(0); //TODO: remove
				return TRUE;

			case ID_STOP:
				isStopped = true;
				if (isRunning) 
				{
					PostMessage(dialogWindow, WM_COMMAND, ID_STOP, 0);
				}
				else 
				{
					for (int i = 0; i < GetMenuItemCount(menu1); ++i)
						EnableMenuItem(menu1, i, MF_BYPOSITION | MF_ENABLED);

					DrawMenuBar(dialogWindow);
					Button_Enable(GetDlgItem(dialogWindow, ID_START), true);
					Button_Enable(GetDlgItem(dialogWindow, ID_STOP), false);
					deviceName = FaceTrackingUtilities::GetCheckedDevice(dialogWindow);
					bool disable = (wcsstr(deviceName, L"R200") != NULL || wcsstr(deviceName, L"DS4") != NULL);
					//if (FaceTrackingUtilities::IsModuleSelected(dialogWindow, IDC_RECOGNITION))
					//{
					//	Button_Enable(GetDlgItem(dialogWindow, ID_REGISTER), false);
					//	Button_Enable(GetDlgItem(dialogWindow, ID_UNREGISTER), false);
					//}
				}
				renderer->Reset();
				return TRUE;

			case ID_MODE_LIVE:
				CheckMenuItem(menu1, ID_MODE_LIVE, MF_CHECKED);
				CheckMenuItem(menu1, ID_MODE_PLAYBACK, MF_UNCHECKED);
				CheckMenuItem(menu1, ID_MODE_RECORD, MF_UNCHECKED);
				return TRUE;

			case ID_MODE_PLAYBACK:
				CheckMenuItem(menu1, ID_MODE_LIVE, MF_UNCHECKED);
				CheckMenuItem(menu1, ID_MODE_PLAYBACK, MF_CHECKED);
				CheckMenuItem(menu1, ID_MODE_RECORD, MF_UNCHECKED);
				GetPlaybackFile();
				return TRUE;

			case ID_MODE_RECORD:
				CheckMenuItem(menu1, ID_MODE_LIVE, MF_UNCHECKED);
				CheckMenuItem(menu1, ID_MODE_PLAYBACK, MF_UNCHECKED);
				CheckMenuItem(menu1, ID_MODE_RECORD, MF_CHECKED);
				GetRecordFile();
				return TRUE;

			case ID_VIEW_EYEWEAR:
				if ((GetMenuState(GetMenu(dialogWindow), ID_VIEW_EYEWEAR, MF_BYCOMMAND) & MF_CHECKED) != 0)
				{
					CheckMenuItem(menu1, ID_VIEW_EYEWEAR, MF_UNCHECKED);
					ShowWindow(GetDlgItem(dialogWindow, IDC_DISTANCES), SW_HIDE);
					renderer->SetActivateEyeCenterCalculations(false);
				}
				else
				{
					CheckMenuItem(menu1, ID_VIEW_EYEWEAR, MF_CHECKED);
					ShowWindow(GetDlgItem(dialogWindow, IDC_DISTANCES), SW_SHOW);
					Button_Enable(GetDlgItem(dialogWindow, IDC_DISTANCES), true);
					renderer->SetActivateEyeCenterCalculations(true); 
				}
				return TRUE;

			case ID_REGISTER:
				processor->RegisterUser();
				return TRUE;

			case ID_UNREGISTER:
				processor->UnregisterUser();
				return TRUE;
			case IDC_DISTANCES:
				renderer->DrawDistances();
				return TRUE;
			} 
			break;
		case WM_SIZE:
			RedoLayout(dialogWindow);
			return TRUE;
		case WM_ACTIVATEAPP:
			isActiveApp = wParam != 0;
			break;
	
		case WM_NOTIFY:

			//switch (((LPNMHDR)lParam)->code)
			//{
			//  case TCN_SELCHANGE:
			//	{ 
			//		int iPage = 0;
			//		deviceName = FaceTrackingUtilities::GetCheckedDevice(dialogWindow);
			//	
			//		if (iPage == 0)
			//		{
			//			renderer->SetRendererType(FaceTrackingRenderer::R2D);
			//		}
			//		if (iPage == 1 && (wcsstr(deviceName, L"3D") || FaceTrackingUtilities::GetPlaybackState(dialogWindow)) &&
			//			(FaceTrackingUtilities::GetCheckedProfile(dialogWindow) == PXCFaceConfiguration::FACE_MODE_COLOR_PLUS_DEPTH
			//			|| FaceTrackingUtilities::GetCheckedProfile(dialogWindow) == PXCFaceConfiguration::FACE_MODE_IR))
			//		{
			//			renderer->SetRendererType(FaceTrackingRenderer::R3D);
			//		}
			//		return TRUE;
			//	} 
			//}

			break;
	} 
	return FALSE; 
}


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int) {

	//ADJ_FLAG = false;
	//RVS_ADJ_FLAG = false;

	InitCommonControls();	//enable commlist.h 

	session = PXCSession::CreateInstance();
	if (session == NULL) 
	{
        MessageBoxW(0, L"Failed to create an SDK session", L"Face Viewer", MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }

    HWND dialogWindow = CreateDialogW(hInstance, MAKEINTRESOURCE(IDD_MAINFRAME), 0, MessageLoopThread);
    if (!dialogWindow)  
	{
        MessageBoxW(0, L"Failed to create a window", L"Face Viewer", MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }

	HWND statusWindow = CreateStatusWindow(WS_CHILD | WS_VISIBLE, L"", dialogWindow, IDC_STATUS);	
	if (!statusWindow) 
	{
	   MessageBoxW(0, L"Failed to create a status bar", L"Face Viewer", MB_ICONEXCLAMATION | MB_OK);
        return 1;
	}
	
	int statusWindowParts[] = {230, -1};
	SendMessage(statusWindow, SB_SETPARTS, sizeof(statusWindowParts)/sizeof(int), (LPARAM) statusWindowParts);
	SendMessage(statusWindow, SB_SETTEXT, (WPARAM)(INT) 0, (LPARAM) (LPSTR) TEXT("OK"));
	UpdateWindow(dialogWindow);

	FaceTrackingRenderer2D* renderer2D = new FaceTrackingRenderer2D(dialogWindow);
	if(renderer2D == NULL)
	{
		MessageBoxW(0, L"Failed to create 2D renderer", L"Face Viewer", MB_ICONEXCLAMATION | MB_OK);
		return 1;
	}
	FaceTrackingRenderer3D* renderer3D = new FaceTrackingRenderer3D(dialogWindow, session);
	if(renderer3D == NULL)
	{
		MessageBoxW(0, L"Failed to create 3D renderer", L"Face Viewer", MB_ICONEXCLAMATION | MB_OK);
		delete renderer2D;
		return 1;
	}

	renderer = new FaceTrackingRendererManager(renderer2D, renderer3D,dialogWindow);
	if(renderer == NULL)
	{
		MessageBoxW(0, L"Failed to create renderer manager", L"Face Viewer", MB_ICONEXCLAMATION | MB_OK);
		delete renderer2D;
		delete renderer3D;
		return 1;
	}


	ghMutex = CreateMutex(NULL, FALSE, NULL);
	if (ghMutex == NULL) 
	{
		MessageBoxW(0, L"Failed to create mutex", L"Face Viewer", MB_ICONEXCLAMATION | MB_OK);
		delete renderer;
		return 1; 
	}

	//significant!
	CreateThread(NULL, NULL, RenderingThread, NULL, NULL, NULL);

    MSG msg;
	while (int status = GetMessageW(&msg, NULL, 0, 0))
	{
        if (status == -1)
			return status;

        TranslateMessage(&msg);
        DispatchMessage(&msg);

		Sleep(0); // let other threads breathe
    }


	CloseHandle(renderer->GetRenderingFinishedSignal());

	if (processor)
		delete processor;

	if (renderer)
		delete renderer;

	session->Release();
    return (int)msg.wParam;
}

/*
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam)
{
	HDC hdc;

	switch (iMessage)
	{
	case WM_LBUTTONDOWN:
		hdc = GetDC(hWnd);
		TextOut(hdc, 100, 100, TEXT("hi"), 28);
		ReleaseDC(hWnd, hdc);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, IParam));
}
//*/