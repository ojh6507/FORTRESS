#include "Scene.h"
#include "Camera.h"
#include "Player.h"
#include "PlayerUI.h"
#include "GameFramework.h"
//#include "sphere.h"
#include "MainMenu.h"
#include "IngameManager.h"

MenuScene::MenuScene(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    //ObjObject* objObject = new ObjObject(device, deviceContext, L"2.obj");
    numbersObject.reserve(2);
    for (int i = 0; i < 2; ++i) {
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
    Projectile* ProjectileObject1 = new Projectile(device, deviceContext);
    ProjectileObject1->OutOfScreen();
    gameObjects.push_back(ProjectileObject1);

    FVector3 playerColor = { 1.f, 0.4f, 0.4f };
    player1 = new Player(device, deviceContext, { 0,0,0 }, playerColor);
    Player* playerBody = new Player(device, deviceContext, { 2.3, 0.7, 1 }, playerColor);
    PlayerHead* playerHead = new PlayerHead(device, deviceContext, { 1.4, 1, 1 }, FVector3(0.0f, 30.0f, 0.0f), playerColor);
    PlayerBarrel* playerBarrel = new PlayerBarrel(device, deviceContext, { 2.4f, .3, 1 }, FVector3(50.0f, 15.0f, 0.0f), playerColor);
    PlayerFirePoint* playerFirePoint = new PlayerFirePoint(device, deviceContext, { 0.2f, .4, 1 }, FVector3(80.0f, 0.0f, 0.0f), playerColor);

    playerBody->SetChild(playerHead);
    playerBarrel->SetParent(playerHead);

    player1->SetChild(playerBody);
    playerBody->_shooter = player1;

    playerHead->SetParent(playerBody);
    playerHead->SetChild(playerBarrel);
    playerBarrel->SetChild(playerFirePoint);
    playerFirePoint->SetParent(playerBarrel);
    player1->Reload(ProjectileObject1);
    player1->SetFirePoint(playerFirePoint);
    player1->SetPosition({ -500,0, 1 });

    
    player1->Reload(ProjectileObject1);

    // Spawn Player2
    FVector3 player2Color = { 0.f, 1.f, 0.0f };
    player2 = new Player(device, deviceContext, { 0, 0, 0 }, player2Color);


    // �ٵ�, ���, �跲, ���̾�����Ʈ �� ���� ���� ����
    Player* player2Body = new Player(device, deviceContext, { 2.3f, 0.7f, 1.0f },
        player2Color);
    PlayerHead* player2Head = new PlayerHead(device, deviceContext, { 1.4f, 1.0f, 1.0f },
        FVector3(0.0f, 30.0f, 0.0f),
        player2Color);

    PlayerBarrel* player2Barrel = new PlayerBarrel(device, deviceContext, { 2.4f, 0.3f, 1.0f },
        FVector3(-50.0f, 15.0f, 0.0f),
        player2Color);
    PlayerFirePoint* player2FirePoint = new PlayerFirePoint(device, deviceContext, { 0.2f, 0.4f, 1.0f },
        FVector3(-90.0f, 0.0f, 0.0f),
        player2Color);

    Projectile* ProjectileObject2 = new Projectile(device, deviceContext);
    ProjectileObject2->OutOfScreen();

    gameObjects.push_back(ProjectileObject2);

    player2Body->SetChild(player2Head);
    player2Barrel->SetParent(player2Head);

    player2->SetChild(player2Body);
    player2Body->_shooter = player2;

    player2Head->SetParent(player2Body);
    player2Head->SetChild(player2Barrel);
    player2Barrel->SetChild(player2FirePoint);
    player2FirePoint->SetParent(player2Barrel);
    player2->Reload(ProjectileObject2);
    player2->SetFirePoint(player2FirePoint);

    player2->SetPosition({ 500,0, 1 });
    player2->SetDir(-1);

    terrain = new Terrain(device, deviceContext, 2000, 400, 1);
    
    IngameManager* ingameManager = IngameManager::Instance();

    gameObjects.push_back(ingameManager);
    ingameManager->players.push_back(playerBody);
    ingameManager->players.push_back(player2Body);


    CubeObject* p = new CubeObject(device, deviceContext, { 1,1,1 });
    gameObjects.push_back(p);

    // Player UI
    player1_UI = new PlayerUI(device, deviceContext, 0.0f, 1, player1);
    player2_UI = new PlayerUI(device, deviceContext, 0.0f, 2, player2);
}

GameScene::~GameScene()
{
}


void GameScene::Update(double deltaTime)
{
    Scene::Update(deltaTime);
    player1->Update(deltaTime);
    player2->Update(deltaTime);

    player1_UI->Update(deltaTime);
    player2_UI->Update(deltaTime);

    if (Input::Instance()->IsKeyPressed(DIK_O))
        player1->TakeDamage(10, FVector3(-10.0f, 8.0f, 0.0f));
    if (Input::Instance()->IsKeyPressed(DIK_I))
        player1->SuccessHitEnemy();

    if (Input::Instance()->IsKeyPressed(DIK_L))
        player2->TakeDamage(10, FVector3(-10.0f, 8.0f, 0.0f));
    if (Input::Instance()->IsKeyPressed(DIK_K))
        player2->SuccessHitEnemy();

}

void GameScene::Render(Camera* camera, ID3D11DeviceContext* deviceContext)
{
    Scene::Render(camera, deviceContext);
    player1->Render();
    player2->Render();
    terrain->Render();
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