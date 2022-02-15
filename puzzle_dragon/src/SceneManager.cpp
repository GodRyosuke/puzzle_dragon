#include"SceneManager.hpp"
#include "CommonData.hpp"

#include <iostream>
class Title;
//class Game;

SceneManager::SceneManager()
    :Scene(new CommonData()),
    mScene(makeScene<Title>())
{
}



SceneManager::~SceneManager() {
    delete mScene;
    delete mCommonData;
}

void SceneManager::SceneManagerUpdate() {
    Scene* p = mScene->update();
    if (p == NULL) {    // ƒQ[ƒ€I—¹
        delete mScene;
        exit(0);
    }
    if (p != mScene) {
        delete mScene;
        mScene = p;
    }
}

void SceneManager::draw() {
    mScene->draw();
}

void SceneManager::input()
{
    mScene->input();
}