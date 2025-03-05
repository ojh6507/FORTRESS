#include "Scene.h"
#include "Camera.h"
#include "GameFramework.h"
//#include "sphere.h"

MenuScene::MenuScene(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    //ObjObject* objObject = new ObjObject(device, deviceContext, L"2.obj");
    numbersObject.reserve(4);
    for (int i = 0; i < 4; ++i) {
        std::wstring ws = std::to_wstring(i+1) + L".obj";
        ObjObject* objObject = new ObjObject(device, deviceContext, ws.c_str());
        objObject->SetPostion({ 0,0,0 });
        objObject->SetScale({ 10,10,10 });
        objObject->SetRotation({ 0,180,0 });
        numbersObject.push_back(objObject);
    }

    ObjObject* arrowObject = new ObjObject(device, deviceContext, L"arrow.obj");
    arrowObject->SetPostion({ -250,-200,0 });
    arrowObject->SetRotation({ 90,0,0 });

    ObjObject* arrowObject2 = new ObjObject(device, deviceContext, L"arrow.obj");
    arrowObject2->SetPostion({ 250,-200,0 });
    arrowObject2->SetRotation({ 90,180,0 });
    arrowObject2->UpdateBoundingBox();
    arrowObject->UpdateBoundingBox();
    
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

void MenuScene::PickingObjects(Camera* pCamera)
{
    XMFLOAT3 rayOrigin, rayDir;
    rayOrigin = pCamera->GetPosition();
    XMFLOAT4X4 tempViewMatrix = pCamera->GetViewMatrix();
    XMFLOAT4X4 tempProjectionMatrix = pCamera->GetProjectionMatrix();
    Input::Instance()->GetMouseRay(rayDir, XMLoadFloat4x4(&tempViewMatrix), XMLoadFloat4x4(&tempProjectionMatrix));
    int pickedIndex = -1;
    float min_dist = FLT_MAX;
    float dist = FLT_MAX;
  
    for (int i = 0; i < gameObjects.size(); ++i) {
        if (gameObjects[i]->IntersectRay(rayOrigin, rayDir, dist) && dist < min_dist) {
            min_dist = dist;
            pickedIndex = i;
        }
    }

    if (pickedIndex != -1) {
        std::wstring debugMsg = L"Picked Object Index: " + std::to_wstring(pickedIndex) + L"\n";
        OutputDebugString(debugMsg.c_str());

        playerCount = (pickedIndex == 1) ? std::clamp((playerCount + 1), 0, 3) : std::clamp((playerCount - 1), 0, 3);
    }
}

