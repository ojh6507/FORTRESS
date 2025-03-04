#pragma once
#include "stdafx.h"
class GameFramework
{
public:
	GameFramework() {

	}
	~GameFramework(){}
	void FrameAdvance();
	void OnDestroy();
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:

};

