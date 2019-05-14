#pragma once

#include "../../buffers/FrameBuffer.h"
#include "../GameObject.h"
#include "../../lighting/SunLight.h"
#include "../../lighting/PointLight.h"
#include "../../lighting/ShadowMap2D.h"
#include "../../lighting/ShadowCubeMap.h"
#include "../../Camera/Camera.h"
#include "../../Renderer/InstanceSet.h"
#include ".././ParticleSystem.h"

#include <vector>
#include <string>

#include "../../Renderer/Simple3DRenderer.h"
#include "./Players.h"

#include "../GameState.h"

class OutBreakLevel
{

	//************* parameters for initialization *************/
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
	OutBreakLevel(Window& window, std::map<std::string, Texture>* loadedTextures);

	void load(const std::vector<std::vector<int> >& layout);
	void render(Window& window);
	void update(Window& window);
	GameState status();
private:
	void processCommands(Window& window);

private:
	Simple3DRenderer simple3DRenderer;

	//************* camera *************/
	Camera camera;
	glm::mat4 projection;

	/************* game components *************/
	// player related
	Player player;
	Ball ball;

	// background
	GameObject background;

	// bricks
	InstanceSet<Brick> bricksIron;
	InstanceSet<Brick> bricksWood;
	InstanceSet<Brick> bricksPaper;

	// particles
	InstanceSet<Particle> particles;
	InstanceSetQuads<Particle> coloredQuads;


	// walls
	glm::vec3 normalVectorWall1;
	glm::vec3 offsetVectorWall1;
	glm::vec3 normalVectorWall2;
	glm::vec3 offsetVectorWall2;
	glm::vec3 normalVectorWall3;
	glm::vec3 offsetVectorWall3;

	/************** lights *************/
	// sun
	SunLight      sun;
	ShadowMap2D   sunShadowMap;
	// point light
	PointLight    pointLight;
	ShadowCubeMap pointShadow;
	
	/************* 3d models *************/
	Model playerModel;
	Model ballModel;
	Model paperBrickModel;
	Model woodBrickModel;
	Model ironBrickModel;
	Model parquetModel;
	Model quad;

	/************* shaders *************/
	// normal shaders 
	Shader objectsShader;
	Shader sunShadowShader;
	Shader cubeDepthShader;
	Shader hdrShader;

	// instance shaders
	Shader instancesObjectsShader;
	Shader instancesSunShadowShader;
	Shader instancesCubeDepthShader;
	Shader instancesColoredQuadsShader;

	/* Framebuffers */
	// HDR framebuffer
	FrameBuffer hdrFB;
	ScreenQuad hdrQuad;
};
