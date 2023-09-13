#pragma once

#include "Scene.hpp"

class SceneManager : public Scene {
public:
    SceneManager();
    ~SceneManager();
private:
    Scene* mScene;

public:
    bool Initialize();
    bool LoadData(); 

    bool SceneManagerUpdate();
    void draw();
    void input();
    void shutdown();

    Scene* update() override {
        return mScene;
    };
};