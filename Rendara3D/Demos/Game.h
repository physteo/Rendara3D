#pragma once

// stl
//#include <vector>
#include <map>
#include <list>
#include <memory>

// rendering engine
#include "../Window/Window.h"
#include "../Texture/Texture.h"
#include "GameLevel.h"
#include "GameState.h"

class Game
{
public:

	Game(GLuint widthIn, GLuint heightIn);
	virtual ~Game() {}

	// Executes the game, first loading the levels through the load function
	// and then cycling through the list of GameLevels
	void execute();

protected:
	std::map<std::string, Texture>			m_loadedTextures;
	std::list<std::unique_ptr<GameLevel> >  m_levels;
	Window									m_window;

	virtual void load() = 0;
};
