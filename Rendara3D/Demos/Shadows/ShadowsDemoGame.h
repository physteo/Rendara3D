
#include "../Game.h"
#include "ShadowsDemoLevel.h"

class ShadowsDemoGame : public Game
{
public:
	ShadowsDemoGame(GLuint widthIn, GLuint heightIn) : Game{ widthIn, heightIn } {}

private:
	void load() override
	{
		this->m_levels.push_back(std::make_unique<ShadowsDemoLevel>(ShadowsDemoLevel{ this->m_window, this->m_loadedTextures }));
	}
};