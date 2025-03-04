#include "GameFramework.h"

void GameFramework::FrameAdvance()
{
	Input::Instance()->Frame();
	sceneManager->Top()->Update();
	graphics->RenderBegin();
	sceneManager->Top()->Render();
	graphics->RenderEnd();
}

void GameFramework::OnDestroy()
{
	delete sceneManager;
	delete graphics;
}

bool GameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	graphics = new Graphics(hMainWnd);
	sceneManager = new SceneManager();
	GameScene* gameScene = new GameScene(graphics->GetDevice(), graphics->GetDeviceContext());
	sceneManager->PushScene(gameScene);
	return true;
}

LRESULT GameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return LRESULT();
}
