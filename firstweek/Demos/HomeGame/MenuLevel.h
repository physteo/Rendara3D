#pragma once

#include "Level.h"
#include "../../Texture.h"
#include "../../Model.h"

namespace homelife
{

	class MenuLevel : public Level
	{
		// default textures for quads
		Texture defaultNormalMap;
		Texture defaultSpecularMap;

		// textures
		Texture menuScreenTexture;
		Material menuScreenMaterial;

		// quad
		Model quad;

	public:

		MenuLevel(Window& window, std::vector<Texture>* loadedTextures) : quad("", loadedTextures)
		{
			defaultSpecularMap.generate("./res/textures/black.jpg", Format::RGBA);
			defaultNormalMap.generate("./res/textures/default_norm.png", Format::RGBA);

			menuScreenTexture.generate("", Format::sRGB);
			menuScreenMaterial.fill(menuScreenTexture, defaultSpecularMap, defaultNormalMap, 1.0f);

		}

		virtual void load() override;
		virtual void render(Window& window) override;
		virtual void update(Window& window) override;
		virtual void gameOver(Window& window) override;
		virtual void gameCompleted(Window& window) override;
	};

}