#include "framework.h"
#include "screensaver.h"

Application::Application(HWND hWnd, Configuration* config)
	: hWnd(hWnd), config(config)
{
#ifdef _DEBUG
	running = true;
#endif

	// Initialize random number generator
	rndGen.seed(std::chrono::steady_clock::now()
		.time_since_epoch()
		.count());

	// Initialize stars and timer
	Initialize();
}

void Application::Deinitialize()
{
	KillTimer(hWnd, TIMER_ID);
}

void Application::Initialize()
{
	SetTimer(hWnd, TIMER_ID, config->timerInterval, nullptr);

	// Initialize stars array
	stars.clear();
	stars.resize(config->numStars);

	// The random number distribution
	auto xDist = std::uniform_int_distribution<>(-320, 320);
	auto yDist = std::uniform_int_distribution<>(-200, 200);

	// Place stars at random position on the screen
	for (size_t i = 0; i < stars.size(); i++) {
		auto& star = stars[i];
		do {
			star.xPos = xDist(rndGen);
			star.yPos = yDist(rndGen);
			star.zPos = i + 1;
			star.lastX = -1;
			star.lastY = -1;
		} while ((star.xPos == 0) || (star.yPos == 0));
	}

	// Clear last rendered screen (in debug mode)
	InvalidateRect(hWnd, nullptr, TRUE);
}

void Application::SetCenterPosition(int x, int y)
{
	centerX = x / 2;
	centerY = y / 2;
}

void Application::Draw(HDC hDC)
{
	for (auto& star : stars) {
		// Simulate 3D rendering on 2D screen
		SetPixel(hDC, star.lastX, star.lastY, BLACK);
		int nx = static_cast<int>(((static_cast<long>(star.xPos << 7)) / static_cast<long>(star.zPos)) + centerX);
		int ny = static_cast<int>(((static_cast<long>(star.yPos << 7)) / static_cast<long>(star.zPos)) + centerY);
		SetPixel(hDC, nx, ny, WHITE);

		// Set last position of the stars
		star.lastX = nx;
		star.lastY = ny;
	}
}

void Application::Update()
{
	for (auto& star : stars) {
		star.zPos -= 3;
		if (star.zPos > config->numStars) {
			star.zPos -= config->numStars;
		}
		if (star.zPos < 1) {
			star.zPos += config->numStars;
		}
	}
}

void Application::Loop()
{
#ifdef _DEBUG
	if (running) {
#endif
		Update();
		HDC hdc = GetDC(hWnd);
		Draw(hdc);
		ReleaseDC(hWnd, hdc);
#ifdef _DEBUG
	}
#endif
}
