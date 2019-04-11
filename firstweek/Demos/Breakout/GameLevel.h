#pragma once

#include "../../buffers/FrameBuffer.h"
#include "../GameObject.h"
#include "../../lighting/SunLight.h"
#include "../../lighting/PointLight.h"
#include "../../lighting/ShadowMap2D.h"
#include "../../lighting/ShadowCubeMap.h"
#include "../../Camera.h"
#include "../../Quad.h"
#include "../../Renderer/InstanceSet.h"
#include ".././ParticleSystem.h"

#include <vector>
#include <string>

#include "../../Renderer/Simple3DRenderer.h"
#include "./Players.h"

class GameLevel
{

	// for initialization
	const glm::vec3 sunPosition = glm::vec3{ +0.0f, 10.0f, 0.0f };
	const glm::vec3 sunCenter = glm::vec3{ 5.0f,0.0f,5.0f };
	const glm::vec3 pointLightPosition = glm::vec3{ +5.0f, 2.0f, 5.0f };
	const glm::vec3 orangeColor = glm::vec3{ 249.f, 113.f, 255.f } / 255.0f;
	const glm::vec3 blueColor   = glm::vec3{ 255.f, 255.f, 0.f } / 255.0f;
	const glm::vec3 whiteColor = glm::vec3{ 255.f, 255.f, 255.f } / 255.0f;
	
	const glm::vec2 ratio = {7.0f, 3.0f};
	const glm::vec3 white_orange_color = (ratio.x * orangeColor + ratio.y * whiteColor) / (ratio.x + ratio.y);

	const glm::vec3 sunAmbient0 = 1.0f * 0.1f * white_orange_color;
	const glm::vec3 sunDiffuse0 = 1.0f * 0.8f * white_orange_color;
	const glm::vec3 sunSpecular = 1.0f * 1.0f * whiteColor;
	const glm::vec3 ambient =  1.0f * 0.1f * 0.1f * glm::vec3{ 1.0f, 1.0f, 1.0f };
	const glm::vec3 diffuse =  1.0f * 1.0f * 0.8f * glm::vec3{ 1.0f, 1.0f, 1.0f };
	const glm::vec3 specular = 1.0f * 1.0f * 1.0f * glm::vec3{ 1.0f, 1.0f, 1.0f };
	const float     constant  = 0.001f;
	const float     linear    = 0.05f;
	const float     quadratic = 0.1f;
	const float     sun_nearPlane = 0.1f; float sun_farPlane = 20.0f; float sun_left = -10.0f; float sun_right = 10.0f; float sun_down = -10.0f; float sun_up = 10.0f;


public:
	Simple3DRenderer simple3DRenderer;
	GameObject newBall;


	// camera
	Camera camera;
	glm::mat4 projection;

	// game components
	//std::vector<Brick> bricks;
	InstanceSet<Brick> bricksIron;
	InstanceSet<Brick> bricksWood;
	InstanceSet<Brick> bricksPaper;

	// particles
	InstanceSet<Particle> particles;
	InstanceSetQuads<Particle> coloredQuads;

	GameObject background;
	
	

	// walls
	glm::vec3 normalVectorWall1;
	glm::vec3 offsetVectorWall1;
	glm::vec3 normalVectorWall2;
	glm::vec3 offsetVectorWall2;
	glm::vec3 normalVectorWall3;
	glm::vec3 offsetVectorWall3;

	Player player;
	Ball ball;

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
	Model ballModel;
	Model blueBrick;
	Model orangeBrick;
	Model ironBrick;
	Model parquet;
	Model quad;

	// shaders
	Shader objectsShader;
	Shader sunShadowShader;
	Shader cubeDepthShader;
	Shader hdrShader;
	Shader debugDepth;

	Shader instancesObjectsShader;
	Shader instancesSunShadowShader;
	Shader instancesCubeDepthShader;
	Shader instancesColoredQuadsShader;



	// for HDR
	FrameBuffer hdrFB;
	ScreenQuad hdrQuad;

};
