//#pragma once

#include "Title.hpp"

#include <conio.h>
#include <fstream>


class Game;


Title::Title(CommonData* const commonData)
	:Scene(commonData),
	mIsRunning(true),
	mGotoGame(false),
	mFrameNum(0),
	mTitleTextAlpha(255),
	IsAlphaIncreasing(false)
{
	if (!Initialize()) {
		printf("error: Failed to initialize Title Scene\n");
		exit(-1);
	}

	ERRCHECK(mBackMusic->start());
}


Title::~Title()
{
	UnloadData();
}

bool Title::Initialize()
{
	if (!LoadData()) {
		printf("error: failed to load data @ Title\n");
		return false;
	}

	return true;
}


static void DrawText(std::string text_data, int x, int y, int* color, TTF_Font* font, SDL_Renderer* renderer, double scale = 1.0)
{
	SDL_Color color_data;
	color_data.r = color[0];
	color_data.g = color[1];
	color_data.b = color[2];
	color_data.a = (Uint8)color[3];

	//j["intrinsics"]["cx"]
	//auto t = mTextData["hello"]["ja"].value();
	//std::stringstream ss;
	//ss << mTextData["hello"]["ja"];
	//SDL_Surface* font_surface = TTF_RenderUTF8_Blended(font, ss.str().c_str(), blue);


	SDL_Surface* font_surface = TTF_RenderUTF8_Blended(font, text_data.c_str(), color_data);

	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, font_surface);
	SDL_FreeSurface(font_surface);
	if (!tex)
	{
		SDL_Log("error: failed to load font");
		exit(1);
	}

	int tex_width;
	int tex_height;
	SDL_QueryTexture(tex, nullptr, nullptr, &tex_width, &tex_height);

	SDL_Rect rect;
	rect.w = tex_width * scale;
	rect.h = tex_height * scale;
	rect.x = x - rect.w / 2;
	rect.y = y - rect.h / 2;
	SDL_RenderCopyEx(renderer,
		tex,
		nullptr,
		&rect,
		0,
		nullptr,
		SDL_FLIP_NONE);
}


bool Title::LoadData()
{
	// タイトルの背景画像読み込み
	{
		SDL_Texture* tex = nullptr;
		std::string filePath = ".\\resources\\title_bg.jpg";
		// Load from file
		SDL_Surface* surf = IMG_Load(filePath.c_str());
		if (!surf){
			SDL_Log("Failed to load texture file %s", filePath.c_str());
			return false;
		}


		// Create texture from surface
		tex = SDL_CreateTextureFromSurface(mCommonData->mRenderer, surf);
		SDL_FreeSurface(surf);
		if (!tex){
			SDL_Log("Failed to convert surface to texture for %s", filePath.c_str());
			return false;
		}
		mTitleTexture = tex;
	}

	// タイトル本文読み込み
	mTitleText.resize(mCommonData->NUM_LANG_TYPE);
	{
		std::string str;
		mCommonData->mTextData["Title"]["TitleText"]["ja"].get_to(str);
		mTitleText[mCommonData->JAPANESE] = str;
	}
	{
		std::string str;
		mCommonData->mTextData["Title"]["TitleText"]["en"].get_to(str);
		mTitleText[mCommonData->ENGLISH] = str;
	}

	// サウンド関連
	// Load Title Music
	FMOD::Studio::EventDescription* BackMusicDesc = NULL;
	ERRCHECK(mCommonData->mAudioSystem->getEvent("event:/Title/back_bgm", &BackMusicDesc));
	mBackMusic = NULL;
	ERRCHECK(BackMusicDesc->createInstance(&mBackMusic));

	return true;
}

void Title::shutdown()
{
	IMG_Quit();
	SDL_DestroyRenderer(mCommonData->mRenderer);
	SDL_DestroyWindow(mCommonData->mWindow);
	SDL_Quit();
}

void Title::UnloadData()
{
	SDL_DestroyTexture(mTitleTexture);
}

Scene* Title::update()
{
	mFrameNum++;

	if (!mIsRunning) {
		return nullptr;
	}
	if (mGotoGame) {
		Scene* game = makeScene<Game>();
		return game;
	}

	// タイトル文字の透明度の処理
	if (IsAlphaIncreasing) {
		mTitleTextAlpha += 2;
		if (mTitleTextAlpha > 255) {
			mTitleTextAlpha = 255;
			IsAlphaIncreasing = false;
		}
	}
	else {
		mTitleTextAlpha -= 2;
		if (mTitleTextAlpha < 2) {
			mTitleTextAlpha = 2;
			IsAlphaIncreasing = true;
		}
	}


	ERRCHECK(mCommonData->mAudioSystem->update());

	return this;
}

void Title::draw()
{
	SDL_SetRenderDrawColor(mCommonData->mRenderer, 220, 220, 220, 255);
	SDL_RenderClear(mCommonData->mRenderer);

	// タイトル背景描画
	{
		int tex_width;
		int tex_height;
		SDL_QueryTexture(mTitleTexture, nullptr, nullptr, &tex_width, &tex_height);

		SDL_Rect rect;
		rect.w = mCommonData->WINDOW_WIDTH;
		rect.h = tex_height * mCommonData->WINDOW_WIDTH / tex_width;
		rect.x = 0;
		rect.y = 0;
		SDL_RenderCopyEx(mCommonData->mRenderer,
			mTitleTexture,
			nullptr,
			&rect,
			0,
			nullptr,
			SDL_FLIP_NONE);
	}
	// タイトル文字描画


	int color[] = { 0xff, 0xff, 0xff, mTitleTextAlpha };
	DrawText(mTitleText[mCommonData->mLangType], mCommonData->WINDOW_WIDTH / 2, mCommonData->WINDOW_HEIGHT * 7 / 8,
		color, mCommonData->mFont, mCommonData->mRenderer, 0.2);

	SDL_RenderPresent(mCommonData->mRenderer);
}

void Title::input()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	if (keyState[SDL_SCANCODE_ESCAPE] || keyState[SDL_SCANCODE_Q])	// escapeもしくはqキーを押下すると終了
	{
		mIsRunning = false;
	}
	if (keyState[SDL_SCANCODE_RETURN]) {	// Enter キーでタイトル移動
		ERRCHECK(mBackMusic->stop(FMOD_STUDIO_STOP_IMMEDIATE));
		mGotoGame = true;
	}
	// 言語変更処理
	if (keyState[SDL_SCANCODE_J]) {			
		mCommonData->mLangType = mCommonData->JAPANESE;
	}
	if (keyState[SDL_SCANCODE_E]) {
		mCommonData->mLangType = mCommonData->ENGLISH;
	}

}