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

    virtual void Render(Camera* camera, ID3D11DeviceContext* deviceContext) {};

    virtual void Update(double deltaTime) {
        for (auto& obj : gameObjects) {
            obj->Update(deltaTime);
        }
    }

protected:
    std::vector<GameObject*> gameObjects;
};

class MenuScene : public Scene {
public:
    
    virtual void Render(Camera* camera, ID3D11DeviceContext* deviceContext) override {};
};

class GameScene : public Scene {
public:
    GameScene(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
    virtual void Render(Camera* camera, ID3D11DeviceContext* deviceContext) override;
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
