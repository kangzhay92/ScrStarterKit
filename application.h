#pragma once

class Configuration;

struct Star
{
	int xPos, yPos, zPos;
	int lastX, lastY;
};

// Class for handling screensaver rendering logic. This class
// using GDI, but you can use another graphics renderer too
// like OpenGL or Direct3D. This class must be initialized on
// WM_CREATE message.
class Application
{
public:
	static constexpr int TIMER_ID = 1;

	Application() {}
	Application(HWND hWnd, Configuration* config);
	
	void Initialize();    // Initialize screen saver, called in constructor by default
	void Deinitialize();  // Deinitialize screen saver, must be called in WM_DESTROY message
	void Loop();          // Main screen saver loop, called every timer ticks

	// Rendering specific methods
	void SetCenterPosition(int x, int y);

	// Called by DebugScreenSaverProc on debug mode
#ifdef _DEBUG
	void Reload()  { Deinitialize(); Initialize(); }
	void Pause()   { running = false; }
	void Unpause() { running = true;  }
#endif

private:
	// Since screen saver always running until users interrupt it,
	// this variable is useless in release mode
#ifdef _DEBUG
	bool running;
#endif

	// This methods called in main loop
	void Update();
	void Draw(HDC hdc);
	
	// Framework specific variables: 
	// Screen saver config, Native window handle, and Random number generator
	Configuration* config;
	HWND hWnd;
	std::mt19937_64 rndGen;

	// Rendering specific variables
	uint32_t centerX, centerY;
	std::vector<Star> stars;
	static constexpr COLORREF BLACK = RGB(0, 0, 0);
	static constexpr COLORREF WHITE = RGB(255, 255, 255);
};
