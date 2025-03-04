#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

// https://blog.nullbus.net/24
// https://hwan-shell.tistory.com/227

class Input {
private:
	static Input* singleton;
	Input() {};
	Input(const Input& ref) {};
	Input& operator=(const Input& ref) {};
	~Input() {};
public:
	static Input* Instance() {
		if (!singleton) {
			singleton = new Input();
		}
		return singleton;
	}
	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	bool IsKeyPressed(char keycode);
	void GetMouseLocation(int& mouse_x, int& mouse_y);

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

	IDirectInput8* m_directInput = nullptr;
	IDirectInputDevice8* m_keyboard = nullptr;
	IDirectInputDevice8* m_mouse = nullptr;

	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;

	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;
};
