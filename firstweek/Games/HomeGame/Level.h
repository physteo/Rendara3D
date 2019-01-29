#pragma once

#include "../GameObject.h"
#include "../HomeGame/HomePlayer.h"

#include "../../SunLight.h"
#include "../../PointLight.h"
#include "../../ShadowMap2D.h"
#include "../../ShadowCubeMap.h"
#include "../../Camera.h"
#include "../../Quad.h"
#include "../InstanceSet.h"
#include "./../ParticleSystem.h"
#include "LevelState.h"

// sounds
#include <irrKlang.h>

#include <vector>
#include <string>



class Level
{
	const float cameraDistance = 2.5f;
	const glm::vec3 cameraEyeOutside    = glm::vec3{ +0.0f, 0.0f, cameraDistance };
	const glm::vec3 cameraCenterOutside = glm::vec3{ +0.0f, 0.0f, 0.0f };
	const glm::vec3 cameraUpOutside     = glm::vec3{ +0.0f, 1.0f, 0.0f };

	const glm::vec3 cameraEyeInside    = glm::vec3{ +0.0f, cameraDistance, -cameraDistance };
	const glm::vec3 cameraCenterInside = glm::vec3{ +0.0f, 0.0f, -cameraDistance };
	const glm::vec3 cameraUpInside     = glm::vec3{ +0.0f, 0.0f, -1.0f };


	// for initialization
	const glm::vec3 sunPosition = glm::vec3{ +0.0f, 0.0f, 10.0f };
	const glm::vec3 sunCenter = glm::vec3{ 0.0f,0.0f,0.0f };
	const glm::vec3 pointLightPosition = glm::vec3{ 6.0f, -2.0f, -1.5f };
	//const glm::vec3 pointLightPosition = glm::vec3{ 4.0f, -2.0f, -4.5f };
	const glm::vec3 orangeColor = glm::vec3{ 249.f, 113.f, 255.f } / 255.0f;
	const glm::vec3 blueColor = glm::vec3{ 255.f, 255.f, 0.f } / 255.0f;
	const glm::vec3 whiteColor = glm::vec3{ 255.f, 255.f, 255.f } / 255.0f;

	const glm::vec2 ratio = { 7.0f, 3.0f };
	const glm::vec3 white_orange_color = (ratio.x * orangeColor + ratio.y * whiteColor) / (ratio.x + ratio.y);

	const glm::vec3 sunAmbient0 = 0.1f * 0.1f * white_orange_color;
	const glm::vec3 sunDiffuse0 = 0.1f * 0.8f * white_orange_color;
	const glm::vec3 sunSpecular = 0.1f * 1.0f * whiteColor;
	const glm::vec3 ambient = 1.0f * 0.1f * 0.1f * glm::vec3{ 1.0f, 1.0f, 1.0f };
	const glm::vec3 diffuse = 1.0f * 1.0f * 0.8f * glm::vec3{ 1.0f, 1.0f, 1.0f };
	const glm::vec3 specular = 1.0f * 1.0f * 1.0f * glm::vec3{ 1.0f, 1.0f, 1.0f };
	const float     constant = 0.001f;
	const float     linear = 0.05f;
	const float     quadratic = 0.1f;
	const float     sun_nearPlane = 0.1f; float sun_farPlane = 20.0f; float sun_left = -10.0f; float sun_right = 10.0f; float sun_down = -10.0f; float sun_up = 10.0f;

	// sounds
	irrklang::ISoundEngine *SoundEngine;

public:

	float completionTime;
	float completedTime;
	LevelState levelState;

	// camera
	Camera camera;
	glm::mat4 projection;

	float lastChangingStateTime;

	// game components


	// particles
	InstanceSet<Particle> particles;
	InstanceSetQuads<Particle> coloredQuads;


	GameObject background;
	GameObject outsideBackground;
	GameObject insideBackground;

	Transform mum;
	bool mumMakesPizza;

	Transform oven;
	
	std::vector<Transform> tiles;
	std::vector<home::Grass> grassTiles;

	memory::ObjectPool<home::Enemy> enemies;

	bool tomatoOutside;
	bool doeOutside;
	home::Tomato tomato;
	home::Doe doe;

	HomePlayer player;
	Ball ball;

	// lights
	SunLight sun;
	OrthoFrustrum orthoFrustrum;
	ShadowMap2D sunShadowMap;

	PointLight pointLight;
	ShadowCubeMap pointShadow;


	// Textures and materials for quads
	Texture  defaultSpecularMap;
	Texture  defaultNormalMap;

	Texture  playerTexture;
	Material playerMaterial;


	Mesh     groundMesh;
	Mesh     quadSheet4x8Mesh;

	Texture  groundWithGrassTexture;
	Material groundWithGrassMaterial;
	Texture  grassTexture;
	Material grassMaterial;
	Texture  backgroundOutsideTexture;
	Material  backgroundOutsideMaterial;

	Texture  backgroundInsideTexture;
	Material  backgroundInsideMaterial;

	Texture gameOverTexture;
	Material gameOverTextureMaterial;

	Texture gameCompletedTexture;
	Material gameCompletedTextureMaterial;

	Texture pressBTexture;
	Material pressBMaterial;

	Texture completionBarTexture;
	Material completionBarMaterial;

	Texture emptyBarTexture;
	Material emptyBarMaterial;


	float gameOverTime;
	const float gameOverDuration = 5.0f;
	bool gameOverCalled;
	bool gameCompletedCalled;

	Transform pizza;
	Texture  pizzaTexture;
	Material pizzaMaterial;


	Texture  itemsTexture;
	Material itemsMaterial;

	Texture  enemiesTexture;
	Material enemiesMaterial;

	Texture  insideTexture;
	Material insideMaterial;


	Texture  whiteTexture;
	Material whiteMaterial;


	int rows;
	int columns;

	Level(Window& window, std::vector<Texture>* loadedTextures);

	void load(const std::vector<std::vector<int> >& layout);
	void render(Window& window);
	void update(Window& window);
	bool isCompleted();
private:
	void processCommands(Window& window);
	void gameOver(Window& window);
	void gameCompleted(Window& window);
	void advanceParticles(float t, float dt);
	void spawnEnemies(float t, float dt, float enemySpeed);
private:
	// models
	Model playerModel;

	Model ballModel;
	Model blueBrick;
	Model orangeBrick;
	Model ironBrick;
	Model parquet;
	Model quad;

	Model outsideBackgroundModel;

	// shaders
	Shader objectsShader;
	Shader sunShadowShader;
	Shader cubeDepthShader;
	Shader hdrShader;

	Shader instancesObjectsShader;
	Shader instancesSunShadowShader;
	Shader instancesCubeDepthShader;
	Shader instancesColoredQuadsShader;


	Shader playerShader;
	Shader defaultQuadShader;
	Shader defaultQuadShader4x8;


	// for HDR
	unsigned int colorBufferTexture;
	unsigned int hdrFBO;
	Quad hdrQuad;

};

