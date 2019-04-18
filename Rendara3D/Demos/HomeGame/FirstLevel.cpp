#include "FirstLevel.h"

namespace homelife {


	FirstLevel::FirstLevel(Window& window, std::vector<Texture>* loadedTextures) :
		parquet{ "./res/model/house/inside.obj", loadedTextures },
		playerModel{ "./res/model/quad/quad_walking_man.obj", loadedTextures },
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
		coloredQuads{ 1000 },
		enemies{ 100 }
	{

		levelState = LevelState::OUTSIDE;

		tomatoOutside = false;
		doeOutside = false;

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
		defaultQuadShader4x8.generate("./res/shaders/quads_default_walpha_4x8.shader");

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
		vertices.at(0).position = glm::vec3{ -0.5f, 0.0f, 0.5 };
		vertices.at(0).normal = glm::vec3{ 0.0f, 1.0f, 0.0f };
		vertices.at(0).tangent = glm::vec3{ 0.0f, 0.16f, 0.0f };

		vertices.at(1).position = glm::vec3{ 0.5f, 0.0f, 0.5 };
		vertices.at(1).normal = glm::vec3{ 0.0f, 1.0f, 0.0f };
		vertices.at(1).tangent = glm::vec3{ 0.0f, 0.16f, 0.0f };

		vertices.at(2).position = glm::vec3{ 0.5f, 0.0f, -0.5 };
		vertices.at(2).normal = glm::vec3{ 0.0f, 1.0f, 0.0f };
		vertices.at(2).tangent = glm::vec3{ 0.0f, 0.16f, 0.0f };

		vertices.at(3).position = glm::vec3{ -0.5f,0.0f,-0.5f };
		vertices.at(3).normal = glm::vec3{ 0.0f,1.0f,0.0f };
		vertices.at(3).tangent = glm::vec3{ 0.0f,0.0f,0.0f };
		std::vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };

		vertices.at(0).texCoords = glm::vec3{ 0.0f, 1.0f, 0.0f };
		vertices.at(1).texCoords = glm::vec3{ 1.0f, 1.0f, 0.0f };
		vertices.at(2).texCoords = glm::vec3{ 1.0f, 0.0f, 0.0f };
		vertices.at(3).texCoords = glm::vec3{ 0.0f,0.0f,0.0f };
		groundMesh.fill(vertices, indices, groundWithGrassMaterial); // not needed: groundWithGrassMaterial

		vertices.at(0).texCoords = glm::vec3{ 0.0f,     1.0 / 8.0f, 0.0f };
		vertices.at(1).texCoords = glm::vec3{ 1.0f / 4.0, 1.0 / 8.0f, 0.0f };
		vertices.at(2).texCoords = glm::vec3{ 1.0f / 4.0, 0.0f,     0.0f };
		vertices.at(3).texCoords = glm::vec3{ 0.0f,     0.0f,     0.0f };
		quadSheet4x8Mesh.fill(vertices, indices, groundWithGrassMaterial); // not needed: groundWithGrassMaterial

	}

	void FirstLevel::load()
	{
		SoundEngine = irrklang::createIrrKlangDevice();
		//SoundEngine->play2D("./res/sounds/bleep.mp3", GL_FALSE);

		completedTime = 0.0f;
		completionTime = 180.0f;
		gameOverCalled = false;
		gameCompletedCalled = false;
		// load the tiles (from file?)
		for (size_t i = 0; i < 10; i++)
		{
			Transform t;
			t.position = glm::vec3{ -5.0f + (float)i, -2.0f, 0.05f };
			t.rotation = glm::vec3{ 90.0f, 0.0f, 0.0f };
			t.scale = glm::vec3{ 1.0f };
			tiles.push_back(t);
		}
		for (size_t i = 0; i < 10; i++)
		{
			Grass t;
			t.position = glm::vec3{ -5.0f + (float)i, -1.01f, 0.05f };
			t.rotation = glm::vec3{ 90.0f, 0.0f, 0.0f };
			t.scale = glm::vec3{ 1.0f };
			t.moving = false;
			t.boundingWidth = t.scale.x;
			t.boundingHeight = 0.25f;
			grassTiles.push_back(t);
		}

		coloredQuads.setModel(&quad);

		// position of the sun
		sun.eye = glm::vec3{ 0.0f, 0.0f, 5.0f };

		// background initialization
		background.transform.scale = glm::vec3{ 1.0f };
		background.transform.position = 1.0f * glm::vec3{ 0.0f,-3.0f,-2.5f };
		background.transform.rotation = 1.0f * glm::vec3{ 0.0f,0.0f,0.0f };
		background.model = &parquet;


		// background initialization
		outsideBackground.transform.scale = glm::vec3{ 10.0f, 8.0f, 8.0f };
		outsideBackground.transform.position = 1.0f * glm::vec3{ 0.0f, 0.7f, 0.0f };
		outsideBackground.transform.rotation = 1.0f * glm::vec3{ 90.0f,0.0f,0.0f };
		outsideBackground.model = &this->outsideBackgroundModel;

		insideBackground.transform.scale = glm::vec3{ 12.0f, 9.0f, 9.0f };
		insideBackground.transform.position = 1.0f * glm::vec3{ 0.5f, 0.0f, -2.5f };
		insideBackground.transform.rotation = 1.0f * glm::vec3{ 0.0f,0.0f,0.0f };
		insideBackground.model = &this->outsideBackgroundModel;


		// player initialization
		player.transform.scale = glm::vec3{ 1.0f,1.0f,1.0f };
		player.transform.rotation = glm::vec3{ 90.0f, 0.0f,0.0f };
		player.fatness = LOW;
		player.jumping = false;
		player.initialYcoord = -1.05f;
		player.initialZcoord = 0.01f;
		player.transform.position = glm::vec3{ 0.0f,player.initialYcoord,player.initialZcoord };
		player.boundingWidth = 0.5f;
		player.boundingHeight = 0.5f;
		player.beingHit = false;
		player.model = &playerModel;

		mum.position = glm::vec3{ -1.0f, 0.0f, -3.5f };
		mum.rotation = glm::vec3{ 0.0f };
		mum.scale = glm::vec3{ 1.0f };
		mumMakesPizza = false;

		oven.position = mum.position - glm::vec3{ 0.75f,0.0f,0.0f };
		oven.rotation = glm::vec3{ 0.0f };
		oven.scale = glm::vec3{ 1.0f };



		camera = Camera{ cameraEyeOutside, cameraCenterOutside, cameraUpOutside };


		defaultSpecularMap.generate("./res/textures/black.jpg", Format::RGBA);
		defaultNormalMap.generate("./res/textures/default_norm.png", Format::RGBA);

		playerTexture.generate("./res/textures/walking_man/Hat_man_sheet_fix.png", Format::sRGBA);
		playerMaterial.fill(playerTexture, defaultSpecularMap, defaultNormalMap, 256.0f);

		groundWithGrassTexture.generate("./res/textures/outside/dirt_gras.png", Format::sRGBA);
		groundWithGrassMaterial.fill(groundWithGrassTexture, defaultSpecularMap, defaultNormalMap, 256.0f);

		grassTexture.generate("./res/textures/outside/gras.png", Format::sRGBA);
		grassMaterial.fill(grassTexture, defaultSpecularMap, defaultNormalMap, 256.0f);

		backgroundOutsideTexture.generate("./res/textures/outside/background.png", Format::sRGBA);
		backgroundOutsideMaterial.fill(backgroundOutsideTexture, defaultSpecularMap, defaultNormalMap, 256.0f);

		backgroundInsideTexture.generate("./res/textures/inside/dream_clouds.png", Format::sRGBA);
		backgroundInsideMaterial.fill(backgroundInsideTexture, defaultSpecularMap, defaultNormalMap, 256.0f);

		gameOverTexture.generate("./res/textures/screens/game_over.png", Format::sRGBA);
		gameOverTextureMaterial.fill(gameOverTexture, defaultSpecularMap, defaultNormalMap, 256.0f);

		pressBTexture.generate("./res/textures/screens/controls.png", Format::sRGBA);
		pressBMaterial.fill(pressBTexture, defaultSpecularMap, defaultNormalMap, 256.0f);

		emptyBarTexture.generate("./res/textures/screens/empty_completion_bar.png", Format::sRGBA);
		emptyBarMaterial.fill(emptyBarTexture, defaultSpecularMap, defaultNormalMap, 256.0f);

		completionBarTexture.generate("./res/textures/screens/full_completion_bar.png", Format::sRGBA);
		completionBarMaterial.fill(completionBarTexture, defaultSpecularMap, defaultNormalMap, 256.0f);

		gameCompletedTexture.generate("./res/textures/screens/game_completed.png", Format::sRGBA);
		gameCompletedTextureMaterial.fill(gameCompletedTexture, defaultSpecularMap, defaultNormalMap, 256.0f);

		itemsTexture.generate("./res/textures/outside/sprite_sheet2.png", Format::sRGBA);
		itemsMaterial.fill(itemsTexture, defaultSpecularMap, defaultNormalMap, 256.0f);

		enemiesTexture.generate("./res/textures/outside/sprite_sheet1.png", Format::sRGBA);
		enemiesMaterial.fill(enemiesTexture, defaultSpecularMap, defaultNormalMap, 256.0f);

		insideTexture.generate("./res/textures/inside/sprit_sheet_inside.png", Format::sRGBA);
		insideMaterial.fill(insideTexture, defaultSpecularMap, defaultNormalMap, 256.0f);

		whiteTexture.generate("./res/textures/inside/white.png", Format::sRGBA);
		whiteMaterial.fill(whiteTexture, defaultSpecularMap, defaultNormalMap, 256.0f);

		pizzaTexture.generate("./res/textures/inside/pizza.png", Format::sRGBA);
		pizzaMaterial.fill(pizzaTexture, defaultSpecularMap, defaultNormalMap, 256.0f);

		generalQuad.fillQuad(4, 8);

	}

	void FirstLevel::render(Window& window)
	{

		float t = window.getCurrentTime();
		float dt = window.getLastFrameTime();


		if (levelState == LevelState::OUTSIDE) {

			window.clearColorBufferBit(0.0f, 0.0f, 0.0f, 1.0f);
			// render the scene
			// enable HDR framebuffer
			GLCall(glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO));
			window.clearColorBufferBit(0.0f, 0.0f, 0.0f, 1.0f);

			// quad
			defaultQuadShader.bind();
			defaultQuadShader.setUniformMatrix("projection", projection, false);
			defaultQuadShader.setUniformMatrix("view", camera.getViewMatrix(), false);
			defaultQuadShader.setUniformValue("brightness", 1.0f);




			outsideBackground.model->draw(outsideBackground.transform.scale,
				outsideBackground.transform.position,
				outsideBackground.transform.rotation, defaultQuadShader, backgroundOutsideMaterial);

			//if (pizza.position.x > 90.0f) {
			outsideBackground.model->draw(0.6f * glm::vec3{ 3.0f,1.0f,1.0f },
				outsideBackground.transform.position + glm::vec3{ -3.0f,-2.6f,0.2f },
				outsideBackground.transform.rotation, defaultQuadShader, pressBMaterial);
			//}

			outsideBackground.model->draw(0.6f * glm::vec3{ 3.0f,1.0f,1.0f },
				outsideBackground.transform.position + glm::vec3{ +3.0f,-2.6f,0.2f },
				outsideBackground.transform.rotation, defaultQuadShader, emptyBarMaterial);

			float fractionCompleted = completedTime / completionTime;
			outsideBackground.model->draw(0.6f * glm::vec3{ fractionCompleted * 3.0f,1.0f,1.0f },
				outsideBackground.transform.position + glm::vec3{ +3.0f,-2.6f,0.2001f } -glm::vec3{ 0.6f*3.0f*0.5f*(1.0f - fractionCompleted),0.0f,0.0f },
				outsideBackground.transform.rotation, defaultQuadShader, completionBarMaterial);


			for (size_t i = 0; i < tiles.size(); i++)
				groundMesh.draw(1.0f, tiles.at(i).position, tiles.at(i).rotation, defaultQuadShader, groundWithGrassMaterial);

			for (size_t i = 0; i < grassTiles.size(); i++)
			{
				if (grassTiles.at(i).moving)
				{
					if ((int)(t / 0.5f) % 2)
					{
						groundMesh.draw(1.0f, grassTiles.at(i).position + glm::vec3{ 0.1f,0.0f,0.0f }, grassTiles.at(i).rotation, defaultQuadShader, grassMaterial);
					}
					else
					{
						groundMesh.draw(1.0f, grassTiles.at(i).position - glm::vec3{ 0.1f,0.0f,0.0f }, grassTiles.at(i).rotation, defaultQuadShader, grassMaterial);
					}
				}
				else
				{
					groundMesh.draw(1.0f, grassTiles.at(i).position, grassTiles.at(i).rotation, defaultQuadShader, grassMaterial);
				}
			}


			defaultQuadShader4x8.bind();
			defaultQuadShader4x8.setUniformMatrix("projection", projection, false);
			defaultQuadShader4x8.setUniformMatrix("view", camera.getViewMatrix(), false);
			defaultQuadShader4x8.setUniformValue("brightness", 1.0f);
			for (size_t i = 0; i < enemies.size(); i++)
			{
				defaultQuadShader4x8.bind();
				if (enemies.at(i).enemyID == 0) { defaultQuadShader4x8.setUniformValue("x", 0); defaultQuadShader4x8.setUniformValue("y", 6); }
				else if (enemies.at(i).enemyID == 1) { defaultQuadShader4x8.setUniformValue("x", 1); defaultQuadShader4x8.setUniformValue("y", 6); }
				else if (enemies.at(i).enemyID == 2) { defaultQuadShader4x8.setUniformValue("x", 3); defaultQuadShader4x8.setUniformValue("y", 7); }
				quadSheet4x8Mesh.draw(enemies.at(i).transform.scale, enemies.at(i).transform.position, enemies.at(i).transform.rotation, defaultQuadShader4x8, enemiesMaterial);
			}

			// render ingredients, lives, inventory
			defaultQuadShader4x8.bind();
			defaultQuadShader4x8.setUniformValue("x", 2);
			defaultQuadShader4x8.setUniformValue("y", 7);
			quadSheet4x8Mesh.draw(tomato.transform.scale, tomato.transform.position, tomato.transform.rotation, defaultQuadShader4x8, itemsMaterial);
			defaultQuadShader4x8.bind();
			defaultQuadShader4x8.setUniformValue("x", 0);
			defaultQuadShader4x8.setUniformValue("y", 7);
			quadSheet4x8Mesh.draw(doe.transform.scale, doe.transform.position, doe.transform.rotation, defaultQuadShader4x8, itemsMaterial);


			defaultQuadShader4x8.bind();
			defaultQuadShader4x8.setUniformValue("x", 0);
			defaultQuadShader4x8.setUniformValue("y", 6);
			quadSheet4x8Mesh.draw(oven.scale, oven.position, oven.rotation, defaultQuadShader4x8, insideMaterial);

			defaultQuadShader4x8.bind();
			defaultQuadShader4x8.setUniformValue("x", 0);
			defaultQuadShader4x8.setUniformValue("y", 7);
			quadSheet4x8Mesh.draw(mum.scale, mum.position, mum.rotation, defaultQuadShader4x8, insideMaterial);

			//generalQuad.drawQuad(glm::vec3{ 1.0f }, glm::vec3{ 0.0f,0.0f,0.1f }, glm::vec3{ 90.0f, 0.0f,0.0f }, 0, 7, defaultQuadShader4x8, insideTexture);

		}
		else if (this->levelState == LevelState::AT_HOME)
		{
			// calculate sunlight's shadows
			sunShadowMap.startShadows(window, sunShadowShader, &sun);
			background.model->draw(background.transform.scale, background.transform.position, background.transform.rotation, sunShadowShader);
			sunShadowMap.stopShadows(window, sunShadowShader);

			// calculate pointlight's shadows
			pointShadow.startShadows(window, cubeDepthShader, pointLight);
			background.model->draw(background.transform.scale, background.transform.position, background.transform.rotation, cubeDepthShader);
			pointShadow.stopShadows(window, cubeDepthShader);

			//sunShadowMap.startShadows(window, instancesSunShadowShader, &sun);
			//particles.drawInstances(instancesSunShadowShader);
			//sunShadowMap.stopShadows(window, instancesSunShadowShader);
			//
			//pointShadow.startShadows(window, instancesCubeDepthShader, pointLight);
			//particles.drawInstances(instancesCubeDepthShader);
			//pointShadow.stopShadows(window, instancesCubeDepthShader);


			window.clearColorBufferBit(0.0f, 0.0f, 0.0f, 1.0f);


			// render the scene
			// enable HDR framebuffer
			GLCall(glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO));
			window.clearColorBufferBit(0.0f, 0.0f, 0.0f, 1.0f);

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
			background.model->draw(background.transform.scale, background.transform.position, background.transform.rotation, objectsShader);

			//// now the instances
			//instancesObjectsShader.bind();
			//instancesObjectsShader.setUniformMatrix("view", camera.getViewMatrix(), false);
			//instancesObjectsShader.setUniformMatrix("projection", projection, false);
			//instancesObjectsShader.setUniformValue("cameraPos", camera.getEye());
			//sun.cast("sun[0]", instancesObjectsShader);
			//sunShadowMap.passUniforms(instancesObjectsShader, "shadowMap[0]", "lightSpaceMatrix[0]", sun.getViewMatrix());
			//pointLight.cast("pointLights[0]", instancesObjectsShader);
			//pointShadow.passUniforms(instancesObjectsShader, "cubeDepthMap[0]", "farPlane");
			//
			//
			//
			//particles.drawInstances(instancesObjectsShader);
			//
			instancesColoredQuadsShader.bind();
			instancesColoredQuadsShader.setUniformMatrix("view", camera.getViewMatrix(), false);
			instancesColoredQuadsShader.setUniformMatrix("projection", projection, false);
			instancesColoredQuadsShader.setUniformValue("brightness", 1.0f);
			coloredQuads.drawInstances(instancesColoredQuadsShader);

			defaultQuadShader.bind();
			defaultQuadShader.setUniformMatrix("projection", projection, false);
			defaultQuadShader.setUniformMatrix("view", camera.getViewMatrix(), false);
			defaultQuadShader.setUniformValue("brightness", 1.0f);
			// draw the clouds
			insideBackground.model->draw(insideBackground.transform.scale,
				insideBackground.transform.position,
				insideBackground.transform.rotation, defaultQuadShader, backgroundInsideMaterial);

			insideBackground.model->draw(insideBackground.transform.scale * 2.0f,
				glm::vec3{ 5.0f, -5.0f, -2.0f },
				insideBackground.transform.rotation, defaultQuadShader, whiteMaterial);

			groundMesh.draw(pizza.scale, pizza.position, pizza.rotation, defaultQuadShader, pizzaMaterial);




			defaultQuadShader4x8.bind();
			defaultQuadShader4x8.setUniformMatrix("projection", projection, false);
			defaultQuadShader4x8.setUniformMatrix("view", camera.getViewMatrix(), false);
			defaultQuadShader4x8.setUniformValue("brightness", 1.0f);
			defaultQuadShader4x8.setUniformValue("x", (int)(t / 0.5f) % 4);
			defaultQuadShader4x8.setUniformValue("y", 6);
			quadSheet4x8Mesh.draw(oven.scale, oven.position, oven.rotation, defaultQuadShader4x8, insideMaterial);

			defaultQuadShader4x8.bind();
			defaultQuadShader4x8.setUniformValue("x", (int)(t / 0.5f) % 4);
			defaultQuadShader4x8.setUniformValue("y", 7);
			quadSheet4x8Mesh.draw(mum.scale, mum.position, mum.rotation, defaultQuadShader4x8, insideMaterial);




		}

		//	// quads
		playerShader.bind();
		playerShader.setUniformMatrix("projection", projection, false);
		playerShader.setUniformMatrix("view", camera.getViewMatrix(), false);
		playerShader.setUniformValue("brightness", 1.0f);
		if (player.beingHit)
		{
			if ((int)(t / 0.15f) % 2)
				player.model->draw(player.transform.scale, player.transform.position, player.transform.rotation, playerShader, playerMaterial);
		}
		else {
			player.model->draw(player.transform.scale, player.transform.position, player.transform.rotation, playerShader, playerMaterial);
		}

		defaultQuadShader4x8.bind();
		playerShader.setUniformMatrix("projection", projection, false);
		playerShader.setUniformMatrix("view", camera.getViewMatrix(), false);
		playerShader.setUniformValue("brightness", 1.0f);
		player.inventory.draw(defaultQuadShader4x8, quadSheet4x8Mesh, itemsMaterial);



		// disable HDR framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// render on screen
		hdrShader.bind();
		hdrShader.setTexture(GL_TEXTURE_2D, "hdrBuffer", colorBufferTexture);
		hdrQuad.draw();
		hdrShader.unbind();

	}

	void FirstLevel::update(Window& window)
	{
		float dt = window.getLastFrameTime();
		float t = window.getCurrentTime();

		if (!gameOverCalled)
		{
			if (t > completionTime)
			{
				gameCompletedCalled = true;
				gameCompleted(window);
			}
		}

		if (player.inventory.hp <= 0)
		{
			if (!gameOverCalled)
				gameOverTime = t;

			gameOverCalled = true;

			gameOver(window);
			if (t - gameOverTime > gameOverDuration)
			{
				window.close();
			}
		}

		if (levelState == LevelState::GOING_IN)
		{
			camera.setEye(cameraEyeInside);
			camera.setCenter(cameraCenterInside);
			camera.setUp(cameraUpInside);
		}
		else if (levelState == LevelState::GOING_OUT)
		{

			camera.setEye(cameraEyeOutside);
			camera.setCenter(cameraCenterOutside);
			camera.setUp(cameraUpOutside);

		}
		else if (levelState == LevelState::OUTSIDE)
		{
			completedTime += dt;
			advanceParticles(t, dt);
			// manage particles
			//Particle newParticle;
			//newParticle.transform.position = ball.transform.position + glm::vec3{ 0.2f * (rand() / (RAND_MAX + 1.0f)) };
			//newParticle.transform.scale = glm::vec3{ 0.2f * (rand() / (RAND_MAX + 1.0f)) };
			//newParticle.transform.rotation = glm::vec3{ 0.0f, 360.0f * rand() / (RAND_MAX + 1.0f), 0.0f };//glm::vec3{ 360.0f * rand() / (RAND_MAX + 1.0f) };
			//float random_green = 0.5 * (rand()) / (RAND_MAX + 1.0);
			//newParticle.color = glm::vec4{ 1.0f, random_green, random_green / 5.0f, 1.0f };
			//newParticle.tau = 0.25f;
			//coloredQuads.push_back(newParticle);
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

			// spawn ingredients
			{
				float period = 3.0; float likely = 0.5;
				int int_t = (int)(t / period); int int_t_dt = (int)((t - dt) / period);
				if (int_t != int_t_dt)
				{
					if ((rand()) / (RAND_MAX + 1.0) > likely) {

						float rand_x = 2.0f + 1.0f * (rand()) / (RAND_MAX + 1.0);
						Transform transform;
						transform.position = glm::vec3{ rand_x, -1.0f, 0.1f };
						transform.rotation = glm::vec3{ 90.0f,0.0f,0.0f };
						transform.scale = glm::vec3{ 0.5f };
						float boundingHeight = transform.scale.y;
						float boundingWidth = transform.scale.x;
						if ((rand()) / (RAND_MAX + 1.0) > 0.5)
						{
							// it's a tomato
							if (!tomatoOutside)
							{
								tomato.transform = transform;
								tomatoOutside = true;
							}
						}
						else
						{
							// it's doe!
							if (!doeOutside)
							{
								doe.transform = transform;
								doeOutside = true;
							}
						}
					}
				}
			}
			// spawn enemies

			// advance enemies
			float enemySpeed = 3.0;
			for (size_t i = 0; i < enemies.size(); i++)
			{
				enemies.at(i).advance(t, dt, enemySpeed);
			}
			spawnEnemies(t, dt, enemySpeed);


			// delete enemies
			for (size_t i = 0; i < enemies.size(); i++)
			{
				if ((t - enemies.at(i).spawnedTime) > enemies.at(i).tau)
					enemies.deleteElement(i);
			}

			// damages on player
			if (!player.beingHit)
			{
				for (size_t i = 0; i < enemies.size(); i++)
				{
					if (player.AABBcollision(enemies.at(i).transform.position.x, enemies.at(i).transform.position.y,
						enemies.at(i).boundingWidth, enemies.at(i).boundingHeight))
					{
						//std::cout << "collision" << std::endl;
						SoundEngine->play2D("./res/sounds/got_hit.wav", GL_FALSE);
						player.beingHit = true;
						player.inventory.hp--;
						player.lastHitTime = t;
						player.inventory.doeCounter = 0;
						player.inventory.tomatoCounter = 0;
					}
				}
			}

			if (player.moving)
			{
				for (size_t i = 0; i < grassTiles.size(); i++)
				{
					if (player.AABBcollision(grassTiles.at(i).position.x, grassTiles.at(i).position.y - 0.25f,
						grassTiles.at(i).boundingWidth, grassTiles.at(i).boundingHeight))
					{
						grassTiles.at(i).moving = true;
						grassTiles.at(i).lastTimeMoving = t;
					}
				}
			}

			// update status of player
			player.updateHitStatus(t);
			if (player.jumping)
			{
				bool stillJumping = player.advanceJump(t);
				if (!stillJumping)
				{
					for (size_t a = 0; a < 10; a++) {
						// spawn particles
						Particle newParticle;
						float rand_x = 0.2f * ((rand() / (RAND_MAX + 1.0f)) - 0.5f);
						newParticle.transform.position = player.transform.position + glm::vec3{ rand_x, 0.0f, 0.0f };
						newParticle.transform.scale = glm::vec3{ 0.2f * (rand() / (RAND_MAX + 1.0f)) };
						newParticle.transform.rotation = glm::vec3{ 90.0f, 0.0f, 0.0f };//glm::vec3{ 360.0f * rand() / (RAND_MAX + 1.0f) };
						float random_green = 0.5 * (rand()) / (RAND_MAX + 1.0);
						newParticle.color = glm::vec4{ 1.0f, random_green, random_green / 5.0f, 1.0f };
						newParticle.direction = glm::vec4{ 1.0f, 1.0f, 0.0f, 1.0f };
						newParticle.tau = 0.25f;
						coloredQuads.push_back(newParticle);
					}
				}
			}
			// update fatness
			if (t - player.lastChangeFat > player.fatDuration)
			{
				if (player.fatness == Fatness::LOW)
				{

				}
				else if (player.fatness == Fatness::MEDIUM)
				{
					player.fatness = Fatness::LOW;
				}
				else if (player.fatness == Fatness::HIGH)
				{
					player.fatness = Fatness::MEDIUM;
				}

				player.lastChangeFat = t;
			}




			for (size_t i = 0; i < grassTiles.size(); i++)
			{
				grassTiles.at(i).updateStates(t);
			}

			// check for collisions


			// collision and collection with tomato and doe
			if (player.AABBcollision(tomato.transform.position.x, tomato.transform.position.y, tomato.boundingWidth, tomato.boundingHeight))
			{
				// play sound collection tomato
				SoundEngine->play2D("./res/sounds/collected_item.mp3", GL_FALSE);

				player.inventory.tomatoCounter = 1;
				// tomato disappear
				tomato.transform.position.x = 100.0;
				tomatoOutside = false;
			}
			if (player.AABBcollision(doe.transform.position.x, doe.transform.position.y, doe.boundingWidth, doe.boundingHeight))
			{
				// play sound collection doe
				SoundEngine->play2D("./res/sounds/collected_item.mp3", GL_FALSE);
				player.inventory.doeCounter = 1;
				doe.transform.position.x = 100.0;
				doeOutside = false;
			}




		}
		else if (levelState == LevelState::AT_HOME)
		{
			if (mumMakesPizza)
			{
				// spawn pizza
				pizza.position = glm::vec3{ -0.5f,1.0f,-2.0f };
				pizza.rotation = glm::vec3{ 0.0f };
				pizza.scale = glm::vec3{ 0.5f };
			}

			// collision and collection with tomato and doe
			if (player.AABBcollisionXZ(pizza.position.x, pizza.position.z, 0.5, 0.5))
			{
				std::cout << "" << std::endl;
				// play sound collection pizza
				SoundEngine->play2D("./res/sounds/ate_pizza.wav", GL_FALSE);

				if (player.inventory.hp < 5)
					player.inventory.hp++;

				// tomato disappear
				pizza.position.x = 100.0;
				mumMakesPizza = false;

				if (player.fatness == Fatness::LOW)
				{
					player.fatness = Fatness::MEDIUM;
					player.lastChangeFat = t;
				}
				else if (player.fatness == Fatness::MEDIUM)
				{
					player.fatness = Fatness::HIGH;
					player.lastChangeFat = t;
				}
				else if (player.fatness == Fatness::HIGH)
				{
					player.fatness = Fatness::HIGH;
					player.lastChangeFat = t;
				}
			}
		}

		processCommands(window);
	}

	void FirstLevel::processCommands(Window& window)
	{
		float t = window.getCurrentTime();
		float dt = window.getLastFrameTime();



		// level state logic
		if (isKeyPressed(GLFW_KEY_B, window.getGLFWwindow()))
		{
			// save the position of the player in the level

			// save the time, for animation of the camera
			lastChangingStateTime = t;

			if (this->levelState == LevelState::AT_HOME) {
				SoundEngine->play2D("./res/sounds/going_in.wav", GL_FALSE);
				this->levelState = LevelState::GOING_OUT;
			}

			if (this->levelState == LevelState::OUTSIDE)
			{
				SoundEngine->play2D("./res/sounds/going_in.wav", GL_FALSE);
				this->levelState = LevelState::GOING_IN;

			}
		}

		if (this->levelState == LevelState::GOING_IN || this->levelState == LevelState::GOING_OUT)
		{
			if (t - lastChangingStateTime >= 0.1)
			{
				if (this->levelState == LevelState::GOING_IN)
				{
					player.transform.rotation = glm::vec3{ 0.0f,0.0f,0.0f };
					this->levelState = LevelState::AT_HOME;
					player.transform.position.z = -2.0f;
					player.transform.position.y = 0.01f;
					player.transform.rotation.x = 0.0f;

					// if inventory is full, mum makes pizza
					if (player.inventory.tomatoCounter > 0 && player.inventory.doeCounter > 0)
					{
						mumMakesPizza = true;
					}

					// inventory is emptied
					player.inventory.tomatoCounter = 0;
					player.inventory.doeCounter = 0;

				}
				else
				{
					player.transform.rotation = glm::vec3{ 90.0f,0.0f,0.0f };
					this->levelState = LevelState::OUTSIDE;
					player.transform.position.z = player.initialZcoord;
					player.transform.position.y = player.initialYcoord;
					player.transform.rotation.x = 90.0f;
				}
			}
		}
		//if(levelState != LevelState::CHANGING)
		player.processCommands(window, dt, t, playerShader, levelState, SoundEngine);


	}





	void FirstLevel::gameOver(Window& window)
	{

		window.clearColorBufferBit(0.0f, 0.0f, 0.0f, 1.0f);


		// render the scene
		// enable HDR framebuffer
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO));
		window.clearColorBufferBit(0.0f, 0.0f, 0.0f, 1.0f);



		defaultQuadShader.bind();
		defaultQuadShader.setUniformMatrix("projection", projection, false);
		defaultQuadShader.setUniformMatrix("view", camera.getViewMatrix(), false);
		defaultQuadShader.setUniformValue("brightness", 1.0f);
		outsideBackground.model->draw(outsideBackground.transform.scale,
			outsideBackground.transform.position,
			outsideBackground.transform.rotation, defaultQuadShader, gameOverTextureMaterial);


		// disable HDR framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// render on screen
		hdrShader.bind();
		hdrShader.setTexture(GL_TEXTURE_2D, "hdrBuffer", colorBufferTexture);
		hdrQuad.draw();
		hdrShader.unbind();

	}


	void FirstLevel::gameCompleted(Window& window)
	{

		window.clearColorBufferBit(0.0f, 0.0f, 0.0f, 1.0f);


		// render the scene
		// enable HDR framebuffer
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO));
		window.clearColorBufferBit(0.0f, 0.0f, 0.0f, 1.0f);


		defaultQuadShader.bind();
		defaultQuadShader.setUniformMatrix("projection", projection, false);
		defaultQuadShader.setUniformMatrix("view", camera.getViewMatrix(), false);
		defaultQuadShader.setUniformValue("brightness", 1.0f);
		outsideBackground.model->draw(outsideBackground.transform.scale,
			outsideBackground.transform.position,
			outsideBackground.transform.rotation, defaultQuadShader, gameCompletedTextureMaterial);


		// disable HDR framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// render on screen
		hdrShader.bind();
		hdrShader.setTexture(GL_TEXTURE_2D, "hdrBuffer", colorBufferTexture);
		hdrQuad.draw();
		hdrShader.unbind();

	}



	void FirstLevel::advanceParticles(float t, float dt)
	{
		const float g = 0.2;
		for (size_t i = 0; i < coloredQuads.size(); i++)
		{
			// parabola in the direction of "rotation"
			Particle particle = coloredQuads.getElement(i);
			particle.transform.position += particle.direction * dt - 0.5f * g * dt * dt;
		}
	}


	void FirstLevel::spawnEnemies(float t, float dt, float enemySpeed)
	{
		float period = 2.0 / enemySpeed;
		float likely = 1.0;
		int int_t = (int)(t / period);
		int int_t_dt = (int)((t - dt) / period);
		if (int_t != int_t_dt)
		{
			if ((rand()) / (RAND_MAX + 1.0) < likely) {
				Enemy enemy;
				float rand_x = 6.0f;//+ 1.0f * (rand()) / (RAND_MAX + 1.0);
				float min = -1.0f;
				float max = +1.0f;
				float rand_y = min + (rand() % static_cast<int>(max - min + 1));

				enemy.transform.position = glm::vec3{ rand_x, 0.6 * rand_y - 0.6f, 0.1f };
				enemy.transform.rotation = glm::vec3{ 90.0f,0.0f,0.0f };
				enemy.transform.scale = glm::vec3{ 0.5f };
				enemy.boundingHeight = enemy.transform.scale.y;
				enemy.boundingWidth = enemy.transform.scale.x;
				enemy.enemyID = rand_y + 1;
				enemy.spawnedTime = t;
				enemies.push_back(enemy);
			}
		}
	}
	// old spawn
#if 0 
	void FirstLevel::spawnEnemies(float t, float dt)
	{
		float period = 1.0;
		float likely = 0.6;
		int int_t = (int)(t / period);
		int int_t_dt = (int)((t - dt) / period);
		if (int_t != int_t_dt)
		{
			if ((rand()) / (RAND_MAX + 1.0) > likely) {
				Enemy enemy;
				float rand_x = 3.0f + 1.0f * (rand()) / (RAND_MAX + 1.0);

				float min = -1.0f;
				float max = +1.0f;
				float rand_y = min + (rand() % static_cast<int>(max - min + 1));

				enemy.transform.position = glm::vec3{ rand_x, 0.6 * rand_y, 0.1f };
				enemy.transform.rotation = glm::vec3{ 90.0f,0.0f,0.0f };
				enemy.transform.scale = glm::vec3{ 0.5f };
				enemy.boundingHeight = enemy.transform.scale.y;
				enemy.boundingWidth = enemy.transform.scale.x;
				enemy.enemyID = rand_y + 1;
				enemy.spawnedTime = t;
				enemies.push_back(enemy);
			}
		}
	}
#endif


}