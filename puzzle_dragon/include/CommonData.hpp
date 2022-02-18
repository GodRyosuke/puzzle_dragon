#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "nlohmann/json.hpp"
#include "fmod.hpp"
#include "fmod_studio.hpp"
#include "FMOD/common.hpp"

namespace nl = nlohmann;

class CommonData {
public:
    CommonData() {}

    void setPlayerNum(int num) {
        player_num = num;
    }
    int getPlayerNum() { return player_num; }


    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;

    int WINDOW_WIDTH;
    int WINDOW_HEIGHT;

    TTF_Font* mFont;    // ゲーム中で使用するフォント
    nl::json mTextData;
    FMOD::Studio::System* mAudioSystem;

    enum {
        JAPANESE,
        ENGLISH,
        NUM_LANG_TYPE
    };

    int mLangType;


private:
    int player_num;

};