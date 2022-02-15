#pragma once

#include "Scene.hpp"

class SceneManager : public Scene {
public:
    SceneManager();
    //SceneManager(const SceneManager&) = delete;
    //SceneManager& operator=(const SceneManager&) = delete;
    ~SceneManager();
private:
    Scene* mScene;
    //CommonData* const mCommonData;

public:
    void SceneManagerUpdate();
    Scene* update() override {
        return mScene;
    };
    void draw();
    void input();
};