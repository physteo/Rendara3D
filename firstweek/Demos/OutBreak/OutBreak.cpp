#include "OutBreak.h"



OutBreak::OutBreak(GLuint widthIn, GLuint heightIn) :
	loadedTextures{},
	window{ "game", widthIn, heightIn, Monitor::G_NOTSPECIFIED }
{
}


void OutBreak::execute()
{
	loadedTextures.reserve(100);
	// without instance example
	std::vector< std::vector<std::vector<int> > > levelLayouts;
	levelLayouts.push_back({ {2,3,2,3,2,0,2,3,2,3,2,3} });

	levelLayouts.push_back({ {2,3,2,3,2,3,2,3,2,3,2,3},
					         {3,2,3,2,3,2,3,2,3,2,3,2},
					         {1,1,1,0,1,1,0,1,1,0,1,1} });

	std::list<OutBreakLevel> levels;
	for (auto it = levelLayouts.begin(); it != levelLayouts.end(); ++it)
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
		if (levels.front().status() == GAME_WIN)
		{
			levels.pop_front();
			if (levels.empty())
			{
				window.close();
			}
		}
		else if (levels.front().status() == GAME_LOST)
		{
			window.close();
		}

		// ******* last stuff to do
		window.swapBuffers();
		window.pollEvents();
		window.updateLastFrameTime();
	}

}