#include "GameLevel.h"



GameLevel::GameLevel(Window& window, std::vector<Texture>* loadedTextures) :
	orangeBrick{ "./res/model/cube/cube.obj", glm::vec3{1.0f,0.0f,1.0f}, loadedTextures },
	blueBrick{ "./res/model/cube/cube.obj", glm::vec3{0.2f,0.2f,8.0f}, loadedTextures },
	ironBrick{ "./res/model/cube/cube.obj", glm::vec3{0.5f,0.5f,0.5f}, loadedTextures },
	parquet{ "./res/model/parquet/parquet.obj", loadedTextures },
	playerModel{ "./res/model/cube/cube.obj", glm::vec3{1.0f,0.0f,0.0f}, loadedTextures },
	sun{ sunPosition, sunCenter, sunAmbient0, sunDiffuse0, sunSpecular},
	pointLight{ pointLightPosition, ambient, diffuse, specular, constant, linear, quadratic },
	// first create a "frustrum", it basically specifies a matrix to project shadows
	orthoFrustrum{ sun_nearPlane, sun_farPlane, sun_left, sun_right, sun_down, sun_up },
	// now create a shadowMap (it's basically a texture containing the info about what is lit what is not)
	sunShadowMap{ orthoFrustrum, 1024.0, 1024.0 },
	// cube shadow map
	pointShadow{ 1024, 1024 },
	hdrQuad{}
{



	// generate textures for shadows
	sunShadowMap.generate();
	pointShadow.generate();

	// create shaders
	objectsShader.generate("./res/shaders/objects_practice.shader");
	sunShadowShader.generate("./res/shaders/depthShader.shader");
	cubeDepthShader.generate("./res/shaders/cubeDepth.shader");
	hdrShader.generate("./res/shaders/hdr.shader");


	// create the HDRframebuffer
	
	glGenFramebuffers(1, &hdrFBO);
	glGenTextures(1, &colorBufferTexture);
	glBindTexture(GL_TEXTURE_2D, colorBufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, window.getWidth(), window.getHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// create depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window.getWidth(), window.getHeight());
	// attach buffers
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBufferTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	hdrShader.bind();
	hdrShader.setUniformValue("exposure", 1.0f);
	hdrShader.unbind();

}

void GameLevel::load(const std::vector<std::vector<int> >& layout)
{

//	std::vector<std::vector<int> > layout = { {2,3,2,3,2,3,2,3,2,3,2,3},
//											  {3,2,3,2,3,2,3,2,3,2,3,2},
//											  {1,0,1,0,0,1,0,1,0,0,0,1} };
    // 0 : empty
	// 1 : non destroyable
	// 2+: destroyable. Each number differs in color
	// 100+: for later: special bricks

	//int numRows = layout.size();
	int maxCols = 0;

	for (size_t i = 0; i < layout.size(); i++)
	{
		for (size_t j = 0; j < layout.at(i).size(); j++)
		{
			if (layout.at(i).size() > maxCols)
			{
				maxCols = layout.at(i).size();
			}

			if (layout.at(i).at(j) != 0)
			{
				GameComponent brick;
				
				if (layout.at(i).at(j) == 1)
				{
					brick.model = &ironBrick;
				}
				else if (layout.at(i).at(j) == 2)
				{
					brick.model = &orangeBrick;
				}
				else if (layout.at(i).at(j) == 3)
				{
					brick.model = &blueBrick;
				}
				
				brick.position = glm::vec3{i,0,j};
				brick.velocity = glm::vec3{ 0.0f };
				brick.scale = 0.5f;
				bricks.push_back(brick);
			}
		}
	}


	// construct the proper orthographic projection
	float bricksize = 1.0;
	projection = glm::ortho(-(maxCols)-0.5f * bricksize + 1, 0.0f + 0.5f * bricksize,
		-(maxCols)-0.5f * bricksize + 1, 0.0f + 0.5f, 1.0f, 10.0f);

	// background initialization
	background.scale = 1.0 * 1.2;
	background.position = glm::vec3{ maxCols / 2.0 - 0.5f * bricksize, 0.0f,maxCols / 2.0 - 0.5f * bricksize };
	background.rotation = glm::vec3{ 0.0f,0.0f,0.0f };
	background.velocity = glm::vec3{ 0.0f,0.0f,0.0f };
	background.model = &parquet;

	// player initialization
	player.scale = 1.0;
	player.position = glm::vec3{ 3.0f,0.0f,0.0f };
	player.rotation = glm::vec3{ 0.0f,0.0f,0.0f };
	player.velocity = glm::vec3{ 0.0f,0.0f,0.0f };
	player.model = &playerModel;

	camera = Camera{ glm::vec3{0.0f, 5.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{-1.0f, 0.0f, 0.0f} };


}

void GameLevel::render(Window& window)
{

	// calculate sunlight's shadows
	sunShadowMap.startShadows(window, sunShadowShader, &sun);
	player.model->draw(player.scale, player.position, player.rotation, sunShadowShader);
	// render bricks
	for (size_t i = 0; i < bricks.size(); i++)
	{
		GameComponent& brick = bricks.at(i);
		brick.model->draw(brick.scale, brick.position, brick.velocity, sunShadowShader);
	}
	// render background
	background.model->draw(background.scale, background.position, background.rotation, sunShadowShader);
	sunShadowMap.stopShadows(window, sunShadowShader);


	// calculate pointlight's shadows
	pointShadow.startShadows(window, cubeDepthShader, pointLight);
	player.model->draw(player.scale, player.position, player.rotation, cubeDepthShader);
	// render bricks
	for (size_t i = 0; i < bricks.size(); i++)
	{
		GameComponent& brick = bricks.at(i);
		brick.model->draw(brick.scale, brick.position, brick.velocity, cubeDepthShader);
	}
	// render background
	background.model->draw(background.scale, background.position, background.rotation, cubeDepthShader);
	pointShadow.stopShadows(window, cubeDepthShader);


	window.clearColorBufferBit(0.5f, 0.5f, 0.5f, 1.0f);


	// render the scene

	objectsShader.bind();
	objectsShader.setUniformMatrix("view", camera.getViewMatrix(), false);
	objectsShader.setUniformMatrix("projection", projection, false);
	objectsShader.setUniformMatrix("view", camera.getViewMatrix(), false);
	objectsShader.setUniformValue("cameraPos", camera.getEye());

	// cast the light and use the shadow
	sun.cast("sun[0]", objectsShader);
	sunShadowMap.passUniforms(objectsShader, "shadowMap[0]", "lightSpaceMatrix[0]", sun.getViewMatrix());

	pointLight.cast("pointLights[0]", objectsShader);
	pointShadow.passUniforms(objectsShader, "cubeDepthMap[0]", "farPlane");


	// enable HDR framebuffer
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO));
	window.clearColorBufferBit(0.5f, 0.5f, 0.5f, 1.0f);
	// draw stuff
	player.model->draw(player.scale, player.position, player.rotation, objectsShader);
	// render bricks
	for (size_t i = 0; i < bricks.size(); i++)
	{
		GameComponent& brick = bricks.at(i);
		brick.model->draw(brick.scale, brick.position, brick.velocity, objectsShader);
	}
	// render background
	background.model->draw(background.scale, background.position, background.rotation, objectsShader);
	// disable HDR framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//std::cout << window.getCurrentTime() << ", " << window.getLastTime() << ", " << 1.0 / window.getLastFrameTime() << std::endl;

	// render on screen
	hdrShader.bind();
	hdrShader.setTexture(GL_TEXTURE_2D, "hdrBuffer", colorBufferTexture);
	hdrQuad.draw();
	hdrShader.unbind();


}

void GameLevel::update(Window& window)
{
	float dt = window.getLastFrameTime();

	processCommands(window);
}

void GameLevel::processCommands(Window& window)
{
	float t = window.getCurrentTime();
	float dt = window.getLastFrameTime();

	camera.processCommands(window);
	player.processCommands(window, dt, 5.0f);
	pointLight.eye = glm::vec3{5.0 + sin(1.0 * t), 1.5, 5.0 + cos(1.0 * t)};
}


bool GameLevel::isCompleted()
{
	if (player.position.x >= 10.0)
		return true;
	return false;
}


void renderQuadi() {
	unsigned int quadVAOi = 0;
	unsigned int quadVBOi;
	if (quadVAOi == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAOi);
		glGenBuffers(1, &quadVBOi);
		glBindVertexArray(quadVAOi);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBOi);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAOi);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}