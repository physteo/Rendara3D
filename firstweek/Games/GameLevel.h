#pragma once

#include "GameComponent.h"
#include "../SunLight.h"
#include "../PointLight.h"
#include "../ShadowMap2D.h"
#include "../ShadowCubeMap.h"
#include "../Camera.h"
#include "../Quad.h"

#include <vector>
#include <string>



class GameLevel
{

	// for initialization
	const glm::vec3 sunPosition = glm::vec3{ +0.0f, 10.0f, 0.0f };
	const glm::vec3 sunCenter = glm::vec3{ 5.0f,0.0f,5.0f };
	const glm::vec3 pointLightPosition = glm::vec3{ +5.0f, 1.0f, 5.0f };
	const glm::vec3 orangeColor = glm::vec3{ 249.f, 113.f, 255.f } / 255.0f;
	const glm::vec3 blueColor = glm::vec3{ 255.f, 255.f, 0.f } / 255.0f;
	const glm::vec3 sunAmbient0 = 0.6f * 0.1f * orangeColor;
	const glm::vec3 sunDiffuse0 = 0.6f * 0.8f * orangeColor;
	const glm::vec3 sunSpecular = 0.6f * 1.0f * glm::vec3{ 1.0f, 1.0f, 1.0f };
	const glm::vec3 ambient = 1.0f * 0.1f * 0.1f * glm::vec3{ 1.0f, 1.0f, 1.0f };
	const glm::vec3 diffuse = 1.0f * 1.0f * 0.8f * glm::vec3{ 1.0f, 1.0f, 1.0f };
	const glm::vec3 specular = 1.0f * 1.0f * 1.0f * glm::vec3{ 1.0f, 1.0f, 1.0f };
	const float     constant = 0.001f;
	const float     linear = 0.05f;
	const float     quadratic = 0.1f;
	const float     sun_nearPlane = 0.1f; float sun_farPlane = 20.0f; float sun_left = -10.0f; float sun_right = 10.0f; float sun_down = -10.0f; float sun_up = 10.0f;


public:
	// camera
	Camera camera;
	glm::mat4 projection;

	// game components
	std::vector<GameComponent> bricks;
	GameComponent background;

	// game components
	Player player;

	// lights
	SunLight sun;
	OrthoFrustrum orthoFrustrum;
	ShadowMap2D sunShadowMap;

	PointLight pointLight;
	ShadowCubeMap pointShadow;

	int rows;
	int columns;

	GameLevel(Window& window, std::vector<Texture>* loadedTextures);

	void load(const std::vector<std::vector<int> >& layout);
	void render(Window& window);
	void update(Window& window);
	bool isCompleted();
private:
	void processCommands(Window& window);

private:
	// models
	Model playerModel;

	Model blueBrick;
	Model orangeBrick;
	Model ironBrick;
	Model parquet;

	// shaders
	Shader objectsShader;
	Shader sunShadowShader;
	Shader cubeDepthShader;
	Shader hdrShader;

	// for HDR
	unsigned int colorBufferTexture;
	unsigned int hdrFBO;
	Quad hdrQuad;


};






// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------

void renderQuadi();