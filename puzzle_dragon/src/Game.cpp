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

	// �T�E���h�֘A
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

	// drop������
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

// Texture�f�[�^�ȂǓǂݏo��
bool Game::LoadData()
{
	// �edrop��texture��ǂݍ���
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

	
	// �ՖʂɎg�����ƒ��F��Texture��ǂݍ���
	{
		// ��
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
		// ���F
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

	// Font�ǂݍ���
	// mFont = TTF_OpenFont(".\\resources\\VL-Gothic-Regular.ttf", 128);


	// // ���{��e�L�X�g�f�[�^�ǂݍ���
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


	// Audio Data�ǂݏo��
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



static bool IsInBoard(Eigen::Vector2i _position, int ROW_MAX, int COLUMN_MAX)	// �Ֆʂ͈͓̔����H
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
		case SDL_MOUSEBUTTONDOWN:	// �}�E�X�̃{�^���������ꂽ��
		{
			mSwipingDropPos = mMousePos / GRID_SIZE;
			ERRCHECK(mMoveDropSound->start());	// swipe���ɗ������y
			phase = PHASE_SWIPE;
		}
			break;
		case SDL_MOUSEBUTTONUP:		// �}�E�X�𗣂�����
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
	if (keyState[SDL_SCANCODE_ESCAPE] || keyState[SDL_SCANCODE_Q])	// escape�L�[����������ƏI��
	{
		mIsRunning = false;
	}
	if (keyState[SDL_SCANCODE_T]) {	// t �L�[�Ń^�C�g���ړ�
		ERRCHECK(mBackMusicInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE));
		goToTitle = true;
	}
}

bool Game::FallDrops()
{
	bool isFalling = false;	// ���������H

	for (int y = ROW_MAX - 1; y > 0; y--) {	// �ォ��2�i�ڂ܂ł�drop��������
		for (int x = 0; x < COLUMN_MAX; x++) {
			if (drops[y][x].drop < 0) {
				isFalling = true;
				std::swap(drops[y][x].drop, drops[y - 1][x].drop);
				drops[y][x].fallY = -GRID_SIZE;
			}
		}
	}

	for (int x = 0; x < COLUMN_MAX; x++) {	// �ŏ�i�̐V�Kdrop�ǉ�����
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
	

	// eraseAlpha�쐬
	for (int y = 0; y < ROW_MAX; y++) {
		for (int x = 0; x < COLUMN_MAX; x++) {
			drops[y][x].eraseAlpha = 0;
		}
	}

	// ������Ă���drop��T��(completed table�쐬
	for (int y = 0; y < ROW_MAX; y++) {
		for (int x = 0; x < COLUMN_MAX; x++) {
			if (drops[y][x].drop < 0) {	// ���łɍ폜����Ă�����X�L�b�v
				continue;
			}

			for (int i = 0; i < DIRECTION_MAX; i++) {	// ���l���Ă���ʒu(x, y)�������i�ɂ��āA������Ă��邩�H
				int count = 0;	// ����i�ɂ��āA������Ă���drop�̌�
				Eigen::Vector2i position = { x, y };
				while (1) {
					count++;
					position += directions[i];
					if (
						(!IsInBoard(position, ROW_MAX, COLUMN_MAX))	// �Ֆʂ̊O���H
						|| (drops[position.y()][position.x()].drop != drops[y][x].drop)	// �قȂ�drop���H
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
			printf("%s", completed[y][x] ? "�X" : "�E");
		}
		std::cout << std::endl;
	}

	// �폜����(�폜����drop��eraseAlpha�ݒ�)
	// 1��̂ݍ폜
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
							(!IsInBoard(position, ROW_MAX, COLUMN_MAX))	// �Ֆʂ̊O���H
							|| (!completed[position.y()][position.x()])	// completed table�ɓ����Ă��邩�H
							|| (drops[position.y()][position.x()].drop != drops[y][x].drop)	//	�قȂ�drop���H
							|| (drops[position.y()][position.x()].eraseAlpha > 0)	// ���łɂ�����Ă���ƌ��o�����ォ�H
							) {
							continue;
						}

						toErase.push_back(position);
					}

					toErase.erase(toErase.begin());
				}

				std::cout << "to erase" << std::endl;
				std::cout << "�@�O�P�Q�R�S�T\n";
				for (int y2 = 0; y2 < ROW_MAX; y2++) {
					printf(" %d", y2);
					for (int x2 = 0; x2 < COLUMN_MAX; x2++) {
						printf("%s", (drops[y2][x2].eraseAlpha > 0) ? "�w" : "�E");
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
		// ���݂�mouse�̈ʒu����e�[�u���̂ǂ��ɂ���̂����Z�o
		Eigen::Vector2i newDrop = mMousePos / GRID_SIZE;
		Eigen::Vector2i localPosition;
		localPosition << mMousePos.x() % GRID_SIZE, mMousePos.y() % GRID_SIZE;	// drop�̑��݂��Ă���}�X�̃s�N�Z�����W
		if ((newDrop != mSwipingDropPos) &&		// �}�X�ɂ���h���b�v���V����dorp���A
			(
				!(	// �}�X��4���Ƀ}�E�X�J�[�\�������Ȃ����
					((localPosition.x() < GRID_SIZE * 0.25) && (localPosition.y() < GRID_SIZE * 0.25))
					|| ((localPosition.x() >= GRID_SIZE * 0.75) && (localPosition.y() < GRID_SIZE * 0.25))
					|| ((localPosition.x() < GRID_SIZE * 0.25) && (localPosition.y() >= GRID_SIZE * 0.75))
					|| ((localPosition.x() >= GRID_SIZE * 0.75) && (localPosition.y() >= GRID_SIZE * 0.75))
					)
				)
			) {	// ���ݓ������Ă���drop���V�������̂ƈقȂ��Ă����
			// ��]�G�t�F�N�g�̉�]���Ȃǂ𓱏o
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


			// drop�f�[�^����ւ�����
			std::swap(drops[mSwipingDropPos.y()][mSwipingDropPos.x()].drop, drops[newDrop.y()][newDrop.x()].drop);
			mSwipingDropPos.x() = newDrop.x();
			mSwipingDropPos.y() = newDrop.y();
		}
	}

	// �G�t�F�N�g����
	for (int y = 0; y < ROW_MAX; y++) {
		for (int x = 0; x < COLUMN_MAX; x++) {
			// �ړ�����drop�̉�]�G�t�F�N�g
			if (drops[y][x].rotate < drops[y][x].rotateMax) {
				drops[y][x].rotate += M_PI / 30;
			}

			// �R���{�̃G�t�F�N�g����
			if (drops[y][x].comboScale > COMBO_SCALE_MIN) {	// ���񂾂�R���{�\�����������Ȃ�
				drops[y][x].comboScale *= 0.95;
				if (drops[y][x].comboScale < COMBO_SCALE_MIN < 0) {
					drops[y][x].comboScale = COMBO_SCALE_MIN;
				}
			}
		}
	}

	// drop�폜�Ɨ����̏���
	switch (phase) {
	case PHASE_ERASE:
	{
		bool isErasing = false;	// �폜�����H
		for (int y = 0; y < ROW_MAX; y++) {
			for (int x = 0; x < COLUMN_MAX; x++) {
				if (drops[y][x].eraseAlpha > 0) {	// alpha�l�̘A���I�ω��ɂ���č폜��������
					isErasing = true;
					drops[y][x].eraseAlpha -= 255 / 60;
					if (drops[y][x].eraseAlpha <= 0) {
						drops[y][x].eraseAlpha = 0;
						drops[y][x].drop = DROP_NONE;
					}
				}
			}
		}
		if (!isErasing) {	// �폜����drop���Ȃ��Ȃ��
			if (!EraseDrops()) {	// �R���{���Ȃ���Ȃ�������
				FallDrops();
				phase = PHASE_FALL;	// ������phase�ڍs
			}
		}
	}
		break;
	case PHASE_FALL:
	{
		bool isFalling = false;
		for (int y = 0; y < ROW_MAX; y++) {
			for (int x = 0; x < COLUMN_MAX; x++) {
				if (drops[y][x].fallY < 0) {	// ������
					isFalling = true;
					drops[y][x].fallY += GRID_SIZE / 60;	// ��������
					if (drops[y][x].fallY >= 0) {
						drops[y][x].fallY = 0;
					}
				}
			}
		}

		if (!isFalling) {			// �P�߂̗����A�j���[�V�������I��������
			if (!FallDrops()) {		// ��������������̂��Ȃ����
				if (EraseDrops()) {	// �폜�ł���drop�������
					phase = PHASE_ERASE;
				}
				else {
					// �R���{�̃J�E���g�����Z�b�g
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

// x, y�𒆐S�ɂ��āA��width�̐����`��`��
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

	// �Ֆʕ`��
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

	// Drop�`��
	for (int y = 0; y < ROW_MAX; y++) {
		for (int x = 0; x < COLUMN_MAX; x++) {
			if (drops[y][x].rotate < drops[y][x].rotateMax) {	// ��]����drop�͕`�悵�Ȃ�
				continue;
			}
			int alpha = 255;
			// ���ݓ������Ă���drop�͔������ɂ���
			if ((phase == PHASE_SWIPE) && (Eigen::Vector2i(x, y) == mSwipingDropPos)) {
				alpha = 128;
			}
			else if ((phase == PHASE_ERASE) && (drops[y][x].eraseAlpha > 0)) {	// drop���폜���Ă���Ƃ�
				alpha = drops[y][x].eraseAlpha;
			}

			Eigen::Vector2i position = Eigen::Vector2i((x + 0.5) * GRID_SIZE, (y + 0.5) * GRID_SIZE);	// drop�̕`��ʒu
			if ((phase == PHASE_FALL) && (drops[y][x].fallY < 0)) {	// �������Ȃ�
				position.y() += drops[y][x].fallY;	// fallY�̏��𔽉f������
			}

			// Drop�`��
	/*		Eigen::Vector2i piecePos;
			piecePos.x() = (x + 0.5) * GRID_SIZE;
			piecePos.y() = (y + 0.5) * GRID_SIZE;*/
			//DrawDrop(drops[y][x].drop, piecePos, alpha);
			//DrawSquare(piecePos.x(), piecePos.y(), GRID_SIZE, 
			//	mDropTextures[drops[y][x].drop], mRenderer, alpha);
			DrawDrop(drops[y][x].drop, position, alpha);
		}
	}

	// ��]����drop�`��
	for (int y = 0; y < ROW_MAX; y++) {
		for (int x = 0; x < COLUMN_MAX; x++) {
			if (drops[y][x].rotate >= drops[y][x].rotateMax) {	// ��]���Ă��Ȃ�drop�ɂ͉�]�G�t�F�N�g�t�^���Ȃ�
				continue;
			}
			int alpha = 255;
			// �������Ă���drop�̌������ꏊ��drop�͔�����
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

	// swipe����drop�`��
	if (phase == PHASE_SWIPE) {
		DrawDrop(drops[mSwipingDropPos.y()][mSwipingDropPos.x()].drop, mMousePos);
		//DrawSquare(mMousePos.x(), mMousePos.y(), GRID_SIZE,
		//	mDropTextures[drops[mSwipingDropPos.y()][mSwipingDropPos.x()].drop], mRenderer);
	}

	// �h�h��ȃR���{�\��
	for (int y = 0; y < ROW_MAX; y++) {
		for (int x = 0; x < COLUMN_MAX; x++) {
			if (drops[y][x].combo >= 1) {	// �R���{���������Ă�����R���{�\��
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
				//position.x() -= (0 * strlen(str) * combo_scale) / 2;	// �R���{�\����drop�̐^�񒆂ɂ��낦��
				//position.y() += 0 * combo_scale / 2;
				DrawText(combo_str, position.x(), position.y(), colors[color_count % 6],
					mCommonData->mFont, mCommonData->mRenderer, combo_scale);
			}
		}
	}

	// Debug�p Phase�`��
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
