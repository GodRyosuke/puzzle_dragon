#define _USE_MATH_DEFINES

#include <iostream>
#include <math.h>
#include <vector>
#include <utility>
#include <time.h>
#include "GL/freeglut.h"
//#include "glm\glm.hpp"
#include "glm/glm.hpp"

#define COLUMN_MAX	(6)
#define ROW_MAX		(5)

#define GRID_SIZE	(128)

#define WINDOW_WIDTH	(GRID_SIZE * COLUMN_MAX)
#define WINDOW_HEIGHT	(GRID_SIZE * ROW_MAX)

#define COMBO_SCALE_MAX (2.0)
#define COMBO_SCALE_MIN (0.2)

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
	glm::vec2 center;	// ��]���S
	double rotate;		// ���݂̉�]��
	double rotateMax;	// �ő�̉�]��
	double eraseAlpha;	// �폜����Ƃ��̕s�����x
	double fallY;		// ��������y���W
	int combo;
	double comboScale;
}DROP;

DROP drops[ROW_MAX][COLUMN_MAX];

glm::ivec2 mousePosition;
glm::ivec2 swipingDrop;

int phase;

glm::ivec2 directions[DIRECTION_MAX] = {
	{0, -1},		//DIRECTION_UP,
	{-1, 0},		//DIRECTION_LEFT,
	{0, 1},			//DIRECTION_DOWN,
	{1, 0},			//DIRECTION_RIGHT,
};

int combo;	// ���݂̃R���{��
unsigned int frame;	// �ǔh��ȃR���{�\���p
unsigned int color_count;

bool IsInBoard(glm::ivec2 _position)	// �Ֆʂ͈͓̔����H
{
	return (_position.x >= 0) && (_position.x < COLUMN_MAX) &&
		(_position.y >= 0) && (_position.y < ROW_MAX);
}

bool EraseDrops()
{
	bool completed[ROW_MAX][COLUMN_MAX]{	// �������drop
	};

	// eraseAlpha�쐬
	for (int y = 0; y < ROW_MAX; y++) {
		for (int x = 0; x < COLUMN_MAX; x++) {
			drops[y][x].eraseAlpha = 0;
		}
	}

	// ������Ă���drop��T��(completed table�쐬
	for (int y = 0; y < ROW_MAX; y++) {
		for (int x = 0; x < COLUMN_MAX; x++) {
			if (drops[y][x].drop < 0) {
				continue;
			}

			for (int i = 0; i < DIRECTION_MAX; i++) {	// ���l���Ă���ʒu(x, y)�������i�ɂ��āA������Ă��邩�H
				int count = 0;	// ����i�ɂ��āA������Ă���drop�̌�
				glm::ivec2 position = { x, y };
				while (1) {
					count++;
					position += directions[i];
					if (
						(!IsInBoard(position))	// �Ֆʂ̊O���H
							|| (drops[position.y][position.x].drop != drops[y][x].drop)	// �قȂ�drop���H
							) {
						break;
					}
				}

				if (count >= 3) {
					//printf("x: %d y: %d dir: %d cnt: %d\n", x, y, i, count);
					position = { x, y };
					for (int j = 0; j < count; j++) {
						completed[position.y][position.x] = true;
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

				std::vector<glm::ivec2> toErase;
				toErase.push_back({ x, y });
				while (!toErase.empty()) { 
					drops[toErase.front().y][toErase.front().x].eraseAlpha = 1;
					for (int i = 0; i < DIRECTION_MAX; i++) {
						glm::ivec2 position = toErase.front() + directions[i];

						if (
							(!IsInBoard(position))	// �Ֆʂ̊O���H
							|| (!completed[position.y][position.x])	// completed table�ɓ����Ă��邩�H
							|| (drops[position.y][position.x].drop != drops[y][x].drop)	//	�قȂ�drop���H
							|| (drops[position.y][position.x].eraseAlpha > 0)
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

				return true;
			}
		}
	}

	return false;
}

// �󔒂�drop���ォ��1�������߂�
// @return ����������̂������true
bool FallDrops()	
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

// drop�`��
// @param _drop drop type
// @param _position drop��`�悷��s�N�Z�����W
void DrowDrop(int _drop, glm::vec2 _position, double _alpha)
{
	if (_drop < 0) {	// DROP_NONE�͕`�悵�Ȃ�
		return;
	}

	glPushMatrix();
	{
		glTranslated(_position.x, _position.y, 0);
		int n = 32;
		double radius = GRID_SIZE / 2 * 0.95;
		double insideRadius = radius * 0.8;
		double colors[DROP_MAX][4]{
			{1, 0, 0, _alpha},		//DROP_RED
			{1, 1, 0, _alpha},		//DROW_YELLOW
			{0, 0.5, 0, _alpha},	//DROP_GREEN
			{0, 0, 1, _alpha},		//DROP_BLUE
			{0.5, 0, 1, _alpha},	//DROP_PURPLE
			{1, 0, 0.5, _alpha}		//DROP_PINK
		};

		// �~���drop��`��
		glBegin(GL_TRIANGLE_FAN);
		{
			glColor4d(1, 1, 1, _alpha);
			glVertex2i(0, 0);
			glColor4dv(colors[_drop]);
			for (int i = 0; i <= n; i++) {
				double theta = 2 * M_PI * i / n;
				glVertex2d(insideRadius * cos(theta), insideRadius * sin(theta));
			}
		}
		glEnd();

		// �~���̘g�`��
		glBegin(GL_TRIANGLE_STRIP);
		{
			for (int i = 0; i <= n; i++) {
				double theta = 2 * M_PI * i / n;
				glColor4d(1, 1, 1, _alpha);
				glVertex2d(radius * cos(theta), radius * sin(theta));
				glColor4dv(colors[_drop]);
				glVertex2d(insideRadius * cos(theta), insideRadius * sin(theta));
			}
		}
		glEnd();
	}
	glPopMatrix();
}


void Display()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// �Ֆʕ`��
	for (int y = 0; y < ROW_MAX; y++) {
		for (int x = 0; x < COLUMN_MAX; x++) {
			glPushMatrix();
			{
				glTranslatef(float(x * GRID_SIZE), float(y * GRID_SIZE), 0);
				const unsigned char colors[][3] = {	// �Ֆʂ̐F
					{0x20, 0x10, 0x08},
					{0x40, 0x20, 0x10}
				};
				glColor3ubv(colors[(x + y) % 2]);
				glBegin(GL_QUADS);
				{
					glVertex2i(0, 0);
					glVertex2i(0, GRID_SIZE);
					glVertex2i(GRID_SIZE, GRID_SIZE);
					glVertex2i(GRID_SIZE, 0);
				}
				glEnd();
			}
			glPopMatrix();
		}
	}

	// Drop�`��
	for (int y = 0; y < ROW_MAX; y++) {
		for (int x = 0; x < COLUMN_MAX; x++) {
			if (drops[y][x].rotate < drops[y][x].rotateMax) {	// ��]����drop�͕`�悵�Ȃ�
				continue;
			}
			// grid �̒��S�ɉ~��`��
			double alpha = 1;
			// ���ݓ������Ă���drop�͔������ɂ���
			if ((phase == PHASE_SWIPE) && (glm::ivec2(x, y) == swipingDrop)) {
				alpha = 0.25;
			}
			else if ((phase == PHASE_ERASE) && (drops[y][x].eraseAlpha > 0)) {	// drop���폜���Ă���Ƃ�
				alpha = drops[y][x].eraseAlpha;
			}

			glm::vec2 position = glm::vec2((x + 0.5) * GRID_SIZE, (y + 0.5) * GRID_SIZE);	// drop�̕`��ʒu
			if ((phase == PHASE_FALL) && (drops[y][x].fallY < 0)) {
				position.y += drops[y][x].fallY;	// fallY�̏��𔽉f������
			}

			DrowDrop(drops[y][x].drop, position, alpha);
		}
	}

	// ��]�G�t�F�N�g�̕`�揈��
	for (int y = 0; y < ROW_MAX; y++) {
		for (int x = 0; x < COLUMN_MAX; x++) {
			if (drops[y][x].rotate >= drops[y][x].rotateMax) {	// ��]���Ă��Ȃ�drop�ɂ͉�]�G�t�F�N�g�t�^���Ȃ�
				continue;
			}
			double alpha = 1;
			// ���ݓ������Ă���drop�͔������ɂ���
			if ((phase == PHASE_SWIPE) && (glm::ivec2(x, y) == swipingDrop)) {
				alpha = 0.25;
			}
			glm::vec2 position = drops[y][x].center;
			position += glm::vec2(-(GRID_SIZE / 2.0) * cos(drops[y][x].rotate),
				(GRID_SIZE / 2.0) * sin(drops[y][x].rotate));
			DrowDrop(drops[y][x].drop, position, alpha);
		}
	}

	for (int y = 0; y < ROW_MAX; y++) {
		for (int x = 0; x < COLUMN_MAX; x++) {
			if (drops[y][x].combo >= 1) {	// �R���{���������Ă�����R���{�\��
				char str[16];
				sprintf_s(str, "Combo %d", drops[y][x].combo);
				const unsigned char colors[][3] = {
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
				glColor3ubv(colors[color_count % 6]);
				glPushMatrix();
				{
					double s = drops[y][x].comboScale;
					glm::vec2 position = glm::vec2((x + 0.5) * GRID_SIZE, (y + 0.5) * GRID_SIZE);
					position.x -= (80 * strlen(str) * s) / 2;	// �R���{�\����drop�̐^�񒆂ɂ��낦��
					position.y += 100 * s / 2;

					glTranslated(position.x, position.y, 0);
					glScaled(s, -s, 0);
					glLineWidth(24 * s);
					glutStrokeString(GLUT_STROKE_ROMAN, (const unsigned char*)str);
				}
				glPopMatrix();
			}
		}
	}

	// swipe����drop��`��
	if (phase == PHASE_SWIPE) {
		// ���݂̃}�E�X�̍��W�ɓ������Ă���drop��`��
		DrowDrop(drops[swipingDrop.y][swipingDrop.x].drop, mousePosition, 1);
	}

	
	// �f�o�b�O�p�Fgame phase�`��
	glPushMatrix();
	{
		const char* phaseName[] = {
			"PHASE_IDLE",
			"PHASE_SWIPE",
			"PHASE_ERASE",
			"PHASE_FALL",
			"PHASE_MAX"
		};
		double s = 32.0 / 80.0;
		glTranslated(0, 100 * s, 0);
		glScaled(s, -s, 0);
		glColor3ub(0x00, 0xff, 0xff);
		glLineWidth(4);
		glutStrokeString(GLUT_STROKE_MONO_ROMAN, (const unsigned char*)phaseName[phase]);
	}
	glPopMatrix();


	glutSwapBuffers();
}

void Idle()
{
	frame++;

	// �ړ�����drop�̉�]�G�t�F�N�g����
	for (int y = 0; y < ROW_MAX; y++) {
		for (int x = 0; x < COLUMN_MAX; x++) {
			if (drops[y][x].rotate < drops[y][x].rotateMax) {
				drops[y][x].rotate += M_PI / 30;
			}

			if (drops[y][x].comboScale > COMBO_SCALE_MIN) {	// ���񂾂�R���{�\�����������Ȃ�
				drops[y][x].comboScale *= 0.95;
				if (drops[y][x].comboScale < COMBO_SCALE_MIN < 0) {
					drops[y][x].comboScale = COMBO_SCALE_MIN;
				}
			}
		}
	}

	switch (phase) {
	case PHASE_ERASE:
	{
		bool isErasing = false;	// �폜�����H
		for (int y = 0; y < ROW_MAX; y++) {
			for (int x = 0; x < COLUMN_MAX; x++) {
				if (drops[y][x].eraseAlpha > 0) {	// alpha�l�̘A���I�ω��ɂ���č폜��������
					isErasing = true;
					drops[y][x].eraseAlpha -= 1.0 / 60;
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

	glutPostRedisplay();
}

void Mouse(int, int _state, int _x, int _y)
{
	mousePosition = { _x, _y };
	switch (_state) {
	case GLUT_DOWN:
		if (phase == PHASE_IDLE) {	// IDLE��ԂŃ}�E�X�������ꂽ��
			// swipingDrop��int�^������AGRID_SIZE�Ŋ���������
			// �����I��Drop�̈ʒu�ɂȂ�
			swipingDrop = mousePosition / GRID_SIZE;
			phase = PHASE_SWIPE;
		}
		break;
	case GLUT_UP:
		if (phase == PHASE_SWIPE) {	// SWIPE��ԂŃ}�E�X�������ꂽ��
			combo = 0;

			if (EraseDrops()) {		// �폜����drop�������PHASE_ERASE�Ɉڍs
				phase = PHASE_ERASE;
			}
			else {
				phase = PHASE_IDLE;
			}
		}
		break;
	}
}

void Motion(int _x, int _y)
{
	mousePosition = { _x, _y };	// �}�E�X���h���b�O���Ă���Œ��ł��}�E�X�̍��W�𔽉f������
	if (phase == PHASE_SWIPE) {
		// ���݂�mouse�̈ʒu����e�[�u���̂ǂ��ɂ���̂����Z�o
		glm::ivec2 newDrop = mousePosition / GRID_SIZE;
		glm::ivec2 localPosition = { mousePosition.x % GRID_SIZE, mousePosition.y % GRID_SIZE };	// drop�̑��݂��Ă���}�X�̃s�N�Z�����W
		if ((newDrop != swipingDrop) &&		// �}�X�ɂ���h���b�v���V����dorp���A
			(
				!(	// �}�X��4���Ƀ}�E�X�J�[�\�������Ȃ����
					   ((localPosition.x < GRID_SIZE * 0.25) && (localPosition.y < GRID_SIZE * 0.25))
					|| ((localPosition.x >= GRID_SIZE * 0.75) && (localPosition.y < GRID_SIZE * 0.25))
					|| ((localPosition.x < GRID_SIZE * 0.25) && (localPosition.y >= GRID_SIZE * 0.75))
					|| ((localPosition.x >= GRID_SIZE * 0.75) && (localPosition.y >= GRID_SIZE * 0.75))
				)
			)
			) {	// ���ݓ������Ă���drop���V�������̂ƈقȂ��Ă����
			// ��]�G�t�F�N�g�̉�]���Ȃǂ𓱏o
			glm::vec2 swipingCenter;	// ���������Ă���drop�̒��S�̃s�N�Z�����W
			{
				glm::vec2 temp = swipingDrop;
				temp.x = (temp.x + 0.5) * GRID_SIZE;
				temp.y = (temp.y + 0.5) * GRID_SIZE;
				swipingCenter = temp;
			}
			glm::vec2 newCenter;		// ���������drop�̒��S�̃s�N�Z�����W
			{
				glm::vec2 temp = newDrop;
				temp.x = (temp.x + 0.5) * GRID_SIZE;
				temp.y = (temp.y + 0.5) * GRID_SIZE;
				newCenter = temp;
			}
			glm::vec2 center;			// ��]���S�̃s�N�Z�����W
			{
				glm::vec2 temp = swipingCenter + newCenter;
				temp.x /= 2;
				temp.y /= 2;
				center = temp;
			}
			drops[swipingDrop.y][swipingDrop.x].center =
				drops[newDrop.y][swipingDrop.x].center = center;
			glm::vec2 centerToNew = newCenter - center;
			drops[swipingDrop.y][swipingDrop.x].rotateMax = atan2(-centerToNew.y, centerToNew.x);
			drops[swipingDrop.y][swipingDrop.x].rotate =
				drops[swipingDrop.y][swipingDrop.x].rotateMax - M_PI;
			drops[newDrop.y][newDrop.x].rotate = drops[swipingDrop.y][swipingDrop.x].rotateMax;
			drops[newDrop.y][newDrop.x].rotateMax =
				drops[newDrop.y][newDrop.x].rotate + M_PI;


			// drop�f�[�^����ւ�����
			std::swap(drops[swipingDrop.y][swipingDrop.x].drop, drops[newDrop.y][newDrop.x].drop);
			swipingDrop = newDrop;
		}
	}
}

int main(int argc, char* argv[])
{
	srand((unsigned char)time(NULL));

	// drop����������
	for (int y = 0; y < ROW_MAX; y++) {
		for (int x = 0; x < COLUMN_MAX; x++) {
			drops[y][x].drop = rand() % DROP_MAX;
		}
	}

	color_count = 0;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowPosition(640, 0);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("");
	glutDisplayFunc(Display);
	glutIdleFunc(Idle);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);

	// �A���t�@�u�����h�L����
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glutMainLoop();

	return 0;
}