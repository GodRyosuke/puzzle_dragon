#include "Title.hpp"

#include <conio.h>
class Game;


Title::Title(CommonData* const commonData)
	:Scene(commonData)
{

}

Title::~Title()
{

}

Scene* Title::update()
{
	char c = _getch();

	bool moveGame = true;
	switch (c) {
	case '1':
		mCommonData->setPlayerNum(1);
		break;
	case '2':
		mCommonData->setPlayerNum(2);
		break;
	case '3':
		mCommonData->setPlayerNum(3);
		break;
	default:
		moveGame = false;
	}
	if (moveGame) {
		Scene* game = makeScene<Game>();
		return game;
	}

	return this;
}

void Title::draw()
{
	system("cls");
	std::cout << "Select platers\n";
	std::cout << "1: plater1\n2: player2\n3: player3\n";
}

void Title::input()
{
	std::cout << "input called\n";

}