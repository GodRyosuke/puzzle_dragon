

#include <iostream>
#include <fstream>
#include <math.h>
#include <time.h>
#include <vector>
#include <map>
#include <utility>
#include <time.h>
#include "GL/freeglut.h"
#include "glm/glm.hpp"
#include <Eigen/Dense>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "nlohmann/json.hpp"
#include "fmod.hpp"
#include "fmod_studio.hpp"
#include "FMOD/common.hpp"

namespace nl = nlohmann;

class Game {
public:
	Game();

	bool Initialize();
	void RunLoop();		// Game�̃��C�����[�v
	void Shutdown();

private:
	void ProcessInput();
	void UpdateGame();
	void Draw();

	bool LoadData();
	void UnloadData();

	void DrawDrop(int drop, Eigen::Vector2i position, int alpha = 255);
	void DrawDrop(int drop, int x, int y, int alpha = 255);
	bool EraseDrops();
	bool FallDrops();

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;


	bool mIsRunning;

	const int COLUMN_MAX;
	const int ROW_MAX;
	
	const int GRID_SIZE;
	
	const int WINDOW_WIDTH;
	const int WINDOW_HEIGHT;
	
	const double COMBO_SCALE_MAX;
	const double COMBO_SCALE_MIN;


	// �p�Y���̐F
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

	// �Q�[���̐i�s���
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
		Eigen::Vector2d center;	// ��]���S
		double rotate;		// ���݂̉�]��
		double rotateMax;	// �ő�̉�]��
		int eraseAlpha;	// �폜����Ƃ��̕s�����x
		double fallY;		// ��������y���W
		int combo;
		double comboScale;
	}DROP;

	DROP** drops;

	//glm::ivec2 mousePosition;
	//glm::ivec2 swipingDrop;

	int phase;

	Eigen::Vector2i directions[DIRECTION_MAX] = {
		{0, -1},		//DIRECTION_UP,
		{-1, 0},		//DIRECTION_LEFT,
		{0, 1},			//DIRECTION_DOWN,
		{1, 0},			//DIRECTION_RIGHT,
	};

	int combo;	// ���݂̃R���{��
	unsigned int frame;	// �ǔh��ȃR���{�\���p
	unsigned int color_count;

	std::vector<SDL_Texture*> mDropTextures;
	std::map<std::string, SDL_Texture*>mFieldTextures;
	nl::json mTextData;

	Eigen::Vector2i mMousePos;
	Eigen::Vector2i mSwipingDropPos;

	TTF_Font* mFont;
	FMOD::Studio::System* mAudioSystem;
	FMOD::Studio::EventInstance* mBackMusicInstance;
	FMOD::Studio::EventInstance* mMoveDropSound;
	FMOD::Studio::EventInstance* mEraseDropSound;
};