#include "Scene.h"
#include "Camera.h"
#include "GameFramework.h"
//#include "sphere.h"
#include "MainMenu.h"

MenuScene::MenuScene(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    //ObjObject* objObject = new ObjObject(device, deviceContext, L"2.obj");
    numbersObject.reserve(4);
    for (int i = 0; i < 3; ++i) {
        std::wstring ws = std::to_wstring(i+1) + L".obj";
        ObjObject* objObject = new ObjObject(device, deviceContext, ws.c_str());
        objObject->SetPostion({ 0,0,0 });
        objObject->SetScale({ 10,10,10 });
        objObject->SetRotation({ 0,180,0 });
        numbersObject.push_back(objObject);
    }

    ObjObject* arrowObject = new ObjObject(device, deviceContext, L"arrow.obj");
    arrowObject->SetPostion({ -200,-200,0 });
    arrowObject->SetRotation({ 90,0,0 });

    ObjObject* arrowObject2 = new ObjObject(device, deviceContext, L"arrow.obj");
    arrowObject2->SetPostion({ 200,-200,0 });
    arrowObject2->SetRotation({ 90,180,0 });
    
    gameObjects.push_back(arrowObject);
    gameObjects.push_back(arrowObject2);
}
GameScene::GameScene(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    //CubeObject* gameObject = new CubeObject(device, deviceContext);
	Projectile* ProjectileObject = new Projectile(device, deviceContext);
    gameObjects.push_back(ProjectileObject);

}
void Scene::Render(Camera* camera, ID3D11DeviceContext* deviceContext)
{
    if (camera) {
        camera->SetViewportsAndScissorRects(deviceContext);
        camera->UpdateShaderVariables(deviceContext);
    }
    for (auto& obj : gameObjects) {
        obj->Render();
    }
}

void MenuScene::Render(Camera* camera, ID3D11DeviceContext* deviceContext)
{
    Scene::Render(camera, deviceContext);
    numbersObject[playerCount]->Render();
}

void MenuScene::Update(double deltaTime)
{
    Scene::Update(deltaTime);
}

