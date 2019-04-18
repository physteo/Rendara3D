#pragma once

/* maths */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

/* rendering engine */
#include "../../utils/ErrorHandling.h"
#include "../../Window/Window.h"
#include "../../Camera/Camera.h"
#include "../../Camera/arcball.h"
#include "../../Model/Model.h"
#include "../../lighting/SunLight.h"
#include "../../lighting/ShadowMap2D.h"
#include "../../lighting/ShadowCubeMap.h"
#include "../../buffers/FrameBuffer.h"
#include "../../Renderer/Simple3DRenderer.h"
#include "../GameLevel.h"

/* stl */
#include <vector>



namespace shadowsDemoParams
{
	const float sun_nearPlane = 0.1f;
	const float sun_farPlane = 20.0f;
	const float sun_left = -10.0f;
	const float sun_right = 10.0f;
	const float sun_down = -10.0f;
	const float sun_up = 10.0f;
	const glm::vec3 colorWhite{ 1.0f };
	const glm::vec3 sunAmbient0 = 0.1f * colorWhite;
	const glm::vec3 sunDiffuse0 = 0.8f * colorWhite;
	const glm::vec3 sunSpecular = 1.0f * colorWhite;

	const glm::vec3 pointLightAmbient = 1.0f * 0.1f * 0.1f * glm::vec3{ 1.0f, 1.0f, 1.0f };
	const glm::vec3 pointLightDiffuse = 1.0f * 1.0f * 0.8f * glm::vec3{ 1.0f, 1.0f, 1.0f };
	const glm::vec3 pointLightSpecular = 1.0f * 1.0f * 1.0f * glm::vec3{ 1.0f, 1.0f, 1.0f };
	const float pointLightConstant = 0.001f;
	const float pointLightLinear = 0.05f;
	const float pointLightQuadratic = 0.1f;

}


class ShadowsDemoLevel : public GameLevel
{
public:
	ShadowsDemoLevel(const Window& window, std::vector<Texture>& loadedTextures);
	void render(const Window& window) override;
	void update(Window& window) override;

	// demo specific data
	// Camera and view
	Camera    camera;
	glm::mat4 projection;

	// 3D models
	Model cube;
	Model sphere;
	Model parquet;
	Transform cubeTransform;
	Transform sphereTransform;
	Transform parquetTransform;

	// Shaders
	Shader hdrShader;
	Shader shadowShader;
	Shader cubeMapShader;
	Shader lampShader;
	Shader cubeDepthShader;
	Shader shader;
	Shader debugDepth;

	// Renderers
	Simple3DRenderer simple3DRenderer;

	// lights
	SunLight      sun;
	ShadowMap2D   sunShadow;
	PointLight    pointLight;
	ShadowCubeMap pointLightShadow;

	// HDR
	FrameBuffer hdrFB;
	ScreenQuad  hdrQuad;


};
