#include "GameFramework.h"

void GameFramework::FrameAdvance(double deltaTime)
{
	Input::Instance()->Frame();
	guiController->NewFrame();
	sceneManager->Top()->Update(deltaTime);
	graphics->RenderBegin();
	sceneManager->Top()->Render(camera, graphics->GetDeviceContext());
	guiController->RenderFrame();
	graphics->RenderEnd();
}

void GameFramework::OnDestroy()
{
	delete sceneManager;
	delete graphics;
	delete guiController;
}

bool GameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	graphics = new Graphics(hMainWnd);
	sceneManager = new SceneManager();
	GameScene* gameScene = new GameScene(graphics->GetDevice(), graphics->GetDeviceContext());
	sceneManager->PushScene(gameScene);
	guiController = new GuiController(hMainWnd, graphics);

	camera = new Camera();
	camera->GenerateProjectionMatrix(0.1f, 100.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

	camera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	camera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	camera->CreateShaderVariables(graphics->GetDevice(), graphics->GetDeviceContext());
	camera->SetPosition(XMFLOAT3(0, 0, -1));
	camera->ResetOrientationVectors();
	return true;
}

LRESULT GameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return LRESULT();
}
