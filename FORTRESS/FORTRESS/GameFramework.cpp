#include "GameFramework.h"

void GameFramework::FrameAdvance(double deltaTime)
{
	Input::Instance()->Frame();
	if (Input::Instance()->IsMouseButtonReleased(0)) {
		sceneManager->Top()->PickingObjects(camera);
	}
	guiController->NewFrame();

	if (Input::Instance()->IsKeyReleased(DIK_1)) {
		GameScene* gameScene = new GameScene(graphics->GetDevice(), graphics->GetDeviceContext());
		sceneManager->ChangeScene(gameScene);
	}
	if (Input::Instance()->IsKeyReleased(DIK_2)) {
		MenuScene* menuScene = new MenuScene(graphics->GetDevice(), graphics->GetDeviceContext());
		sceneManager->ChangeScene(menuScene);
	}	

	
	//sceneManager->Top()->Update(deltaTime);

	graphics->RenderBegin();
	sceneManager->Top()->Update(deltaTime);
	guiController->RenderFrame();
	sceneManager->Top()->Render(camera, graphics->GetDeviceContext());
	graphics->RenderEnd();


}

void GameFramework::OnDestroy()
{
	delete sceneManager;
	delete camera;
	delete graphics;
	delete guiController;
}

bool GameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	graphics = new Graphics(hMainWnd);
	sceneManager = new SceneManager();
	MenuScene* menuScene = new MenuScene(graphics->GetDevice(), graphics->GetDeviceContext());

	sceneManager->PushScene(menuScene);
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
