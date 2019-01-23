#include "Game.h"



Game::Game(GLuint widthIn, GLuint heightIn) :
	loadedTextures{},
	window{ "game", widthIn, heightIn, Monitor::G_NOTSPECIFIED }
{
	width = widthIn;
	height = heightIn;
	state = GAME_MENU;
}



int Game::execute()
{

	InstanceSet<GameObject> instanceExamples(6500);

	// HDR
	Quad hdrQuad{};
	Shader hdrShader;
	hdrShader.generate("./res/shaders/hdr.shader");
	// create the HDRframebuffer
	unsigned int hdrFBO;
	unsigned int colorBufferTexture;
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


	Shader sunShadowShaderInstances;
	sunShadowShaderInstances.generate("./res/shaders/instances_depthShader.shader");

	Shader sunShadowShader;
	sunShadowShader.generate("./res/shaders/depthShader.shader");


	Camera camera{ glm::vec3{0.0f, 5.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{-1.0f, 0.0f, 0.0f} };
	glm::mat4 projection = glm::ortho(-12.0f, 0.5f,
			-12.0f, 0.5f, 1.0f, 10.0f);
	//Model brickModel{ "./res/model/container/container.obj", &loadedTextures };
	Model brickModel{ "./res/model/sphere/sphere.obj", &loadedTextures };
	Model parquet{ "./res/model/parquet/parquet.obj", &loadedTextures };
	GameObject background;
	background.transform.scale = glm::vec3{ 1.2f };
	background.transform.position = glm::vec3{ 12.5 / 2.0 - 0.5f * 1.0, 0.0f,12.5 / 2.0 - 0.5f * 1.0 };
	background.transform.rotation = glm::vec3{ 0.0f,0.0f,0.0f };
	background.model = &parquet;


	Shader particlesShader;
	particlesShader.generate("./res/shaders/instances_objects_practice.shader");
	
	Shader objectsShader;
	objectsShader.generate("./res/shaders/objects_practice.shader");

	Shader cubeDepthShader;
	cubeDepthShader.generate("./res/shaders/cubeDepth.shader");

	Shader cubeDepthShaderInstances;
	cubeDepthShaderInstances.generate("./res/shaders/instances_cubeDepth.shader");


	// lights
	SunLight sun{ glm::vec3{ +3.0f, 7.0f, 3.0f }, glm::vec3{ 10.0f,0.0f,10.0f }, 0.0f * glm::vec3{1.0f}, 0.0f * glm::vec3{1.0f}, 0.0f * glm::vec3{1.0f} };
	OrthoFrustrum orthoFrustrum{ 0.1f, 20.0f, -10.0f, 10.0f, -10.0f, 10.0f };
	ShadowMap2D sunShadowMap{ orthoFrustrum, 1024.0, 1024.0 };
	sunShadowMap.generate();

	const glm::vec3 ambient = 1.0f * 0.1f * 0.1f * glm::vec3{ 1.0f, 1.0f, 1.0f };
	const glm::vec3 diffuse = 1.0f * 1.0f * 0.8f * glm::vec3{ 1.0f, 1.0f, 1.0f };
	const glm::vec3 specular = 1.0f * 1.0f * 1.0f * glm::vec3{ 1.0f, 1.0f, 1.0f };
	const float     constant = 0.001f;
	const float     linear = 0.05f;
	const float     quadratic = 0.1f;
	PointLight pointLight{ glm::vec3{5.0f, 2.0f, 5.0f}, ambient, diffuse, specular, constant, linear, quadratic };
	ShadowCubeMap pointShadow{ 1024, 1024 };
	pointShadow.generate();

	srand(glfwGetTime()); // initialize random seed	
	for (size_t i = 0; i < 40; i++)
	{
		for (size_t j = 0; j < 40; j++)
		{
			float scale = 1.0f;//(rand() % 20) / 100.0f + 0.05;
			
			Brick exampleObject;
			exampleObject.model = &brickModel;
			exampleObject.destructible = true;
			exampleObject.transform.position = glm::vec3{ 0.0f + i / 2.0, 0.0f ,0.0f + j / 2.0 };
			exampleObject.transform.rotation = glm::vec3{ 0.0f };
			exampleObject.transform.scale = glm::vec3{ 0.8f * scale };
			instanceExamples.push_back(exampleObject);

		}
	}

	instanceExamples.setModel(&brickModel);



	// without instance example
	std::vector<std::vector<int> > layout_0 = {{2,3,2,3,2,3,2,3,2,3,2,3}};

	std::vector<std::vector<int> > layout_1 = { {2,3,2,3,2,3,2,3,2,3,2,3},
											  {3,2,3,2,3,2,3,2,3,2,3,2},
											  {1,1,1,0,1,1,0,1,1,0,1,1} };


	std::vector<GameLevel> levels;
	levels.emplace_back( window, &loadedTextures );
	levels.emplace_back( window, &loadedTextures );
	levels.at(0).load(layout_0);
	levels.at(1).load(layout_1);

	size_t level_id = 0;

	while (!window.isClosed())
	{
		// ******* first stuff to do
		window.updateTime();
		window.clearColorBufferBit(0.5f, 0.5f, 0.5f, 1.0f);


		// ******* graphics, physics and game logic
			// graphics
		levels.at(level_id).render(window);
		
			// game logic: commands, events
		levels.at(level_id).update(window);
		
		// ******* advance levels
		if (levels.at(level_id).isCompleted())
		{
			if (level_id < levels.size() - 1)
			{
				level_id += 1;
			}
			else
			{
				// game is finished
				window.close();
			}
		}

#if 0


		sunShadowMap.startShadows(window, sunShadowShader, &sun);
		background.model->draw(background.transform.scale, background.transform.position, background.transform.rotation, sunShadowShader);
		sunShadowMap.stopShadows(window, sunShadowShader);

		pointShadow.startShadows(window, cubeDepthShader, pointLight);
		background.model->draw(background.transform.scale, background.transform.position, background.transform.rotation, cubeDepthShader);
		pointShadow.stopShadows(window, cubeDepthShader);



		sunShadowMap.startShadows(window, sunShadowShaderInstances, &sun);
		instanceExamples.drawInstances(sunShadowShaderInstances);
		sunShadowMap.stopShadows(window, sunShadowShaderInstances);

		pointShadow.startShadows(window, cubeDepthShaderInstances, pointLight);
		instanceExamples.drawInstances(cubeDepthShaderInstances);
		pointShadow.stopShadows(window, cubeDepthShaderInstances);


		window.clearColorBufferBit(0.5f, 0.5f, 0.5f, 1.0f);

		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO));
		window.clearColorBufferBit(0.5f, 0.5f, 0.5f, 1.0f);

		// draw without instance
		objectsShader.bind();
		objectsShader.setUniformMatrix("view", camera.getViewMatrix(), false);
		objectsShader.setUniformMatrix("projection", projection, false);
		objectsShader.setUniformValue("cameraPos", camera.getEye());
		// pass sun and shadows
		sun.cast("sun[0]", objectsShader);
		sunShadowMap.passUniforms(objectsShader, "shadowMap[0]", "lightSpaceMatrix[0]", sun.getViewMatrix());

		pointLight.cast("pointLights[0]", objectsShader);
		pointShadow.passUniforms(objectsShader, "cubeDepthMap[0]", "farPlane");

		background.model->draw(background.transform.scale, background.transform.position, background.transform.rotation, objectsShader);

		// draw with instances
		particlesShader.bind();
		particlesShader.setUniformMatrix("view", camera.getViewMatrix(), false);
		particlesShader.setUniformMatrix("projection", projection, false);
		particlesShader.setUniformValue("cameraPos", camera.getEye());

		// pass sun and shadows
		sun.cast("sun[0]", particlesShader);
		sunShadowMap.passUniforms(particlesShader, "shadowMap[0]", "lightSpaceMatrix[0]", sun.getViewMatrix());

		pointLight.cast("pointLights[0]", particlesShader);
		pointShadow.passUniforms(particlesShader, "cubeDepthMap[0]", "farPlane");

		// new instance stuff
		instanceExamples.drawInstances(particlesShader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// render on screen
		hdrShader.bind();
		hdrShader.setTexture(GL_TEXTURE_2D, "hdrBuffer", colorBufferTexture);
		hdrQuad.draw();
		hdrShader.unbind();
#endif
	

		//// move one object
		//float t = window.getCurrentTime();
		//
		//
		//size_t i;
		//for (i = 0; i < instanceExamples.size(); i++)
		//{
		//	GameObject e = instanceExamples.getElement(i);
		//	e.transform.position.x += ((rand() % 10) -4.5) / 160.0f;
		//	e.transform.position.z += ((rand() % 10) -4.5) / 160.0f;
		//	instanceExamples.setElement(i, e);	
		//	float r = (rand() / (RAND_MAX + 1.0));
		//	//if ( r < 0.01)
		//	//	instanceExamples.deleteElement(i);
		//}
		//
		//pointLight.eye.x = 5.0 + 3.0 * sin(t / 2.0);
		//pointLight.eye.y = 2.0 + 0.75 * sin(t / 2.0);


		//std::cout << t << ", " << 1.0 / window.getLastFrameTime() << std::endl;

		// ******* last stuff to do
		window.swapBuffers();
		window.pollEvents();
		window.updateLastFrameTime();
	}

	return 0;
}