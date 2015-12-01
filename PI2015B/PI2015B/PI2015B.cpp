// PI2015B.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <math.h>
#include "PI2015B.h"
#include "DXGIManager.h"
#include "..\\Video\\AtWareVideoCapture.h"
#include "VideoProcessor.h"
#include "Frame.h"
#define MAX_LOADSTRING 100



// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

	CDXGIManager g_Manager;

	IAtWareVideoCapture* g_pVC;	//Interface Video Capture
	CVideoProcessor g_VP;
	CFrame* g_pPulledFrame; 
										
// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	VideoHost(HWND hWND, UINT msg, WPARAM wParam, LPARAM lParam );

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PI2015B, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PI2015B));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PI2015B));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= 0; // (HBRUSH)(COLOR_WINDOW+1)
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_PI2015B);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	ATOM a = RegisterClassEx( &wcex );
	wcex.lpfnWndProc = VideoHost;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"VideoHost";

	return a;
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
	IDXGIAdapter* pAdapter = g_Manager.EnumAdapters( hWnd );
	bool bResult = g_Manager.Initialize( hWnd, pAdapter, false );
	SAFE_RELEASE( pAdapter );
	if( !bResult )
		return FALSE;

	HWND hWndVH = CreateWindowEx( WS_EX_TOOLWINDOW, L"VideoHost", L"Vista Previa", 
								  WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
								  CW_USEDEFAULT, NULL, NULL, hInst, NULL );
	g_pVC = CreateAtWareVideoCapture();
	g_pVC->Initialize( NULL );
	if( g_pVC->EnumAndChooseCaptureDevice() )
	{
		printf( "Hola Mundo" );
		wprintf( L"Hola Mundo" );
		g_pVC->BuildStreamGraph();
		g_pVC->SetCallBack( &g_VP, 1 );
		AM_MEDIA_TYPE mt;
		g_pVC->GetMediaType( &mt );
		g_VP.m_mt = mt;
		g_pVC->Start();
		g_pVC->ShowPreviewWindow( true );
	}

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   ShowWindow( hWndVH, SW_SHOW );

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	static float s_ftime = 0;

	switch (message)
	{
			case WM_CREATE:
				SetTimer( hWnd, 1, 20, NULL );
				break;
			case WM_TIMER:
				switch( wParam )
				{ case 1:
					s_ftime += 0.02;
					InvalidateRect( hWnd, NULL, false );
				}
				break;
			case WM_SIZE:
				if( g_Manager.GetDevice() )
					g_Manager.Resize( LOWORD(lParam), HIWORD(lParam) );
			break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
		case WM_PAINT:
		{
			
			g_pPulledFrame  = g_VP.Pull();
			if( g_pPulledFrame != NULL  )
			{
				// Use pulled frame
				delete g_pPulledFrame;
			
				g_Manager.GetSwapChain()->Present( 1, 0 ); // 1-Sync  

			}
		}
			ValidateRect( hWnd, NULL );
			break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
LRESULT CALLBACK VideoHost(HWND hWND, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
	case WM_CREATE:
		return 0;
	case WM_SIZE:
		{
			RECT rc;
			GetClientRect( hWND, &rc );
			g_pVC->SetPreviewWindowPosition( &rc );
		}
		break;
	}
	return DefWindowProc( hWND, msg, wParam, lParam );
}
// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
