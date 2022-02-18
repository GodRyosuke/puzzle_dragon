//#pragma once

#include "Game.hpp"

class Title;

Game::Game(CommonData* const commonData)
	:Scene(commonData), 
	COLUMN_MAX(6),
	ROW_MAX(5),
	GRID_SIZE(128),
	// WINDOW_WIDTH(GRID_SIZE * COLUMN_MAX),
	// WINDOW_HEIGHT(GRID_SIZE * ROW_MAX),
	COMBO_SCALE_MAX(2.0),
	COMBO_SCALE_MIN(0.2),
	phase(PHASE_IDLE),
	frame(0),
	combo(0),
	color_count(0),
	goToTitle(false)
{
	if (!Initialize()) {
		printf("error: Failed to initialize Game Scene\n");
		exit(-1);
	}

	ERRCHECK(mBackMusicInstance->start());
}

Game::~Game()
{
	UnloadData();
	// Shutdown();
}

bool Game::Initialize()
{
	// if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	// {
	// 	SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
	// 	return false;
	// }

	// mWindow = SDL_CreateWindow("Game Programming in C++ (Chapter 3)", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	// if (!mWindow)
	// {
	// 	SDL_Log("Failed to create window: %s", SDL_GetError());
	// 	return false;
	// }

	// mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	// if (!mRenderer)
	// {
	// 	SDL_Log("Failed to create renderer: %s", SDL_GetError());
	// 	return false;
	// }

	// int flags = IMG_INIT_JPG | IMG_INIT_PNG;
	// int initted = IMG_Init(flags);
	// if ((initted & flags) != flags) {
	// 	printf("IMG_Init: Failed to init required jpg and png support!\n");
	// 	std::string error = IMG_GetError();
	// 	SDL_Log("IMG_Init: %s\n", IMG_GetError());

	// }

	// TTF_Init();

	// サウンド関連
	// void* extraDriverData = NULL;
	// Common_Init(&extraDriverData);

	// mCommonData->mAudioSystem = NULL;
	// ERRCHECK(FMOD::Studio::System::create(&mCommonData->mAudioSystem));

	// // The example Studio project is authored for 5.1 sound, so set up the system output mode to match
	// FMOD::System* coreSystem = NULL;
	// ERRCHECK(mCommonData->mAudioSystem->getCoreSystem(&coreSystem));
	// ERRCHECK(coreSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_5POINT1, 0));

	// ERRCHECK(mCommonData->mAudioSystem->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, extraDriverData));


	mIsRunning = true;

	if (!LoadData()) {
		printf("error: failed to load data");
		return false;
	}

	srand((unsigned char)time(NULL));

	// drop初期化
	drops = new DROP * [ROW_MAX];
	for (int y = 0; y < ROW_MAX; y++) {
		drops[y] = new DROP[COLUMN_MAX];
	}
	for (int y = 0; y < ROW_MAX; y++) {
		for (int x = 0; x < COLUMN_MAX; x++) {
			drops[y][x].drop = rand() % DROP_MAX;
			drops[y][x].eraseAlpha = 0;
			drops[y][x].fallY = 0;
			drops[y][x].combo = 0;
		}
	}



	return true;
}

// Textureデータなど読み出し
bool Game::LoadData()
{
	// 各dropのtextureを読み込む
	mDropTextures.resize(DROP_MAX);
	for (int i = 0; i < DROP_MAX; i++) {
		SDL_Texture* tex = nullptr;
		//std::string fileName = "C:\\Users\\kusumoto\\source\\repos\\puzzle_dragon\\puzzle_dragon\\resources\\test_picture.png";

		std::string filePath = ".\\resources\\";
		std::string fileName = "";
		switch (i) {
		case DROP_RED:
			fileName = "red_drop.png";
			break;
		case DROW_YELLOW:
			fileName = "yellow_drop.png";
			break;
		case DROP_GREEN:
			fileName = "green_drop.png";
			break;
		case DROP_BLUE:
			fileName = "blue_drop.png";
			break;
		case DROP_PURPLE:
			fileName = "purple_drop.png";
			break;
		case DROP_PINK:
			fileName = "pink_drop.png";
			break;
		default:
			return false;
		}
		filePath += fileName;
		// Load from file
		SDL_Surface* surf = IMG_Load(filePath.c_str());
		if (!surf)
		{
			SDL_Log("Failed to load texture file %s", filePath.c_str());
			return false;
		}

		// Create texture from surface
		tex = SDL_CreateTextureFromSurface(mCommonData->mRenderer, surf);
		SDL_FreeSurface(surf);
		if (!tex)
		{
			SDL_Log("Failed to convert surface to texture for %s", filePath.c_str());
			return false;
		}

		mDropTextures[i] = tex;
	}

	
	// 盤面に使う黒と茶色のTextureを読み込む
	{
		// 黒
		SDL_Texture* tex = nullptr;
		std::string filePath = ".\\resources\\black_piece.png";
		// Load from file
		SDL_Surface* surf = IMG_Load(filePath.c_str());
		if (!surf)
		{
			SDL_Log("Failed to load texture file %s", filePath.c_str());
			return false;
		}

		// Create texture from surface
		tex = SDL_CreateTextureFromSurface(mCommonData->mRenderer, surf);
		SDL_FreeSurface(surf);
		if (!tex)
		{
			SDL_Log("Failed to convert surface to texture for %s", filePath.c_str());
			return false;
		}

		mFieldTextures.insert(std::make_pair("black", tex));
	}
	{
		// 茶色
		SDL_Texture* tex = nullptr;
		std::string filePath = ".\\resources\\brown_piece.png";
		// Load from file
		SDL_Surface* surf = IMG_Load(filePath.c_str());
		if (!surf)
		{
			SDL_Log("Failed to load texture file %s", filePath.c_str());
			return false;
		}

		// Create texture from surface
		tex = SDL_CreateTextureFromSurface(mCommonData->mRenderer, surf);
		SDL_FreeSurface(surf);
		if (!tex)
		{
			SDL_Log("Failed to convert surface to texture for %s", filePath.c_str());
			return false;
		}

		mFieldTextures.insert(std::make_pair("brown", tex));
	}

	// Font読み込み
	// mFont = TTF_OpenFont(".\\resources\\VL-Gothic-Regular.ttf", 128);


	// // 日本語テキストデータ読み込み
	// {
	// 	std::string filePath = ".\\resources\\GameText.json";
	// 	std::ifstream ifs(filePath.c_str());
	// 	if (ifs.good())
	// 	{
	// 		ifs >> mTextData;
	// 		/*for (const auto& elem : mTextData.items())
	// 		{
	// 			std::cout << elem.value() << std::endl;
	// 		}*/
	// 		//std::cout << j["project name"] << std::endl;
	// 		//std::cout << j["intrinsics"]["cx"] << std::endl;
	// 	}
	// }


	// Audio Data読み出し
	// Load Bank
	// FMOD::Studio::Bank* masterBank = NULL;
	// ERRCHECK(mCommonData->mAudioSystem->loadBankFile(Common_MediaPath(".\\resources\\Master.bank"), FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank));

	// FMOD::Studio::Bank* stringsBank = NULL;
	// ERRCHECK(mCommonData->mAudioSystem->loadBankFile(Common_MediaPath(".\\resources\\Master.strings.bank"), FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank));

	// Load Back Music
	FMOD::Studio::EventDescription* BackMusicDesc = NULL;
	ERRCHECK(mCommonData->mAudioSystem->getEvent("event:/Game/back_bgm", &BackMusicDesc));
	mBackMusicInstance = NULL;
	ERRCHECK(BackMusicDesc->createInstance(&mBackMusicInstance));

	// Load Move Drop Music
	FMOD::Studio::EventDescription* MoveDropSoundDesc = NULL;
	ERRCHECK(mCommonData->mAudioSystem->getEvent("event:/Game/move_drop", &MoveDropSoundDesc));
	mMoveDropSound = NULL;
	ERRCHECK(MoveDropSoundDesc->createInstance(&mMoveDropSound));

	// Load Erase Drop Music
	FMOD::Studio::EventDescription* EraseDropSoundDesc = NULL;
	ERRCHECK(mCommonData->mAudioSystem->getEvent("event:/Game/break_drop", &EraseDropSoundDesc));
	mEraseDropSound = NULL;
	ERRCHECK(EraseDropSoundDesc->createInstance(&mEraseDropSound));


	return true;
}

void Game::UnloadData()
{
	for (auto tex : mDropTextures) {
		SDL_DestroyTexture(tex);
	}
	for (auto itr : mFieldTextures) {
		SDL_DestroyTexture(itr.second);
	}
}

void Game::shutdown()
{

	IMG_Quit();
	SDL_DestroyRenderer(mCommonData->mRenderer);
	SDL_DestroyWindow(mCommonData->mWindow);
	SDL_Quit();
}



static bool IsInBoard(Eigen::Vector2i _position, int ROW_MAX, int COLUMN_MAX)	// 盤面の範囲内か？
{
	return (_position.x() >= 0) && (_position.x() < COLUMN_MAX) &&
		(_position.y() >= 0) && (_position.y() < ROW_MAX);
}


void Game::input()
{
	SDL_Point mouse_position = { mCommonData->WINDOW_WIDTH / 2, mCommonData->WINDOW_HEIGHT / 2 };
	SDL_GetMouseState(&mouse_position.x, &mouse_position.y);
	mMousePos.x() = mouse_position.x;
	mMousePos.y() = mouse_position.y;

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;
		case SDL_MOUSEBUTTONDOWN:	// マウスのボタンが押されたら
		{
			mSwipingDropPos = mMousePos / GRID_SIZE;
			ERRCHECK(mMoveDropSound->start());	// swipe中に流す音楽
			phase = PHASE_SWIPE;
		}
			break;
		case SDL_MOUSEBUTTONUP:		// マウスを離したら
			if (phase == PHASE_SWIPE) {
				combo = 0;
				ERRCHECK(mMoveDropSound->stop(FMOD_STUDIO_STOP_IMMEDIATE));

				if (EraseDrops()) {
					phase = PHASE_ERASE;
				}
				else {
					phase = PHASE_IDLE;
				}
			}
			break;
		}
	}

	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	if (keyState[SDL_SCANCODE_ESCAPE] || keyState[SDL_SCANCODE_Q])	// escapeキーを押下すると終了
	{
		mIsRunning = false;
	}
	if (keyState[SDL_SCANCODE_T]) {	// t キーでタイトル移動
		ERRCHECK(mBackMusicInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE));
		goToTitle = true;
	}
}

bool Game::FallDrops()
{
	bool isFalling = false;	// 落下中か？

	for (int y = ROW_MAX - 1; y > 0; y--) {	// 上から2段目までのdrop落下処理
		for (int x = 0; x < COLUMN_MAX; x++) {
			if (drops[y][x].drop < 0) {
				isFalling = true;
				std::swap(drops[y][x].drop, drops[y - 1][x].drop);
				drops[y][x].fallY = -GRID_SIZE;
			}
		}
	}

	for (int x = 0; x < COLUMN_MAX; x++) {	// 最上段の新規drop追加処理
		if (drops[0][x].drop < 0) {
			isFalling = true;
			drops[0][x].drop = rand() % DROP_MAX;
			drops[0][x].fallY = -GRID_SIZE;
		}
	}

	return isFalling;
}

bool Game::EraseDrops()
{
	bool** completed = new bool*[ROW_MAX];
	for (int y = 0; y < ROW_MAX; y++) {
		completed[y] = new bool[COLUMN_MAX];
	}
	for (int y = 0; y < ROW_MAX; y++) {
		for (int x = 0; x < COLUMN_MAX; x++) {
			completed[y][x] = false;
		}
	}
	

	// eraseAlpha作成
	for (int y = 0; y < ROW_MAX; y++) {
		for (int x = 0; x < COLUMN_MAX; x++) {
			drops[y][x].eraseAlpha = 0;
		}
	}

	// そろっているdropを探す(completed table作成
	for (int y = 0; y < ROW_MAX; y++) {
		for (int x = 0; x < COLUMN_MAX; x++) {
			if (drops[y][x].drop < 0) {	// すでに削除されていたらスキップ
				continue;
			}

			for (int i = 0; i < DIRECTION_MAX; i++) {	// 今考えている位置(x, y)から方向iについて、そろっているか？
				int count = 0;	// 方向iについて、そろっているdropの個数
				Eigen::Vector2i position = { x, y };
				while (1) {
					count++;
					position += directions[i];
					if (
						(!IsInBoard(position, ROW_MAX, COLUMN_MAX))	// 盤面の外か？
						|| (drops[position.y()][position.x()].drop != drops[y][x].drop)	// 異なるdropか？
						) {
						break;
					}
				}

				if (count >= 3) {
					//printf("x: %d y: %d dir: %d cnt: %d\n", x, y, i, count);
					position = { x, y };
					for (int j = 0; j < count; j++) {
						completed[position.y()][position.x()] = true;
						position += directions[i];
					}
				}
			}
		}
	}

	std::cout << "completed\n";
	std::cout << "   0 1 2 3 4 5\n";
	for (int y = 0; y < ROW_MAX; y++) {
		printf(" %d", y);
		for (int x = 0; x < COLUMN_MAX; x++) {
			printf("%s", completed[y][x] ? "Ⅴ" : "・");
		}
		std::cout << std::endl;
	}

	// 削除処理(削除するdropにeraseAlpha設定)
	// 1列のみ削除
	for (int y = ROW_MAX - 1; y >= 0; y--) {
		for (int x = 0; x < COLUMN_MAX; x++) {
			if (completed[y][x]) {
				combo++;
				drops[y][x].combo = combo;
				drops[y][x].comboScale = COMBO_SCALE_MAX;

				std::vector<Eigen::Vector2i> toErase;
				toErase.push_back({ x, y });
				while (!toErase.empty()) {
					drops[toErase.front().y()][toErase.front().x()].eraseAlpha = 255;
					for (int i = 0; i < DIRECTION_MAX; i++) {
						Eigen::Vector2i position = toErase.front() + directions[i];

						if (
							(!IsInBoard(position, ROW_MAX, COLUMN_MAX))	// 盤面の外か？
							|| (!completed[position.y()][position.x()])	// completed tableに入っているか？
							|| (drops[position.y()][position.x()].drop != drops[y][x].drop)	//	異なるdropか？
							|| (drops[position.y()][position.x()].eraseAlpha > 0)	// すでにそろっていると検出した後か？
							) {
							continue;
						}

						toErase.push_back(position);
					}

					toErase.erase(toErase.begin());
				}

				std::cout << "to erase" << std::endl;
				std::cout << "　０１２３４５\n";
				for (int y2 = 0; y2 < ROW_MAX; y2++) {
					printf(" %d", y2);
					for (int x2 = 0; x2 < COLUMN_MAX; x2++) {
						printf("%s", (drops[y2][x2].eraseAlpha > 0) ? "Ｘ" : "・");
					}
					printf("\n");
				}
				printf("\n");

				ERRCHECK(mEraseDropSound->start());

				return true;
			}
		}
	}

	for (int y = 0; y < ROW_MAX; y++) {
		free(completed[y]);
	}
	free(completed);

	return false;
}


Scene* Game::update()
{
	frame++;

	if (phase == PHASE_SWIPE) {
		// 現在のmouseの位置からテーブルのどこにいるのかを算出
		Eigen::Vector2i newDrop = mMousePos / GRID_SIZE;
		Eigen::Vector2i localPosition;
		localPosition << mMousePos.x() % GRID_SIZE, mMousePos.y() % GRID_SIZE;	// dropの存在しているマスのピクセル座標
		if ((newDrop != mSwipingDropPos) &&		// マスにいるドロップが新しいdorpかつ、
			(
				!(	// マスの4隅にマウスカーソルがいなければ
					((localPosition.x() < GRID_SIZE * 0.25) && (localPosition.y() < GRID_SIZE * 0.25))
					|| ((localPosition.x() >= GRID_SIZE * 0.75) && (localPosition.y() < GRID_SIZE * 0.25))
					|| ((localPosition.x() < GRID_SIZE * 0.25) && (localPosition.y() >= GRID_SIZE * 0.75))
					|| ((localPosition.x() >= GRID_SIZE * 0.75) && (localPosition.y() >= GRID_SIZE * 0.75))
					)
				)
			) {	// 現在動かしているdropが新しいものと異なっていれば
			// 回転エフェクトの回転軸などを導出
			Eigen::Vector2d mSwipingDropPosd = { (double)mSwipingDropPos.x(), (double)mSwipingDropPos.y() };
			Eigen::Vector2d newDropd = { (double)newDrop.x(), (double)newDrop.y() };
			Eigen::Vector2d one_vec;
			one_vec <<  1.0, 1.0 ;
			Eigen::Vector2d swipingCenter = mSwipingDropPosd * GRID_SIZE + one_vec * GRID_SIZE / 2;
			Eigen::Vector2d newCenter = newDropd * GRID_SIZE + one_vec * GRID_SIZE / 2;
			Eigen::Vector2d center = swipingCenter / 2 + newCenter / 2;

			drops[mSwipingDropPos.y()][mSwipingDropPos.x()].center =
				drops[newDrop.y()][newDrop.x()].center = center;
			Eigen::Vector2d centerToNew = newCenter - center;

			drops[mSwipingDropPos.y()][mSwipingDropPos.x()].rotateMax = atan2(-centerToNew.y(), centerToNew.x());
			drops[mSwipingDropPos.y()][mSwipingDropPos.x()].rotate =
				drops[mSwipingDropPos.y()][mSwipingDropPos.x()].rotateMax - M_PI;
			drops[newDrop.y()][newDrop.x()].rotate = drops[mSwipingDropPos.y()][mSwipingDropPos.x()].rotateMax;
			drops[newDrop.y()][newDrop.x()].rotateMax =
				drops[newDrop.y()][newDrop.x()].rotate + M_PI;


			// dropデータ入れ替え処理
			std::swap(drops[mSwipingDropPos.y()][mSwipingDropPos.x()].drop, drops[newDrop.y()][newDrop.x()].drop);
			mSwipingDropPos.x() = newDrop.x();
			mSwipingDropPos.y() = newDrop.y();
		}
	}

	// エフェクト処理
	for (int y = 0; y < ROW_MAX; y++) {
		for (int x = 0; x < COLUMN_MAX; x++) {
			// 移動したdropの回転エフェクト
			if (drops[y][x].rotate < drops[y][x].rotateMax) {
				drops[y][x].rotate += M_PI / 30;
			}

			// コンボのエフェクト処理
			if (drops[y][x].comboScale > COMBO_SCALE_MIN) {	// だんだんコンボ表示が小さくなる
				drops[y][x].comboScale *= 0.95;
				if (drops[y][x].comboScale < COMBO_SCALE_MIN < 0) {
					drops[y][x].comboScale = COMBO_SCALE_MIN;
				}
			}
		}
	}

	// drop削除と落下の処理
	switch (phase) {
	case PHASE_ERASE:
	{
		bool isErasing = false;	// 削除中か？
		for (int y = 0; y < ROW_MAX; y++) {
			for (int x = 0; x < COLUMN_MAX; x++) {
				if (drops[y][x].eraseAlpha > 0) {	// alpha値の連続的変化によって削除中を実現
					isErasing = true;
					drops[y][x].eraseAlpha -= 255 / 60;
					if (drops[y][x].eraseAlpha <= 0) {
						drops[y][x].eraseAlpha = 0;
						drops[y][x].drop = DROP_NONE;
					}
				}
			}
		}
		if (!isErasing) {	// 削除するdropがなくなれば
			if (!EraseDrops()) {	// コンボがつながらなかったら
				FallDrops();
				phase = PHASE_FALL;	// 落下中phase移行
			}
		}
	}
		break;
	case PHASE_FALL:
	{
		bool isFalling = false;
		for (int y = 0; y < ROW_MAX; y++) {
			for (int x = 0; x < COLUMN_MAX; x++) {
				if (drops[y][x].fallY < 0) {	// 落下中
					isFalling = true;
					drops[y][x].fallY += GRID_SIZE / 60;	// 落下距離
					if (drops[y][x].fallY >= 0) {
						drops[y][x].fallY = 0;
					}
				}
			}
		}

		if (!isFalling) {			// １つめの落下アニメーションが終了したら
			if (!FallDrops()) {		// もう落下するものがなければ
				if (EraseDrops()) {	// 削除できるdropがあれば
					phase = PHASE_ERASE;
				}
				else {
					// コンボのカウントをリセット
					for (int y = 0; y < ROW_MAX; y++) {
						for (int x = 0; x < COLUMN_MAX; x++) {
							drops[y][x].combo = 0;
						}
					}

					phase = PHASE_IDLE;
				}
			}
		}
	}
	break;
	}


	// Update Game
	ERRCHECK(mCommonData->mAudioSystem->update());

	if (mIsRunning == false) {
		return nullptr;
	}

	if (goToTitle) {
		Scene* title = makeScene<Title>();
		return title;
	}

	return this;
}

// x, yを中心にして、幅widthの正方形を描画
static void DrawSquare(int x, int y, int width, SDL_Texture* tex, SDL_Renderer* renderer, int alpha = 255)
{
	SDL_SetTextureAlphaMod(tex, alpha);

	int tex_width;
	int tex_height;
	SDL_QueryTexture(tex, nullptr, nullptr, &tex_width, &tex_height);

	SDL_Rect rect;
	rect.w = width;
	rect.h = width;
	rect.x = x - width / 2;
	rect.y = y - width / 2;
	SDL_RenderCopyEx(renderer,
		tex,
		nullptr,
		&rect,
		0,
		nullptr,
		SDL_FLIP_NONE);
}

void Game::DrawDrop(int drop, Eigen::Vector2i position, int alpha)
{
	DrawDrop(drop, position.x(), position.y(), alpha);
}

void Game::DrawDrop(int drop, int x, int y, int alpha)
{
	if (drop < 0) {
		return;
	}
	SDL_Texture* tex = mDropTextures[drop];

	SDL_SetTextureAlphaMod(tex, alpha);

	int tex_width;
	int tex_height;
	SDL_QueryTexture(tex, nullptr, nullptr, &tex_width, &tex_height);

	SDL_Rect rect;
	rect.w = GRID_SIZE;
	rect.h = GRID_SIZE;
	rect.x = x - GRID_SIZE / 2;
	rect.y = y - GRID_SIZE / 2;
	SDL_RenderCopyEx(mCommonData->mRenderer,
		tex,
		nullptr,
		&rect,
		0,
		nullptr,
		SDL_FLIP_NONE);
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

void Game::draw()
{
	SDL_SetRenderDrawColor(mCommonData->mRenderer, 220, 220, 220, 255);
	SDL_RenderClear(mCommonData->mRenderer);

	// 盤面描画
	for (int y = 0; y < ROW_MAX; y++) {
		for (int x = 0; x < COLUMN_MAX; x++) {
			Eigen::Vector2i piecePos;
			piecePos.x() = (x + 0.5) * GRID_SIZE;
			piecePos.y() = (y + 0.5) * GRID_SIZE;
			std::string color = ((x + y) % 2 == 0) ? "black" : "brown";
			SDL_Texture* pieceTex = mFieldTextures[color];
			DrawSquare(piecePos.x(), piecePos.y(), GRID_SIZE, pieceTex, mCommonData->mRenderer);
		}
	}

	// Drop描画
	for (int y = 0; y < ROW_MAX; y++) {
		for (int x = 0; x < COLUMN_MAX; x++) {
			if (drops[y][x].rotate < drops[y][x].rotateMax) {	// 回転中のdropは描画しない
				continue;
			}
			int alpha = 255;
			// 現在動かしているdropは半透明にする
			if ((phase == PHASE_SWIPE) && (Eigen::Vector2i(x, y) == mSwipingDropPos)) {
				alpha = 128;
			}
			else if ((phase == PHASE_ERASE) && (drops[y][x].eraseAlpha > 0)) {	// dropを削除しているとき
				alpha = drops[y][x].eraseAlpha;
			}

			Eigen::Vector2i position = Eigen::Vector2i((x + 0.5) * GRID_SIZE, (y + 0.5) * GRID_SIZE);	// dropの描画位置
			if ((phase == PHASE_FALL) && (drops[y][x].fallY < 0)) {	// 落下中なら
				position.y() += drops[y][x].fallY;	// fallYの情報を反映させる
			}

			// Drop描画
	/*		Eigen::Vector2i piecePos;
			piecePos.x() = (x + 0.5) * GRID_SIZE;
			piecePos.y() = (y + 0.5) * GRID_SIZE;*/
			//DrawDrop(drops[y][x].drop, piecePos, alpha);
			//DrawSquare(piecePos.x(), piecePos.y(), GRID_SIZE, 
			//	mDropTextures[drops[y][x].drop], mRenderer, alpha);
			DrawDrop(drops[y][x].drop, position, alpha);
		}
	}

	// 回転中のdrop描画
	for (int y = 0; y < ROW_MAX; y++) {
		for (int x = 0; x < COLUMN_MAX; x++) {
			if (drops[y][x].rotate >= drops[y][x].rotateMax) {	// 回転していないdropには回転エフェクト付与しない
				continue;
			}
			int alpha = 255;
			// 動かしているdropの元居た場所のdropは半透明
			if ((phase == PHASE_SWIPE) && (Eigen::Vector2i(x, y) == mSwipingDropPos)) {
				alpha = 127;
			}
			Eigen::Vector2d position = drops[y][x].center;
			position += Eigen::Vector2d(-(GRID_SIZE / 2.0) * cos(drops[y][x].rotate),
				(GRID_SIZE / 2.0) * sin(drops[y][x].rotate));

			DrawDrop(drops[y][x].drop, (int)position.x(), (int)position.y(), alpha);
			//DrawSquare(position.x(), position.y(), GRID_SIZE, mDropTextures[drops[y][x].drop], mRenderer, alpha);
		}
	}

	// swipe中のdrop描画
	if (phase == PHASE_SWIPE) {
		DrawDrop(drops[mSwipingDropPos.y()][mSwipingDropPos.x()].drop, mMousePos);
		//DrawSquare(mMousePos.x(), mMousePos.y(), GRID_SIZE,
		//	mDropTextures[drops[mSwipingDropPos.y()][mSwipingDropPos.x()].drop], mRenderer);
	}

	// ド派手なコンボ表示
	for (int y = 0; y < ROW_MAX; y++) {
		for (int x = 0; x < COLUMN_MAX; x++) {
			if (drops[y][x].combo >= 1) {	// コンボが発生していたらコンボ表示
				char str[16];
				sprintf_s(str, "Combo %d", drops[y][x].combo);
				int colors[][3] = {
					{0xff, 0x00, 0x00},
					{0x00, 0xff, 0x00},
					{0x00, 0x00, 0xff},
					{0xff, 0xff, 0x00},
					{0x00, 0xff, 0xff},
					{0xff, 0x00, 0xff},
				};
				if (frame % 10 == 0) {
					color_count++;
				}
				std::string combo_str = "Combo " + std::to_string(drops[y][x].combo);
				double combo_scale = drops[y][x].comboScale;
				Eigen::Vector2i position = Eigen::Vector2i((x + 0.5) * GRID_SIZE, (y + 0.5) * GRID_SIZE);
				//position.x() -= (0 * strlen(str) * combo_scale) / 2;	// コンボ表示をdropの真ん中にそろえる
				//position.y() += 0 * combo_scale / 2;
				DrawText(combo_str, position.x(), position.y(), colors[color_count % 6],
					mCommonData->mFont, mCommonData->mRenderer, combo_scale);
			}
		}
	}

	// Debug用 Phase描画
	int color[] = { 0xff, 0xff, 0xff, 240 };
	const char* phaseName[] = {
		"PHASE_IDLE",
		"PHASE_SWIPE",
		"PHASE_ERASE",
		"PHASE_FALL",
		"PHASE_MAX"
	};
	//DrawText(phaseName[phase], GRID_SIZE * 1.5, GRID_SIZE * 0.5, color, mFont, mRenderer, 0.4);



	SDL_RenderPresent(mCommonData->mRenderer);
}

void Game::RunLoop()
{
	ERRCHECK(mBackMusicInstance->start());
	while (mIsRunning) {
		ProcessInput();
		UpdateGame();
		Draw();
	}
}
