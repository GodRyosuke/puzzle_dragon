#include "Game.hpp"

Game::Game()
	:COLUMN_MAX(6),
	ROW_MAX(5),
	GRID_SIZE(128),
	WINDOW_WIDTH(GRID_SIZE * COLUMN_MAX),
	WINDOW_HEIGHT(GRID_SIZE * ROW_MAX),
	COMBO_SCALE_MAX(2.0),
	COMBO_SCALE_MIX(0.2),
	phase(PHASE_IDLE),
	frame(0),
	color_count(0)
{

}

bool Game::Initialize()
{
	drops = new DROP*[ROW_MAX];
	for (int y = 0; y < ROW_MAX; y++) {
		drops[y] = new DROP[COLUMN_MAX];
	}

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	mWindow = SDL_CreateWindow("Game Programming in C++ (Chapter 3)", 100, 100, 1024, 768, 0);
	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!mRenderer)
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
	mIsRunning = true;

	return true;
}


void Game::Shutdown()
{

}

static bool IsInBoard(glm::ivec2 _position, const int ROW_MAX, const int COLUMN_MAX)	// 盤面の範囲内か？
{
	return (_position.x >= 0) && (_position.x < COLUMN_MAX) &&
		(_position.y >= 0) && (_position.y < ROW_MAX);
}


void Game::ProcessInput()
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
	if (keyState[SDL_SCANCODE_ESCAPE])	// escapeキーを押下すると終了
	{
		mIsRunning = false;
	}
}

void Game::UpdateGame()
{

}

void Game::Draw()
{
	SDL_SetRenderDrawColor(mRenderer, 220, 220, 220, 255);
	SDL_RenderClear(mRenderer);

	int tex_width;
	int tex_height;
	SDL_QueryTexture(mTexture, nullptr, nullptr, &tex_width, &tex_height);

	SDL_Rect rect;
	rect.w = 1024;
	rect.h = tex_height * 1024 / tex_width;
	rect.x = 0;
	rect.y = 0;
	SDL_RenderCopyEx(mRenderer,
		mTexture,
		nullptr,
		&rect,
		0,
		nullptr,
		SDL_FLIP_NONE);


	SDL_RenderPresent(mRenderer);
}

void Game::RunLoop()
{
	// SDL Textureを作成
	SDL_Texture* tex = nullptr;
	//std::string fileName = "C:\\Users\\kusumoto\\source\\repos\\puzzle_dragon\\puzzle_dragon\\resources\\test_picture.png";
	std::string fileName = ".\\resources\\test_picture.png";
	// Load from file
	SDL_Surface* surf = IMG_Load(fileName.c_str());
	if (!surf)
	{
		SDL_Log("Failed to load texture file %s", fileName.c_str());
		return;
	}

	// Create texture from surface
	tex = SDL_CreateTextureFromSurface(mRenderer, surf);
	SDL_FreeSurface(surf);
	if (!tex)
	{
		SDL_Log("Failed to convert surface to texture for %s", fileName.c_str());
		return;
	}

	mTexture = tex;


	while (mIsRunning) {
		ProcessInput();
		UpdateGame();
		Draw();
	}

}
