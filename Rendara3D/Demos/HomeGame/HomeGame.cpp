#include "HomeGame.h"


namespace homelife {


	HomeGame::HomeGame(GLuint widthIn, GLuint heightIn) :
		loadedTextures{},
		window{ "game", widthIn, heightIn, Monitor::G_NOTSPECIFIED }
	{
		width = widthIn;
		height = heightIn;
		state = HOME_GAME_MENU;
	}



	int HomeGame::execute()
	{
		

		FirstLevel first{ window, &loadedTextures };

		std::vector<Level*> levels;
		levels.push_back(&first);
		levels.at(0)->load();

		size_t level_id = 0;

		while (!window.isClosed())
		{
			// ******* first stuff to do
			window.updateTime();
			window.clearColorBufferBit(0.5f, 0.5f, 0.5f, 1.0f);


			// ******* graphics, physics and game logic
				// graphics
			levels.at(level_id)->render(window);

			// game logic: commands, events
			levels.at(level_id)->update(window);

			// ******* advance levels
			//if (levels.at(level_id).isCompleted())
			//{
			//	if (level_id < levels.size() - 1)
			//	{
			//		level_id += 1;
			//	}
			//	else
			//	{
			//		// game is finished
			//		window.close();
			//	}
			//}



			// ******* last stuff to do
			window.swapBuffers();
			window.pollEvents();
			window.updateLastFrameTime();
		}

		return 0;
	}


}