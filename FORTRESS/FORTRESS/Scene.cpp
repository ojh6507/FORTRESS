#include "Scene.h"
#include "Camera.h"
#include "GameFramework.h"
//#include "sphere.h"

GameScene::GameScene(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    GameObject* gameObject = new GameObject(device, deviceContext);
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
