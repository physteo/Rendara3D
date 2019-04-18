#include "ShadowsDemoLevel.h"

ShadowsDemoLevel::ShadowsDemoLevel(const Window& window, std::vector<Texture>& loadedTextures)
{
	/* game status */
	m_state = GameState::GAME_ACTIVE;

	/* camera and view */
	camera = Camera{ glm::vec3{3.0f, 3.0f, 3.0f}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f} };
	projection = glm::perspective(glm::radians(90.0f), (float)(window.getWidth() / window.getWidth()), 0.1f, 50.0f);

	/* models */
	cube = std::move(Model{ "./res/model/container/container_hard.obj",glm::vec3{115.,194.,251.} / 255.0f, &loadedTextures });
	sphere = std::move(Model{ "./res/model/sphere/sphere.obj", &loadedTextures });
	parquet = std::move(Model{ "./res/model/parquet/parquet.obj", &loadedTextures });
	cubeTransform = { { +0.686f,0.6655f, 1.0f }, glm::vec3{0.0f}, glm::vec3{1.2f} };
	sphereTransform = { { +1.6182f, 0.3465f, 0.3693f }, glm::vec3{0.0f}, glm::vec3{1.6182f} };
	parquetTransform = { { 0.0f,0.0f,0.0f }, glm::vec3{0.0f}, glm::vec3{1.0f} };

	/* shaders */
	shadowShader = std::move(Shader{ "./res/shaders/depth.shader" });
	cubeMapShader = std::move(Shader{ "./res/shaders/cubemap.shader" });
	lampShader = std::move(Shader{ "./res/shaders/1_lamp.shader" });
	cubeDepthShader = std::move(Shader{ "./res/shaders/cubeDepth.shader" });
	shader = std::move(Shader{ "./res/shaders/objects_wlights.shader" });
	debugDepth = std::move(Shader{ "./res/shaders/debugDepth.shader" });
	hdrShader = std::move(Shader{ "./res/shaders/hdr.shader" });
	hdrShader.bind();
	hdrShader.setUniformValue("exposure", 0.5f);
	hdrShader.unbind();

	/* lights */
	sun = SunLight{ glm::vec3{+4.08030128,3.94482803,-2.77766037},
					glm::vec3{0.0f,0.0f,0.0f}, shadowsDemoParams::sunAmbient0,
					shadowsDemoParams::sunDiffuse0, shadowsDemoParams::sunSpecular };

	sunShadow = std::move(ShadowMap2D{ shadowsDemoParams::sun_nearPlane, shadowsDemoParams::sun_farPlane,
									   shadowsDemoParams::sun_left,      shadowsDemoParams::sun_right,
									   shadowsDemoParams::sun_down,      shadowsDemoParams::sun_up,     1024, 1024 });

	pointLight = PointLight{ glm::vec3{+1.0f, 2.0f, 0.0f},        shadowsDemoParams::pointLightAmbient,
						   shadowsDemoParams::pointLightDiffuse,  shadowsDemoParams::pointLightSpecular,
						   shadowsDemoParams::pointLightConstant, shadowsDemoParams::pointLightLinear,
						   shadowsDemoParams::pointLightQuadratic };
	//pointLightShadow = std::move(ShadowCubeMap(1024, 1024));

	/* HDR */
	hdrFB.attach2DTexture(GL_COLOR_ATTACHMENT0, window.getWidth(), window.getHeight(), 4, RGBA16, GL_FLOAT);
	hdrFB.attachRenderBuffer(GL_DEPTH_COMPONENT, window.getWidth(), window.getHeight());
	if (!hdrFB.iscomplete())
	{
		std::cerr << "Framebuffer not complete!" << std::endl;
		// TODO: add proper logging
	}

}


void ShadowsDemoLevel::render(const Window& window)
{
	// submit to simple renderer non instanced objects
	simple3DRenderer.submit({ &cube, cubeTransform      , &shader });
	simple3DRenderer.submit({ &sphere, sphereTransform  , &shader });
	simple3DRenderer.submit({ &parquet, parquetTransform, &shader });
	simple3DRenderer.submit({ &cube, Transform{ pointLight.eye, glm::vec3{0.0f}, glm::vec3{.02f} }, &lampShader });

	// clear the shadow maps
	sunShadow.clearShadows();
	pointLightShadow.clearShadows();

	// draw shadowmaps
	sunShadow.startShadows(window, shadowShader, &sun);
	simple3DRenderer.draw(&shadowShader);
	sunShadow.stopShadows(window, shadowShader);

	pointLightShadow.startShadows(window, cubeDepthShader, pointLight);
	simple3DRenderer.draw(&cubeDepthShader);
	pointLightShadow.stopShadows(window, cubeDepthShader);

	// prepare shader for objects
	shader.bind();
	shader.setUniformMatrix("projection", projection, false);
	shader.setUniformValue("cameraPos", camera.getEye());
	shader.setUniformMatrix("view", camera.getViewMatrix(), false);

	sun.cast("sun[0]", shader);
	sunShadow.passUniforms(shader, "shadowMap[0]", "lightSpaceMatrix[0]", sun.getViewMatrix());
	pointLight.cast("pointLights[0]", shader);
	pointLightShadow.passUniforms(shader, "cubeDepthMap[0]", "farPlane");
	shader.unbind();

	// activate hdr framebuffer
	hdrFB.bind();
	window.clearColorBufferBit(0.5f, 0.5f, 0.5f, 1.0f);
	// draw stuff
	simple3DRenderer.draw();
	// lamps's shaders
	lampShader.bind();
	lampShader.setUniformMatrix("view", camera.getViewMatrix(), false);
	lampShader.setUniformMatrix("projection", projection, false);
	lampShader.unbind();

	// disable HDR framebuffer
	hdrFB.unbind();

	// render on screen
	hdrShader.bind();
	hdrShader.setTexture(GL_TEXTURE_2D, "hdrBuffer", hdrFB.getAttachedTextureID(0));
	hdrQuad.draw();
	hdrShader.unbind();

	// clear renderers
	simple3DRenderer.clear();

}

void ShadowsDemoLevel::update(Window& window)
{
	camera.processCommands(window);
	controlVector(window, 2.0f, GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_X, GLFW_KEY_Z, GLFW_KEY_RIGHT, GLFW_KEY_LEFT, pointLight.eye);
	//float newEye[3] = { camera.getEye().x, camera.getEye().y, camera.getEye().z };
	//float newCenter[3] = { camera.getCenter().x, camera.getCenter().y, camera.getCenter().z };
	//float newUp[3] = { camera.getUp().x, camera.getUp().y, camera.getUp().z };
	//
	//double mouseX;
	//double mouseY;
	//double oldMouseX;
	//double oldMouseY;
	//window.getCurrentMousePosition(mouseX, mouseY);
	//window.getLastMousePosition(oldMouseX, oldMouseY);
	//
	//arcball_camera_update(newEye, newCenter, newUp, nullptr, window.getLastFrameTime(), 1.0, 1.0, 1.0, window.getWidth(), window.getHeight(), oldMouseX, mouseX, oldMouseY, mouseY, 1, 0, 1.0f, 0);
	//
	//camera.setEye(glm::vec3{newEye[0], newEye[1], newEye[2]});
	//camera.setCenter(glm::vec3{newCenter[0], newCenter[1], newCenter[2]});
	//camera.setUp(glm::vec3{newUp[0], newUp[1], newUp[2]});
}