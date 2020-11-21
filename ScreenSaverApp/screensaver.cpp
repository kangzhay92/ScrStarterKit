#include "framework.h"
#include "screensaver.h"
#include <ScrnSave.h>

#ifdef _MSC_VER
#pragma comment(lib, "scrnsave.lib")   // Microsoft Visual C++ library for creating Screen saver application
#pragma comment(lib, "comctl32.lib")   // Windows Common Control Library, must be linked alongside with scrnsave.lib
#endif

// Use this function if you want to register new window class for the configure
// dialog, with the hModule parameter as HINSTANCE in CreateWindow function.
// Must be return TRUE if you want to create custom window class for the configure dialog.
BOOL WINAPI RegisterDialogClasses(HANDLE hModule)
{
	MessageBox(nullptr, "No configuration needed to be set", "Screen Saver", MB_ICONINFORMATION);
	return FALSE;
}

// Handle messages for the screen saver's configuration dialog box. If the
// screen saver has a configure dialog, this function must be defined. And
// the dialog template in resource file must have DLG_SCRNSAVECONFIGURE identifier.
// Return TRUE if the function successfully process the message.
BOOL WINAPI ScreenSaverConfigureDialog(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
		case WM_INITDIALOG:
			return TRUE;
		case WM_COMMAND:
			break;
	}
	return FALSE;
}

// The screensaver's window procedure, like lpfnWndProc callback in WNDCLASS struct for creating
// normal Windows application. Use DefScreenSaverProc instead of DefWindowProc for default message
// processing. All rendering process will be performed in this function.
LRESULT WINAPI ScreenSaverProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefScreenSaverProc(hWnd, uMsg, wParam, lParam);
}
