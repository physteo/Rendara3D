#include "Game.h"



Game::Game(GLuint widthIn, GLuint heightIn) :
	loadedTextures{},
	window{ "game", widthIn, heightIn, Monitor::G_NOTSPECIFIED }, 
	cube{ "./res/model/cube/cube.obj", glm::vec3{0.0f,1.0f,1.0f}, &loadedTextures }
{
	width = widthIn;
	height = heightIn;
	state = GAME_MENU;
	init();
}

void Game::init()
{

	// set camera
		// Perspective
	//Camera camera{ glm::vec3{-3.0f, 3.0f, 3.0f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 1.0f, 0.0f} };
	//glm::mat4 projection = glm::perspective(glm::radians(90.0f), width / height, 0.1f, 50.0f);
		// Ortho

}


int Game::execute()
{

	std::vector<std::vector<int> > layout_0 = { {2,3,2,3,2,3,2,3,2,3,2,3},
											  {3,2,3,2,3,2,3,2,3,2,3,2},
											  {1,0,1,0,0,1,0,1,0,0,0,1} };
	std::vector<std::vector<int> > layout_1 = { {2,3,2,3,2,3,2,3,2,3,2,3},
											    {3,2,3,2,3,2,3,2,3,2,3,2},
												{2,3,2,3,2,3,2,3,2,3,2,3},
											    {1,0,1,0,0,1,0,1,0,0,0,1} };



	std::vector<GameLevel> levels;
	levels.emplace_back( window, &loadedTextures );
	levels.emplace_back( window, &loadedTextures );
	levels.at(0).load(layout_0);
	levels.at(1).load(layout_1);

	size_t level_id = 0;

	while (!window.isClosed())
	{
		// ******* first stuff to do
		window.updateTime();
		window.clearColorBufferBit(0.5f, 0.5f, 0.5f, 1.0f);

		// ******* graphics, physics and game logic
		levels.at(level_id).render(window);
		
		// ******* game logic: commands, physics, etc
		levels.at(level_id).update(window);
		
		// ******* advance levels
		if (levels.at(level_id).isCompleted())
		{
			if (level_id < levels.size() - 1)
			{
				level_id += 1;
			}
			else
			{
				// game is finished
				window.close();
			}
		}

		// ******* last stuff to do
		window.swapBuffers();
		window.pollEvents();
		window.updateLastFrameTime();
	}

	return 0;
}