#include "Input.h"

Input& Input::GetInstance()
{
	static Input instance;

	return instance;
}

void Input::RegisterDevices(const HWND* hWnd)
{
	static RAWINPUTDEVICE m_Rid[2];

	// Register mouse
	m_Rid[0].usUsagePage = 0x01;
	m_Rid[0].usUsage = 0x02;
	m_Rid[0].dwFlags = 0;
	m_Rid[0].hwndTarget = *hWnd;

	// Register keyboard
	m_Rid[1].usUsagePage = 0x01;
	m_Rid[1].usUsage = 0x06;
	m_Rid[1].dwFlags = 0;
	m_Rid[1].hwndTarget = *hWnd;

	if (RegisterRawInputDevices(m_Rid, 2, sizeof(m_Rid[0])) == FALSE)
	{
		Log::Print("Device registration error: %f\n", GetLastError());
	}
	else
	{
		Log::Print("Input devices registered!\n");
	}
}

void Input::SetKeyState(SCAN_CODES key, bool pressed)
{
	SetJustPressed(key, !m_KeyState[key]);
	m_KeyState[key] = pressed;
}

void Input::SetJustPressed(SCAN_CODES key, bool justPressed)
{
	m_JustPressed[key] = justPressed;
}

void Input::SetMouseButtonState(MOUSE_BUTTON button, bool pressed)
{
	m_MouseButtonState[button] = pressed;
}

void Input::SetMouseScroll(SHORT scroll)
{
	m_Scroll = (scroll > 0) * 2 - 1;
}

void Input::SetMouseMovement(int x, int y)
{
	m_MouseMovement = std::make_pair(x, y);
}

bool Input::GetKeyState(SCAN_CODES key)
{
	return m_KeyState[key];
}

bool Input::GetJustPressed(SCAN_CODES key)
{
	bool toReturn = m_JustPressed[key];
	m_JustPressed[key] = false;
	return toReturn;
}

bool Input::GetMouseButtonState(MOUSE_BUTTON button)
{
	return m_MouseButtonState[button];
}

int Input::GetMouseScroll()
{
	int toReturn = m_Scroll;
	m_Scroll = 0;
	return toReturn;
}

std::pair<int, int> Input::GetMouseMovement()
{
	return m_MouseMovement;
}

Input::Input()
{
}