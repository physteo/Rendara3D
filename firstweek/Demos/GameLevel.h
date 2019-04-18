#pragma once

#include "GameState.h"

class GameLevel
{
protected:
	GameState m_state;

public:
	virtual ~GameLevel() {};

	virtual void render(const Window& window) = 0;
	virtual void update(Window& window) = 0;

	inline GameState getState() const { return m_state; }
};