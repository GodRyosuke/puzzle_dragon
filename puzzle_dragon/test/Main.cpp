//#include "GameLoop.hpp"
#include <iostream>
#include "SceneManager.hpp"

int main(int argc, char** argv)
{
	SceneManager* scene_manager = new SceneManager();


	if (!scene_manager->Initialize()) {
		printf("error: failed to initialize Scene Manager\n");
	}
	while (true) {
		scene_manager->input();
		if (!scene_manager->SceneManagerUpdate()) {
			break;
		}
		scene_manager->draw();
	}

	scene_manager->shutdown();


	//GameLoop game_loop;

	//game_loop.Run();


	//Game game;
	//bool success = game.Initialize();
	//if (success) {
	//	game.RunLoop();
	//}

	//game.Shutdown();

	return 0;
}