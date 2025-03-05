#include "Scene.h"
#include "Camera.h"
#include "GameFramework.h"
//#include "sphere.h"
#include "MainMenu.h"

GameScene::GameScene(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{

    GameObject* gameObject = new MainMenu(device, deviceContext);
	gameObjects.push_back(gameObject);

}

void GameScene::Render(Camera* camera, ID3D11DeviceContext* deviceContext)
{
    if (camera) {
        camera->SetViewportsAndScissorRects(deviceContext);
        camera->UpdateShaderVariables(deviceContext);
    }
    for (auto& obj : gameObjects) {
        obj->Render();
    }
}
