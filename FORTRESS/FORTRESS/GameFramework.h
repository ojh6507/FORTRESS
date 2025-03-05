#pragma once
#include "stdafx.h"
#include "Graphics.h"
#include "Scene.h"
#include "Camera.h"

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

	Input* input;

private:
	Camera* camera;
	Graphics* graphics;
	SceneManager* sceneManager;
};

