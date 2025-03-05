#include "Scene.h"
#include "GameFramework.h"
//#include "sphere.h"

GameScene::GameScene(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{

	//for (int i{}; i < 1; ++i) {

	//}
	GameObject* gameObject = new _test_concrete_GameObject(device, deviceContext);
	gameObjects.push_back(gameObject);

}
