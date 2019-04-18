
#include "../Game.h"
#include "InstancingDemoLevel.h"

class InstancingDemoGame : public Game
{
public:
	InstancingDemoGame(GLuint widthIn, GLuint heightIn) : Game{ widthIn, heightIn } {}

private:
	void load() override
	{
		this->m_levels.push_back(std::make_unique<InstancingDemoLevel>(InstancingDemoLevel{ this->m_window, this->m_loadedTextures }));
	}
};