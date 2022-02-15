#pragma once

#include "SceneManager.hpp"

class GameLoop {
public:
	GameLoop(){}
	void Run()
	{
		SceneManager* scene_manager = new SceneManager();


		while (true) {
			scene_manager->draw();
			scene_manager->SceneManagerUpdate();
			scene_manager->input();
		}
	}

private:

};

//GameLoop::GameLoop()
//{
//
//
//}
//
//void GameLoop::Run()
//{
//	SceneManager* scene_manager = new SceneManager();
//
//
//	while (true) {
//		scene_manager->draw();
//		scene_manager->SceneManagerUpdate();
//		scene_manager->input();
//	}
//}