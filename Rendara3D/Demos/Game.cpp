#include "Game.h"

Game::Game(GLuint widthIn, GLuint heightIn) :
	m_window{ "game", widthIn, heightIn, Monitor::G_NOTSPECIFIED } {}


void Game::execute()
{
	// load levels
	load();

	while (!m_window.isClosed())
	{
		// ******* initialize frame
		m_window.updateTime();
		m_window.clearColorBufferBit(0.5f, 0.5f, 0.5f, 1.0f);


		// ******* graphics, physics and game logic
			// graphics
		m_levels.front()->render(m_window);

		// game logic: commands, events
		m_levels.front()->update(m_window);

		// ******* advance levels
		if (m_levels.front()->getState() == GAME_WIN)
		{
			m_levels.pop_front();
			if (m_levels.empty())
			{
				m_window.close();
			}
		}
		else if (m_levels.front()->getState() == GAME_LOST)
		{
			m_window.close();
		}

		// ******* last stuff to do
		m_window.swapBuffers();
		m_window.pollEvents();
		m_window.updateLastFrameTime();
	}

}