#ifndef WINDOW_H
#define WINDOW_H
#include <Windows.h>
#include <string>
// Temp
static bool spacePressed = false;
static bool tabPressed = false;

class Window
{
public:
	// Varying screensize constructor
	Window(
		HINSTANCE hInstance,
		int nCmdShow,
		bool fullScreen = false,
		int screenWidth = 800,
		int screenHeight = 600,
		LPCTSTR windowName = L"windowName",
		LPCTSTR windowTitle = L"windowTitle");

	~Window();

	void SetWindowTitle(std::wstring newTitle);

	bool IsFullScreen() const;
	int GetScreenWidth() const;
	int GetScreenHeight() const;
	const HWND* GetHwnd() const;

	bool ExitWindow();

	// Temp
	bool WasSpacePressed();
	bool WasTabPressed();
private:
	// Window * window;
	bool InitWindow(HINSTANCE hInstance, int nCmdShow);

	int screenWidth;
	int screenHeight;
	bool fullScreen;
	LPCTSTR windowName;
	LPCTSTR windowTitle;

	HWND hwnd;
};

#endif