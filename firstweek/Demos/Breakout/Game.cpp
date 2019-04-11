#include "Game.h"



Game::Game(GLuint widthIn, GLuint heightIn) :
	loadedTextures{},
	window{ "game", widthIn, heightIn, Monitor::G_NOTSPECIFIED }
{
	width = widthIn;
	height = heightIn;
	state = GAME_MENU;
}



int Game::execute()
{

	// without instance example
	std::vector< std::vector<std::vector<int> > > layouts;
	layouts.push_back({ {2,3,2,3,2,0,2,3,2,3,2,3} });
	layouts.push_back({ {2,3,2,3,2,3,2,3,2,3,2,3},
					    {3,2,3,2,3,2,3,2,3,2,3,2},
					    {1,1,1,0,1,1,0,1,1,0,1,1} });

	std::list<GameLevel> levels;
	for (auto it = layouts.begin(); it != layouts.end(); ++it)
	{
		levels.emplace_back( window, &loadedTextures );
		levels.back().load(*it);
	}


	while (!window.isClosed())
	{
		// ******* first stuff to do
		window.updateTime();
		window.clearColorBufferBit(0.5f, 0.5f, 0.5f, 1.0f);


		// ******* graphics, physics and game logic
			// graphics
		levels.front().render(window);
		
			// game logic: commands, events
		levels.front().update(window);
		
		// ******* advance levels
		if (levels.front().isCompleted())
		{
			levels.pop_front();
			if (levels.empty())
			{
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