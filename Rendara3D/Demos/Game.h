#pragma once

// stl
#include <vector>
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
	std::list<std::unique_ptr<GameLevel> >  m_levels;
	std::vector<Texture>					m_loadedTextures;
	Window									m_window;

	virtual void load() = 0;
};
