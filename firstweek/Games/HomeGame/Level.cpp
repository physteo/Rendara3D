#include "Level.h"



Level::Level(Window& window, std::vector<Texture>* loadedTextures) :
	orangeBrick{ "./res/model/container/container.obj", glm::vec3{1.0f,0.0f,1.0f}, loadedTextures },
	blueBrick{ "./res/model/container/container_cardboard.obj", glm::vec3{0.2f,0.2f,8.0f}, loadedTextures },
	ironBrick{ "./res/model/container/container_hard.obj", glm::vec3{0.5f,0.5f,0.5f}, loadedTextures },
	parquet{ "./res/model/parquet/parquet.obj", loadedTextures },
	playerModel{ "./res/model/quad/quad_walking_man.obj", loadedTextures },
	ballModel{ "./res/model/sphere/sphere.obj", loadedTextures },
	outsideBackgroundModel{ "./res/model/quad/background_example.obj", loadedTextures },
	quad{ "./res/model/quad/quad.obj", loadedTextures },
	sun{ sunPosition, sunCenter, sunAmbient0, sunDiffuse0, sunSpecular },
	pointLight{ pointLightPosition, ambient, diffuse, specular, constant, linear, quadratic },
	// first create a "frustrum", it basically specifies a matrix to project shadows
	orthoFrustrum{ sun_nearPlane, sun_farPlane, sun_left, sun_right, sun_down, sun_up },
	// now create a shadowMap (it's basically a texture containing the info about what is lit what is not)
	sunShadowMap{ orthoFrustrum, 1024.0, 1024.0 },
	// cube shadow map
	pointShadow{ 1024, 1024 },
	hdrQuad{},
	particles{ 1000 },
	coloredQuads{ 1000 }
{

	levelState = LevelState::OUTSIDE;

	// generate textures for shadows
	sunShadowMap.generate();
	pointShadow.generate();

	// create shaders
	objectsShader.generate("./res/shaders/objects_wlights.shader");
	sunShadowShader.generate("./res/shaders/depth.shader");
	cubeDepthShader.generate("./res/shaders/cubeDepth.shader");
	instancesObjectsShader.generate("./res/shaders/instances_objects_wlights.shader");
	instancesSunShadowShader.generate("./res/shaders/instances_depth.shader");
	instancesCubeDepthShader.generate("./res/shaders/instances_cubeDepth.shader");
	instancesColoredQuadsShader.generate("./res/shaders/instances_colored_quads.shader");
	hdrShader.generate("./res/shaders/hdr.shader");

	playerShader.generate("./res/shaders/quads_with_alpha.shader");
	defaultQuadShader.generate("./res/shaders/quads_default_walpha.shader");

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
	hdrShader.setUniformValue("exposure", 0.5f);
	hdrShader.unbind();

	// construct the proper orthographic projection
	float delta_z = 5.0f;
	float delta_x = delta_z * window.getWidth() / window.getHeight();

	//projection = glm::ortho(-delta_x / 2.0f, +delta_x / 2.0f,
	//	-delta_z / 2.0f, +delta_z / 2.0f, 1.0f, 10.0f);
	//projection = glm::perspective( glm::radians(90.0f), (float)window.getWidth() / (float)window.getHeight(), 0.1f, 50.0f);

	projection = glm::perspective(glm::radians(90.0f), (float)window.getWidth() / (float)window.getHeight(), 0.1f, 50.0f);


	std::vector<Vertex> vertices;
	vertices.resize(4);
	vertices.at(0).position  = glm::vec3{-0.5f, 0.0f, 0.5};
	vertices.at(0).normal    = glm::vec3{0.0f, 1.0f, 0.0f};
	vertices.at(0).tangent   = glm::vec3{0.0f, 0.16f, 0.0f};

	vertices.at(1).position  = glm::vec3{0.5f, 0.0f, 0.5};
	vertices.at(1).normal    = glm::vec3{0.0f, 1.0f, 0.0f};
	vertices.at(1).tangent   = glm::vec3{0.0f, 0.16f, 0.0f};

	vertices.at(2).position  = glm::vec3{0.5f, 0.0f, -0.5};
	vertices.at(2).normal    = glm::vec3{0.0f, 1.0f, 0.0f};
	vertices.at(2).tangent   = glm::vec3{0.0f, 0.16f, 0.0f};

	vertices.at(3).position  = glm::vec3{-0.5f,0.0f,-0.5f};
	vertices.at(3).normal    = glm::vec3{0.0f,1.0f,0.0f};
	vertices.at(3).tangent   = glm::vec3{0.0f,0.0f,0.0f};

	vertices.at(0).texCoords = glm::vec3{0.0f, 1.0f, 0.0f};
	vertices.at(1).texCoords = glm::vec3{1.0f, 1.0f, 0.25f};
	vertices.at(2).texCoords = glm::vec3{1.0f, 0.0f, 0.25f};
	vertices.at(3).texCoords = glm::vec3{0.0f,0.0f,0.0f};

	std::vector<unsigned int> indices =	{0, 1, 2, 0, 2, 3};

	groundMesh.fill(vertices, indices, groundWithGrassMaterial);

}

void Level::load(const std::vector<std::vector<int> >& layout)
{

	// load the tiles (from file?)
	for (size_t i = 0; i < 10; i++)
	{
		Transform t;
		t.position = glm::vec3{ -5.0f + (float)i, -2.0f, 0.01f };
		t.rotation = glm::vec3{ 90.0f, 0.0f, 0.0f };
		t.scale = glm::vec3{ 1.0f };
		tiles.push_back(t);
	}

	particles.setModel(&ironBrick);
	coloredQuads.setModel(&quad);

	// position of the sun
	sun.eye = glm::vec3{ 0.0f, 0.0f, 5.0f };

	// background initialization
	background.transform.scale =    glm::vec3{ 1.0f };
	background.transform.position = 1.0f * glm::vec3{ 0.0f,-3.0f,-2.5f };
	background.transform.rotation = 1.0f * glm::vec3{ 0.0f,0.0f,0.0f };
	background.model = &parquet;


	// background initialization
	outsideBackground.transform.scale = glm::vec3{ 10.0f };
	outsideBackground.transform.position = 1.0f * glm::vec3{ 0.0f, 0.0f, 0.0f };
	outsideBackground.transform.rotation = 1.0f * glm::vec3{ 90.0f,0.0f,0.0f };
	outsideBackground.model = &this->outsideBackgroundModel;

	// player initialization
	player.transform.scale = glm::vec3{ 1.0f,1.0f,1.0f };
	player.transform.rotation = glm::vec3{ 90.0f, 0.0f,0.0f };
	player.fatness = HIGH;
	player.jumping = false;
	player.initialYcoord = -1.0f;
	player.transform.position = glm::vec3{ 0.0f,player.initialYcoord,0.01f };
	player.boundingWidth = 1.0f;
	player.boundingHeight = 1.0f;
	player.beingHit = false;
	player.model = &playerModel;

	// ball initialization
	ball.radius = 0.25f;
	ball.transform.scale = glm::vec3{ 1.0f };
	ball.transform.position = player.transform.position - 4.0f * glm::vec3{ 1.0f,1.0f, 0.0f };
	ball.transform.rotation = glm::vec3{ 0.0f, 0.0f, 0.0f };
	ball.velocity = 2.0f * glm::vec3{ -3.0f, 0.0f, -3.0f };
	ball.model = &ballModel;

	camera = Camera{ cameraEyeOutside, cameraCenterOutside, cameraUpOutside };


	defaultSpecularMap.generate("./res/textures/black.jpg", Format::RGBA);
	defaultNormalMap.generate("./res/textures/default_norm.png", Format::RGBA);
	
	playerTexture.generate("./res/textures/walking_man/Hat_man_sheet_fix.png", Format::sRGBA);
	playerMaterial.fill(playerTexture, defaultSpecularMap, defaultNormalMap, 256.0f);

	groundWithGrassTexture.generate("./res/textures/outside/ground_gras.png", Format::sRGBA);
	groundWithGrassMaterial.fill(groundWithGrassTexture, defaultSpecularMap, defaultNormalMap, 256.0f);

}

void Level::render(Window& window)
{
//	float dt = window.getLastFrameTime();
//	float t = window.getLastTime();
//	playerShader.bind();
//	playerShader.setUniformMatrix("projection", projection, false);
//	playerShader.setUniformMatrix("view", camera.getViewMatrix(), false);
//	playerShader.setUniformValue("brightness", 1.0f);
//
//	if (isKeyPressed(GLFW_KEY_LEFT, window.getGLFWwindow()))
//	{
//		glm::vec3 toAdd = -glm::vec3{ 2.0 * dt ,0.0f,0.0f };
//		player.transform.position += toAdd;
//		playerShader.setUniformValue("animation", 1);
//		playerShader.setUniformValue("t", (int)(t / 0.25f) % 4);
//	}
//	if (isKeyPressed(GLFW_KEY_RIGHT, window.getGLFWwindow()))
//	{
//		glm::vec3 toAdd = glm::vec3{ 2.0 * dt,0.0f,0.0f };
//		player.transform.position += toAdd;
//		playerShader.setUniformValue("animation", 0);
//		playerShader.setUniformValue("t", (int)(t / 0.25f) % 4);
//	}
//
//	playerModel.draw(1.0f, player.transform.position, glm::vec3{ 90.0f, 0.0f * window.getCurrentTime(), 0.0f }, playerShader);



	// calculate sunlight's shadows
	sunShadowMap.startShadows(window, sunShadowShader, &sun);
	ball.model->draw(ball.transform.scale, ball.transform.position, ball.transform.rotation, sunShadowShader);
	//// render bricks
	//for (size_t i = 0; i < bricks.size(); i++)
	//{
	//	GameObject& brick = bricks.at(i);
	//	brick.model->draw(brick.transform.scale, brick.transform.position, brick.transform.rotation, sunShadowShader);
	//}
	// render background
	background.model->draw(background.transform.scale, background.transform.position, background.transform.rotation, sunShadowShader);
	sunShadowMap.stopShadows(window, sunShadowShader);


	// calculate pointlight's shadows
	pointShadow.startShadows(window, cubeDepthShader, pointLight);
	ball.model->draw(ball.transform.scale, ball.transform.position, ball.transform.rotation, cubeDepthShader);
	//// render bricks
	//for (size_t i = 0; i < bricks.size(); i++)
	//{
	//	GameObject& brick = bricks.at(i);
	//	brick.model->draw(brick.transform.scale, brick.transform.position, brick.transform.rotation, cubeDepthShader);
	//}
	// render background
	background.model->draw(background.transform.scale, background.transform.position, background.transform.rotation, cubeDepthShader);
	pointShadow.stopShadows(window, cubeDepthShader);


	sunShadowMap.startShadows(window, instancesSunShadowShader, &sun);

	particles.drawInstances(instancesSunShadowShader);
	sunShadowMap.stopShadows(window, instancesSunShadowShader);


	pointShadow.startShadows(window, instancesCubeDepthShader, pointLight);

	particles.drawInstances(instancesCubeDepthShader);
	pointShadow.stopShadows(window, instancesCubeDepthShader);


	window.clearColorBufferBit(0.5f, 0.5f, 0.5f, 1.0f);


	// render the scene
	// enable HDR framebuffer
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO));
	window.clearColorBufferBit(0.5f, 0.5f, 0.5f, 1.0f);



	objectsShader.bind();
	objectsShader.setUniformMatrix("view", camera.getViewMatrix(), false);
	objectsShader.setUniformMatrix("projection", projection, false);
	objectsShader.setUniformValue("cameraPos", camera.getEye());

	// cast the light and use the shadow
	sun.cast("sun[0]", objectsShader);
	sunShadowMap.passUniforms(objectsShader, "shadowMap[0]", "lightSpaceMatrix[0]", sun.getViewMatrix());

	pointLight.cast("pointLights[0]", objectsShader);
	pointShadow.passUniforms(objectsShader, "cubeDepthMap[0]", "farPlane");

	// draw stuff
	ball.model->draw(ball.transform.scale, ball.transform.position, ball.transform.rotation, objectsShader);
	//// render bricks
	//for (size_t i = 0; i < bricks.size(); i++)
	//{
	//	GameObject& brick = bricks.at(i);
	//	brick.model->draw(brick.transform.scale, brick.transform.position, brick.transform.rotation, objectsShader);
	//}
	// render background
	background.model->draw(background.transform.scale, background.transform.position, background.transform.rotation, objectsShader);


	// now the instances
	instancesObjectsShader.bind();
	instancesObjectsShader.setUniformMatrix("view", camera.getViewMatrix(), false);
	instancesObjectsShader.setUniformMatrix("projection", projection, false);
	instancesObjectsShader.setUniformValue("cameraPos", camera.getEye());
	sun.cast("sun[0]", instancesObjectsShader);
	sunShadowMap.passUniforms(instancesObjectsShader, "shadowMap[0]", "lightSpaceMatrix[0]", sun.getViewMatrix());
	pointLight.cast("pointLights[0]", instancesObjectsShader);
	pointShadow.passUniforms(instancesObjectsShader, "cubeDepthMap[0]", "farPlane");


	particles.drawInstances(instancesObjectsShader);

	instancesColoredQuadsShader.bind();
	instancesColoredQuadsShader.setUniformMatrix("view", camera.getViewMatrix(), false);
	instancesColoredQuadsShader.setUniformMatrix("projection", projection, false);
	instancesColoredQuadsShader.setUniformValue("brightness", 1.0f);
	coloredQuads.drawInstances(instancesColoredQuadsShader);


//	// quads
	playerShader.bind();
	playerShader.setUniformMatrix("projection", projection, false);
	playerShader.setUniformMatrix("view", camera.getViewMatrix(), false);
	playerShader.setUniformValue("brightness", 1.0f);
	//playerShader.setUniformValue("animation", 0);
	//playerShader.setUniformValue("t", 0);
	if (player.beingHit)
	{
		float t = window.getCurrentTime();
		float dt = window.getLastFrameTime();

		if ( (int) (t/0.15f) % 2  )
			player.model->draw(player.transform.scale, player.transform.position, player.transform.rotation, playerShader, playerMaterial);
	}
	else {
		player.model->draw(player.transform.scale, player.transform.position, player.transform.rotation, playerShader, playerMaterial);
	}


	defaultQuadShader.bind();
	defaultQuadShader.setUniformMatrix("projection", projection, false);
	defaultQuadShader.setUniformMatrix("view", camera.getViewMatrix(), false);
	defaultQuadShader.setUniformValue("brightness", 1.0f);
	outsideBackground.model->draw(outsideBackground.transform.scale,
									outsideBackground.transform.position,
									outsideBackground.transform.rotation, defaultQuadShader);

	for(size_t i = 0; i < tiles.size(); i++)
		groundMesh.draw(1.0f, tiles.at(i).position, tiles.at(i).rotation, defaultQuadShader, groundWithGrassMaterial);

	for (size_t i = 0; i < enemies.size(); i++)
		groundMesh.draw(1.0f, enemies.at(i).transform.position, enemies.at(i).transform.rotation, defaultQuadShader, groundWithGrassMaterial);


	// disable HDR framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//std::cout << window.getCurrentTime() << ", " << window.getLastTime() << ", " << 1.0 / window.getLastFrameTime() << std::endl;



	// render on screen
	hdrShader.bind();
	hdrShader.setTexture(GL_TEXTURE_2D, "hdrBuffer", colorBufferTexture);
	hdrQuad.draw();
	hdrShader.unbind();
//


}

void Level::update(Window& window)
{
	float dt = window.getLastFrameTime();
	float t = window.getCurrentTime();

	if (levelState == LevelState::GOING_IN)
	{
		// animation of the level change
		//float new_x = camera.getEye().x;
		//float new_y = +5.0f * (4.0f / 3.1415f) * glm::tanh((t - lastChangingStateTime));
		//float new_z = -5.0f;
		//camera.setEye(glm::vec3{ new_x, new_y, new_z });


		camera.setEye(cameraEyeInside);
		camera.setCenter(cameraCenterInside);
		camera.setUp(cameraUpInside);
	}
	else if (levelState == LevelState::GOING_OUT)
	{


		camera.setEye(cameraEyeOutside);
		camera.setCenter(cameraCenterOutside);
		camera.setUp(cameraUpOutside);

	} else
	{

		// manage particles
		Particle newParticle;
		newParticle.transform.position = ball.transform.position + glm::vec3{ 0.2f * (rand() / (RAND_MAX + 1.0f)) };
		newParticle.transform.scale = glm::vec3{ 0.2f * (rand() / (RAND_MAX + 1.0f)) };
		newParticle.transform.rotation = glm::vec3{ 0.0f, 360.0f * rand() / (RAND_MAX + 1.0f), 0.0f };//glm::vec3{ 360.0f * rand() / (RAND_MAX + 1.0f) };
		float random_green = 0.5 * (rand()) / (RAND_MAX + 1.0);
		newParticle.color = glm::vec4{ 1.0f, random_green, random_green / 5.0f, 1.0f };
		newParticle.tau = 0.25f;
		coloredQuads.push_back(newParticle);
		// update time of all particles and kill old particles

		for (size_t i = 0; i < coloredQuads.size(); i++)
		{
			Particle particle = coloredQuads.getElement(i);
			particle.tau -= dt;
			if (particle.tau <= 0.0f)
				coloredQuads.deleteElement(i);
			else
				coloredQuads.setElement(i, particle);
		}



		// manage enemies
		float period = 2.0;
		float likely = 0.5;
		int int_t = (int)(t / period);
		int int_t_dt = (int)((t - dt) / period);
		if (int_t != int_t_dt)
		{
			if ((rand()) / (RAND_MAX + 1.0) > likely) {
				home::Enemy enemy;
				float rand_x = 1.0f + 1.0f * (rand()) / (RAND_MAX + 1.0);
				
				float min = -1.0f;
				float max = +0.0f;
				float rand_y = min + (rand() % static_cast<int>(max - min + 1));

				enemy.transform.position = glm::vec3{ rand_x, rand_y, 0.1f };
				enemy.transform.rotation = glm::vec3{ 90.0f,0.0f,0.0f };
				enemy.transform.scale = glm::vec3{ 1.0f };
				enemies.push_back(enemy);
			}
		}

		// advance enemies
		for (size_t i = 0; i < enemies.size(); i++)
		{
			enemies.at(i).advance(t, dt, 1.0f);
		}

		// damages on player
		if (!player.beingHit)
		{
			for (size_t i = 0; i < enemies.size(); i++)
			{
				if (player.AABBcollision(enemies.at(i).transform.position.x, enemies.at(i).transform.position.y, 1.0f, 1.0f))
				{
					std::cout << "collision" << std::endl;
					player.beingHit = true;
					player.hp--;
					player.lastHitTime = t;
				}
			}
		}
		
		player.updateHitStatus(t);

		// check for collisions
		if (ball.collisionWithRectangle(player.transform.position, player.transform.scale.x, player.transform.scale.y, player.transform.scale.z))
		{
		}


		if(player.jumping)
			player.advanceJump(t);

	}



	processCommands(window);
}

void Level::processCommands(Window& window)
{
	float t = window.getCurrentTime();
	float dt = window.getLastFrameTime();

	// level state logic
	if (isKeyPressed(GLFW_KEY_M, window.getGLFWwindow()))
	{
		// save the position of the player in the level

		// save the time, for animation of the camera
		lastChangingStateTime = t;

		if (this->levelState == LevelState::AT_HOME) {
			this->levelState = LevelState::GOING_OUT;
		}

		if (this->levelState == LevelState::OUTSIDE)
		{
			this->levelState = LevelState::GOING_IN;

		}
	}
	
	if (this->levelState == LevelState::GOING_IN || this->levelState == LevelState::GOING_OUT)
	{
		if (t - lastChangingStateTime >= 0.1)
		{
			if (this->levelState == LevelState::GOING_IN)
			{
				player.transform.rotation = glm::vec3{0.0f,0.0f,0.0f};
				this->levelState = LevelState::AT_HOME;
				player.transform.position.z = -2.0f;
				player.transform.position.y = 0.01f;
				player.transform.rotation.x = 0.0f;

			}
			else
			{
				player.transform.rotation = glm::vec3{ 90.0f,0.0f,0.0f };
				this->levelState = LevelState::OUTSIDE;
				player.transform.position.z = 0.01f;
				player.transform.rotation.x = 90.0f;
			}
		}
	}
	//if(levelState != LevelState::CHANGING)
	player.processCommands(window, dt, t, playerShader);


}


bool Level::isCompleted()
{
	if (ball.transform.position.x >= 12.0)
		return true;
	return false;
}


