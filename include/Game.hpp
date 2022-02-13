#include <iostream>
#include <iostream>
#include <math.h>
#include <vector>
#include <utility>
#include <time.h>
#include "GL/freeglut.h"
#include "glm/glm.hpp"
#include <Eigen/Dense>
#include "SDL.h"
#include "SDL_image.h"

class Game {
public:
	Game();

	bool Initialize();
	void RunLoop();		// Gameのメインループ
	void Shutdown();

private:
	void ProcessInput();
	void UpdateGame();
	void Draw();

	bool EraseDrops();

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;

	SDL_Texture* mTexture;

	bool mIsRunning;

	const int COLUMN_MAX;
	const int ROW_MAX;
	
	const int GRID_SIZE;
	
	const int WINDOW_WIDTH;
	const int WINDOW_HEIGHT;
	
	const double COMBO_SCALE_MAX;
	const double COMBO_SCALE_MIX;


	// パズルの色
	enum {
		DROP_NONE = -1,
		DROP_RED,
		DROW_YELLOW,
		DROP_GREEN,
		DROP_BLUE,
		DROP_PURPLE,
		DROP_PINK,
		DROP_MAX
	};

	// ゲームの進行状態
	enum {
		PHASE_IDLE,
		PHASE_SWIPE,
		PHASE_ERASE,
		PHASE_FALL,
		PHASE_MAX
	};

	enum {
		DIRECTION_UP,
		DIRECTION_LEFT,
		DIRECTION_DOWN,
		DIRECTION_RIGHT,
		DIRECTION_MAX
	};

	typedef struct {
		int drop;
		glm::vec2 center;	// 回転中心
		double rotate;		// 現在の回転量
		double rotateMax;	// 最大の回転量
		double eraseAlpha;	// 削除するときの不透明度
		double fallY;		// 落下中のy座標
		int combo;
		double comboScale;
	}DROP;

	DROP** drops;

	glm::ivec2 mousePosition;
	glm::ivec2 swipingDrop;

	int phase;

	glm::ivec2 directions[DIRECTION_MAX] = {
		{0, -1},		//DIRECTION_UP,
		{-1, 0},		//DIRECTION_LEFT,
		{0, 1},			//DIRECTION_DOWN,
		{1, 0},			//DIRECTION_RIGHT,
	};

	int combo;	// 現在のコンボ数
	unsigned int frame;	// ど派手なコンボ表示用
	unsigned int color_count;
};