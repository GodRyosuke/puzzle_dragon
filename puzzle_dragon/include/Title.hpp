#pragma once

#include <stdlib.h>
#include <iostream>
#include"Scene.hpp"
#include "CommonData.hpp"

class Title : public Scene {
public:
	Title(CommonData* const commonData);
	virtual ~Title();
	virtual Scene* update() override;
	virtual void draw() override;
	virtual void input() override;
	virtual void shutdown() override;

private:
	bool Initialize();
	bool LoadData();	// ゲームで使用するデータのロード
	void UnloadData();

	SDL_Texture* mTitleTexture;
	bool mIsRunning;
	bool mGotoGame;

	FMOD::Studio::EventInstance* mBackMusic;

	std::vector<std::string> mTitleText;

	unsigned int mFrameNum;
	bool IsAlphaIncreasing;
	int mTitleTextAlpha;
};

template<>
Scene* Scene::makeScene<Title>() {
	return new Title(mCommonData);
}