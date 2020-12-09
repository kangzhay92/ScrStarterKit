#include "framework.h"
#include "screensaver.h"
#ifndef _DEBUG
#include <ScrnSave.h>
#endif

#ifdef _MSC_VER
#ifndef _DEBUG
#pragma comment(lib, "scrnsave.lib")   // Microsoft Visual C++ library for creating Screen saver application
#endif
#pragma comment(lib, "comctl32.lib")   // Windows Common Control Library, for the controls on configure dialog
#endif

static Application app;
static Configuration config;

// Use this function if you want to register new window class for the configure
// dialog, with the hModule parameter as HINSTANCE in CreateWindow function.
// Must be return TRUE if screen saver has configure dialog. FALSE otherwise
BOOL WINAPI RegisterDialogClasses(HANDLE hModule)
{
	return TRUE;
}

// Handle messages for the screen saver's configuration dialog box. The dialog 
// template in resource file must have DLG_SCRNSAVECONFIGURE identifier.
// Return TRUE if the function successfully process the message.
BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
		case WM_INITDIALOG: {
			// Initialize the configuration
			config = Configuration();

			// Set range of the sliders
			SendDlgItemMessage(hDlg, IDC_SLIDER_NUMOFSTARS, TBM_SETRANGE, FALSE, MAKELPARAM(50, 1000));
			SendDlgItemMessage(hDlg, IDC_SLIDER_INTERVAL,   TBM_SETRANGE, FALSE, MAKELPARAM(1, 100));
			
			// Set position of the sliders
			SendDlgItemMessage(hDlg, IDC_SLIDER_NUMOFSTARS, TBM_SETPOS, TRUE, config.numStars);
			SendDlgItemMessage(hDlg, IDC_SLIDER_INTERVAL,   TBM_SETPOS, TRUE, config.timerInterval);
			
			// Set value of the slider texts
			SetDlgItemInt(hDlg, IDC_TEXT_NUMOFSTARS, config.numStars,      FALSE);
			SetDlgItemInt(hDlg, IDC_TEXT_INTERVAL,   config.timerInterval, FALSE);

			return TRUE;
		}
		case WM_HSCROLL: {
			// Link slider texts value with sliders
			int numStars = SendDlgItemMessage(hDlg, IDC_SLIDER_NUMOFSTARS, TBM_GETPOS, 0, 0);
			int interval = SendDlgItemMessage(hDlg, IDC_SLIDER_INTERVAL,   TBM_GETPOS, 0, 0);
			SetDlgItemInt(hDlg, IDC_TEXT_NUMOFSTARS, numStars, FALSE);
			SetDlgItemInt(hDlg, IDC_TEXT_INTERVAL, interval, FALSE);
			return TRUE;
		}
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_BUTTON_DEFAULT:
					// Reset all configurations to default value
					SendDlgItemMessage(hDlg, IDC_SLIDER_NUMOFSTARS, TBM_SETPOS, TRUE, 250);
					SendDlgItemMessage(hDlg, IDC_SLIDER_INTERVAL,   TBM_SETPOS, TRUE, 20);
					SetDlgItemInt(hDlg, IDC_TEXT_NUMOFSTARS, 250, FALSE);
					SetDlgItemInt(hDlg, IDC_TEXT_INTERVAL,   20,  FALSE);
					return TRUE;
				case IDOK:
					config.numStars      = SendDlgItemMessage(hDlg, IDC_SLIDER_NUMOFSTARS, TBM_GETPOS, 0, 0);
					config.timerInterval = SendDlgItemMessage(hDlg, IDC_SLIDER_INTERVAL,   TBM_GETPOS, 0, 0);
					config.Commit();
					// Pass through to next case
				case IDCANCEL:
					EndDialog(hDlg, LOWORD(wParam));
					return TRUE;
			}
			break;
	}
	return FALSE;
}

// The screensaver's window procedure, like lpfnWndProc callback in WNDCLASS struct for creating
// normal Windows application. Use DefScreenSaverProc instead of DefWindowProc for default message
// processing. All rendering process will be processed in this function.
LRESULT WINAPI ScreenSaverProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
		case WM_CREATE:
			app = Application(hWnd, &config);
			break;
		case WM_DESTROY:
			app.Deinitialize();
			break;
		case WM_SIZE:
			app.SetCenterPosition(LOWORD(lParam), HIWORD(lParam));
			break;
		// WM_PAINT case can be removed if you not using GDI renderer
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;
		}
		// WM_TIMER case can be removed if you use high resolution timer (QPC and QPF combination)
		case WM_TIMER:
			if (wParam == Application::TIMER_ID) {
				app.Loop();
			}
			break;
		default:
#ifdef _DEBUG
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
#else
			return DefScreenSaverProc(hWnd, uMsg, wParam, lParam);
#endif
	}

	return 0;
}

// Create a windowed screen saver, since output .scr file is not debugable (will stuck
// on the main screen and freezing if breakpoint was triggered).
#ifdef _DEBUG
constexpr uint32_t IDM_CONFIGURE = 11001;
constexpr uint32_t IDM_EXIT = 11002;

static LRESULT WINAPI DebugScreenSaverProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
		case WM_CREATE: {
			HMENU hMenuBar = CreateMenu();
			HMENU hFileMenu = CreatePopupMenu();
			AppendMenu(hFileMenu, MF_STRING, IDM_CONFIGURE, "&Configure");
			AppendMenu(hFileMenu, MF_SEPARATOR, 0, nullptr);
			AppendMenu(hFileMenu, MF_STRING, IDM_EXIT, "&Exit");
			AppendMenu(hMenuBar, MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(hFileMenu), "&File");
			SetMenu(hWnd, hMenuBar);
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDM_CONFIGURE: {
					HINSTANCE hInst = GetModuleHandle(nullptr);
					if (!RegisterDialogClasses(hInst)) {
						break;
					}
					// TODO Use GetWindowLongPtr instead, and pass app as a pointer?
					app.Pause();
					DialogBox(hInst, MAKEINTRESOURCE(DLG_SCRNSAVECONFIGURE), hWnd, ScreenSaverConfigureDialog);
					app.Unpause();
					app.Reload();
					break;
				}
				case IDM_EXIT:
					SendMessage(hWnd, WM_CLOSE, 0, 0);
					break;
			}
			break;
	}
	return ScreenSaverProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdCount)
{
	InitCommonControls();

	constexpr LPCSTR WINDOW_CLASS = "DebugScreenSaverWindow";
	WNDCLASS wc = {};
	wc.hCursor = LoadCursor(hInst, IDC_ARROW);
	wc.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SCREENSAVER));
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = WINDOW_CLASS;
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wc.hInstance = hInst;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = DebugScreenSaverProc;
	
	// Set window size here, and window position at center of screen
	constexpr UINT STYLE = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE;
	constexpr UINT EX_STYLE = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	constexpr int WIDTH = 800;
	constexpr int HEIGHT = 600;
	const int x = (GetSystemMetrics(SM_CXSCREEN) - WIDTH) / 2;
	const int y = (GetSystemMetrics(SM_CYSCREEN) - HEIGHT) / 2;

	// Lazy window initialization (no error handling)
	RegisterClass(&wc);
	CreateWindowEx(EX_STYLE, WINDOW_CLASS, "Screen Saver Preview", STYLE, x, y, WIDTH, HEIGHT, nullptr, nullptr, hInst, nullptr);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}
#endif
