//#pragma once

#include"SceneManager.hpp"
#include "CommonData.hpp"

#include <fstream>
class Title;
//class Game;

SceneManager::SceneManager()
    :Scene(new CommonData())
{
}


SceneManager::~SceneManager() {
    delete mScene;
    delete mCommonData;
}

bool SceneManager::Initialize()
{
	mCommonData->WINDOW_WIDTH = 768;
	mCommonData->WINDOW_HEIGHT = 640;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	mCommonData->mWindow = SDL_CreateWindow("Puzzle and Dragons ??", 100, 100, mCommonData->WINDOW_WIDTH, mCommonData->WINDOW_HEIGHT, 0);
	if (!mCommonData->mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	mCommonData->mRenderer = SDL_CreateRenderer(mCommonData->mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!mCommonData->mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	int flags = IMG_INIT_JPG | IMG_INIT_PNG;
	int initted = IMG_Init(flags);
	if ((initted & flags) != flags) {
		printf("IMG_Init: Failed to init required jpg and png support!\n");
		std::string error = IMG_GetError();
		SDL_Log("IMG_Init: %s\n", IMG_GetError());

	}

	TTF_Init();

	// サウンド関連
	void* extraDriverData = NULL;
	Common_Init(&extraDriverData);

	mCommonData->mAudioSystem = NULL;
	ERRCHECK(FMOD::Studio::System::create(&mCommonData->mAudioSystem));

	// The example Studio project is authored for 5.1 sound, so set up the system output mode to match
	FMOD::System* coreSystem = NULL;
	ERRCHECK(mCommonData->mAudioSystem->getCoreSystem(&coreSystem));
	ERRCHECK(coreSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_5POINT1, 0));

	ERRCHECK(mCommonData->mAudioSystem->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, extraDriverData));

	if (!LoadData()) {
		printf("error: failed to load data @ Scene Manager");
		return false;
	}


	// 最初のSceneを設定
	mScene = makeScene<Title>();

    return true;
}

bool SceneManager::LoadData()
{
	// Font読み込み
	mCommonData->mFont = TTF_OpenFont(".\\resources\\VL-Gothic-Regular.ttf", 128);

	// 日本語テキストデータ読み込み
	{
		std::string filePath = ".\\resources\\GameText.json";
		std::ifstream ifs(filePath.c_str());
		if (ifs.good()) {
			ifs >> mCommonData->mTextData;
		}
		else {
			printf("error: failed to load text lang data\n");
			return false;
		}
	}


	// デフォルトは英語
	mCommonData->mLangType = mCommonData->ENGLISH;


	// Load Master Bank
	FMOD::Studio::Bank* masterBank = NULL;
	ERRCHECK(mCommonData->mAudioSystem->loadBankFile(Common_MediaPath(".\\resources\\Master.bank"), FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank));

	FMOD::Studio::Bank* stringsBank = NULL;
	ERRCHECK(mCommonData->mAudioSystem->loadBankFile(Common_MediaPath(".\\resources\\Master.strings.bank"), FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank));


	return true;
}

bool SceneManager::SceneManagerUpdate() {
    Scene* p = mScene->update();
    if (p == NULL) {    // ゲーム終了
        return false;
    }
    if (p != mScene) {
        delete mScene;
        mScene = p;
    }

    return true;
}



void SceneManager::draw() {
    mScene->draw();
}

void SceneManager::input()
{
    mScene->input();
}

void SceneManager::shutdown()
{
    mScene->shutdown();
    delete mScene;
}