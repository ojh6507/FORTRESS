#include "Scene.h"
#include "Camera.h"
#include "Player.h"
#include "GameFramework.h"
//#include "sphere.h"
#include "MainMenu.h"
#include "IngameManager.h"

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
	//GameObject* gameObject = new _test_concrete_GameObject(device, deviceContext);
	//gameObjects.push_back(gameObject);

    ObjObject* arrowObject2 = new ObjObject(device, deviceContext, L"arrow.obj");
    arrowObject2->SetPostion({ 250,-200,0 });
    arrowObject2->SetRotation({ 90,180,0 });

    
    gameObjects.push_back(arrowObject);
    gameObjects.push_back(arrowObject2);

}
MenuScene::~MenuScene()
{
    for (auto& obj : numbersObject) delete obj;
}
GameScene::GameScene(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	Projectile* ProjectileObject = new Projectile(device, deviceContext);
    ProjectileObject->OutOfScreen();

    gameObjects.push_back(ProjectileObject);
    player = new Player(device, deviceContext, { 0,0,0 });
    Player* playerBody = new Player(device, deviceContext, { 3, 1, 1 });
    PlayerHead* playerHead = new PlayerHead(device, deviceContext, {2, 1, 1}, FVector3(0.0f, 55.0f, 0.0f));
    PlayerBarrel* playerBarrel = new PlayerBarrel(device, deviceContext, {3.f, .3, 1}, FVector3(50.0f, 15.0f, 0.0f));
    
    playerBody->SetChild(playerHead);
    playerBarrel->SetParent(playerHead);

    player->SetChild(playerBody);
    //playerBody->SetParent(player);

    playerHead->SetParent(playerBody);
    playerHead->SetChild(playerBarrel);

    
    playerBody->Reload(ProjectileObject);
    
    IngameManager* ingameManager = IngameManager::Instance();
    gameObjects.push_back(ingameManager);
    ingameManager->players.push_back(playerBody);

}

GameScene::~GameScene()
{
}


void GameScene::Update(double deltaTime)
{
    Scene::Update(deltaTime);
    player->Update(deltaTime);
}

void GameScene::Render(Camera* camera, ID3D11DeviceContext* deviceContext)
{
    Scene::Render(camera, deviceContext);
    player->Render();
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
    XMFLOAT4X4 tempViewMatrix = pCamera->GetViewMatrix();
    XMFLOAT4X4 tempProjectionMatrix = pCamera->GetProjectionMatrix();

    Input::Instance()->GetMouseRay(rayOrigin, rayDir, XMLoadFloat4x4(&tempViewMatrix), XMLoadFloat4x4(&tempProjectionMatrix));

    int pickedIndex = -1;
    float min_dist = FLT_MAX;

    for (int i = 0; i < gameObjects.size(); ++i) {
        float dist = FLT_MAX;
        if (gameObjects[i]->IntersectRay2D(rayOrigin, rayDir, dist) && dist < min_dist) {
            min_dist = dist;
            pickedIndex = i;
        }
    }

    if (pickedIndex != -1) {
        playerCount = (pickedIndex == 1) ? std::clamp((playerCount + 1), 0, 3) : std::clamp((playerCount - 1), 0, 3);
    }
}