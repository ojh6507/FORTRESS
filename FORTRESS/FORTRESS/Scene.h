#pragma once
#include "stdafx.h"
#include "GameObject.h"
#include <stack>
class Camera;
class GameFramework;
class Scene {
public:
    Scene() {}
    Scene(ID3D11Device* device, ID3D11DeviceContext* deviceContext) {}
    virtual ~Scene() { Destroy(); }

    virtual void Destroy() {
        for (auto obj : gameObjects) {
            delete obj;
        }
        gameObjects.clear();
    }

    virtual void Render(Camera* camera, ID3D11DeviceContext* deviceContext);

    virtual void Update(double deltaTime) {
        for (auto& obj : gameObjects) {
            obj->Update(deltaTime);
        }
    }
    void SetPlayerCount(int cnt) {
        playerCount = cnt % 2 + 1;
    }
protected:
    std::vector<GameObject*> gameObjects;
    int playerCount = 0;
};

class MenuScene : public Scene {
public:
    
    MenuScene(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
    virtual void Render(Camera* camera, ID3D11DeviceContext* deviceContext);
    virtual void Update(double deltaTime);
private:
    std::vector<GameObject*> numbersObject;
};

class GameScene : public Scene {
public:
    GameScene(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
   
};

class SceneManager {
public:
    SceneManager() {}
    ~SceneManager() { ClearScenes(); }

    void PushScene(Scene* newScene) {
        sceneStack.push(newScene);
    }

    void PopScene() {
        if (!sceneStack.empty()) {
            sceneStack.pop();
        }
    }

    void ChangeScene(Scene* newScene) {
        ClearScenes();
        PushScene(newScene);
    }

    Scene* Top() {
        return sceneStack.empty() ? nullptr : sceneStack.top();
    }

    bool IsEmpty() const {
        return sceneStack.empty();
    }

    void ClearScenes() {
        while (!sceneStack.empty()) {
            sceneStack.pop();
        }
    }

private:
    std::stack<Scene*> sceneStack;
};
