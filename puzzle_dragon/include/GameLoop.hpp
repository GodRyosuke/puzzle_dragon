//#pragma once

// #include "SceneManager.hpp"

class GameLoop {
public:
	GameLoop();
	void Run();

	//GameLoop(){}
	//void Run()
	//{
	//	SceneManager* scene_manager = new SceneManager();


	//	while (true) {
	//		scene_manager->input();
	//		if (!scene_manager->SceneManagerUpdate()) {
	//			break;
	//		}
	//		scene_manager->draw();
	//	}

	//	scene_manager->shutdown();
	//}


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