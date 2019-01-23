#include "Chapter_practice.h"


// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void moveMyVector(Window& window, glm::vec3& vector, float speed)
{
	if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_UP) == GLFW_PRESS)
		vector.x += speed * window.getLastFrameTime();
	if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_DOWN) == GLFW_PRESS)
		vector.x -= speed * window.getLastFrameTime();

	if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_RIGHT) == GLFW_PRESS)
		vector.y += speed * window.getLastFrameTime();
	if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_LEFT) == GLFW_PRESS)
		vector.y -= speed * window.getLastFrameTime();

	if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_Z) == GLFW_PRESS)
		vector.z += speed * window.getLastFrameTime();
	if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_X) == GLFW_PRESS)
		vector.z -= speed * window.getLastFrameTime();
}


#include <unordered_map>
int chapter_practice()
{
	/* window */
	float width = 1280.0;
	float height = 1280.0;
	Window window{ "Practice", width, height, G_NOTSPECIFIED };

	/* camera */
	Camera camera{ glm::vec3{-3.0f, 3.0f, 3.0f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 1.0f, 0.0f} };
	glm::mat4 projection = glm::perspective(glm::radians(90.0f), width / height, 0.1f, 50.0f);

	/* load models */
	std::vector<Texture> loadedTextures;
	Model cube{ "./res/model/cube/cube.obj", &loadedTextures };
	Model sphere{ "./res/model/sphere/sphere.obj", &loadedTextures };
	Model parquet{ "./res/model/parquet/parquet.obj", &loadedTextures };

	glm::vec3 spherePosition{ -3.0f, 0.0f, 0.0f };
	glm::vec3 cubePosition{ +3.0f, 0.0f, 0.0f };
	glm::vec3 displayPosition{ 2.0f, 6.0f, 0.0f };

	/* shaders */
	Shader shader; 
	shader.generate( "./res/shaders/objects_wlights.shader" );
	Shader shadowShader; 
	shadowShader.generate( "./res/shaders/depth.shader" );
	Shader debugDepth;
	debugDepth.generate( "./res/shaders/debugDepth.shader" );
	Shader cubeMapShader; 
	cubeMapShader.generate( "./res/shaders/cubemap.shader" );
	Shader lampShader; 
	lampShader.generate( "./res/shaders/1_lamp.shader" );
	Shader cubeDepthShader; 
	cubeDepthShader.generate( "./res/shaders/cubeDepth.shader" );

	Shader hdrShader;
	hdrShader.generate("./res/shaders/hdr.shader");


	/* lights */
	// sun
	glm::vec3 orangeColor   = glm::vec3{249.f, 113.f, 255.f} / 255.0f;
	glm::vec3 blueColor = glm::vec3{255.f, 255.f, 0.f} / 255.0f;
	glm::vec3 sunAmbient0  = 0.3f * 0.1f * orangeColor;
	glm::vec3 sunAmbient1  = 0.3f * 0.1f * blueColor;
	glm::vec3 sunAmbient2  = 0.3f * 0.1f * blueColor;
	glm::vec3 sunAmbient3  = 0.3f * 0.1f * orangeColor;
	glm::vec3 sunDiffuse0  = 0.3f * 0.8f * orangeColor;
	glm::vec3 sunDiffuse1  = 0.3f * 0.8f * blueColor;
	glm::vec3 sunDiffuse2  = 0.3f * 0.8f * blueColor;
	glm::vec3 sunDiffuse3  = 0.3f * 0.8f * orangeColor;
	glm::vec3 sunSpecular =	0.4f * 1.0f * glm::vec3{1.0f, 1.0f, 1.0f};
	float sun_nearPlane = 0.1f; float sun_farPlane = 20.0f; float sun_left = -10.0f; float sun_right = 10.0f; float sun_down = -10.0f; float sun_up = 10.0f;
	// create the lights
	std::vector<SunLight> suns;
	suns.push_back(SunLight{ glm::vec3{+4.08030128,3.94482803,-2.77766037}, glm::vec3{0.0f,0.0f,0.0f}, sunAmbient0, sunDiffuse0, sunSpecular });
	suns.push_back(SunLight{ glm::vec3{-4.08030128,3.94482803,-2.77766037}, glm::vec3{0.0f,0.0f,0.0f}, sunAmbient1, sunDiffuse1, sunSpecular });
	suns.push_back(SunLight{ glm::vec3{+4.08030128,3.94482803,+2.77766037}, glm::vec3{0.0f,0.0f,0.0f}, sunAmbient2, sunDiffuse2, sunSpecular });
	suns.push_back(SunLight{ glm::vec3{-4.08030128,3.94482803,+2.77766037}, glm::vec3{0.0f,0.0f,0.0f}, sunAmbient3, sunDiffuse3, sunSpecular });
	//  create shadows
	OrthoFrustrum frustrum(sun_nearPlane, sun_farPlane, sun_left, sun_right, sun_down, sun_up);
	std::vector<ShadowMap2D> sunShadows;
	for (size_t i = 0; i < suns.size(); i++)
	{
		sunShadows.push_back(ShadowMap2D(frustrum, 1024, 1024));
		sunShadows.back().generate();
	}
	// create a frustrum to be used by the light, and create a vector of lightSpaceMatrices that will be filled and used
	// during the game loop
	std::vector<glm::mat4> lightSpaceMatrices{suns.size()};

	// another cubeMap to be filled by a framebuffer
	glm::vec3 ambient  = 1.0f * 0.1f * 0.1f * glm::vec3{ 1.0f, 1.0f, 1.0f };
	glm::vec3 diffuse  = 1.0f * 1.0f * 0.8f * glm::vec3{ 1.0f, 1.0f, 1.0f };
	glm::vec3 specular = 1.0f * 1.0f * 1.0f * glm::vec3{ 1.0f, 1.0f, 1.0f };
	float constant  = 0.001f;
	float linear    = 0.05f;
	float quadratic = 0.1f;
	std::vector<PointLight> pointLights;
	pointLights.push_back(PointLight{ glm::vec3{+1.0f, 0.1f, 0.0f}, ambient, diffuse, specular, constant, linear, quadratic });
	//pointLights.push_back(PointLight{ glm::vec3{-1.0f, 0.2f, 0.0f}, ambient, diffuse, specular, constant, linear, quadratic });
	std::vector<ShadowCubeMap> pointShadows;
	for (size_t i = 0; i < pointLights.size(); i++)
	{
		pointShadows.push_back(ShadowCubeMap(1024, 1024));
		pointShadows.back().generate();
	}

	// HDRframebuffer
	unsigned int hdrFBO;
	glGenFramebuffers(1, &hdrFBO);

	unsigned int colorBufferTexture;
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



	while (!window.isClosed())
	{

		/* 0 -  time logic */
		window.updateTime();
		window.clearColorBufferBit(0.5f, 0.5f, 0.5f, 1.0f);

		/* render shadowmaps */
				// SunLights
				for (size_t i = 0; i < suns.size(); i++)
				{
					sunShadows.at(i).startShadows(window, shadowShader, &suns.at(i));
					sphere.draw(1.0f, spherePosition, glm::vec3{ 0.0f,0.0f,0.0f }, shadowShader);
					cube.draw(1.0f, cubePosition, glm::vec3{ 0.0f,0.0f,0.0f }, shadowShader);
					parquet.draw(1.0f, glm::vec3{ 0.0f,-1.0f,0.0f }, glm::vec3{ 0.0f,0.0f,0.0f }, shadowShader);
					sunShadows.at(i).stopShadows(window, shadowShader);
				}

				// PointLights
				for (size_t i = 0; i < pointLights.size(); i++)
				{
					pointShadows.at(i).startShadows(window, cubeDepthShader, pointLights.at(i));
					sphere.draw(1.0f, spherePosition, glm::vec3{ 0.0f,0.0f,0.0f }, cubeDepthShader);
					cube.draw(1.0f, cubePosition, glm::vec3{ 0.0f,0.0f,0.0f }, cubeDepthShader);
					parquet.draw(1.0f, glm::vec3{ 0.0f,-1.0f,0.0f }, glm::vec3{ 0.0f,0.0f,0.0f }, cubeDepthShader);
					pointShadows.at(i).stopShadows(window, cubeDepthShader);
				}

				// debug
				//sunShadow2.drawShadowMap(window, 256, 256, debugDepth);

		/* 1 - Rendering and game logic */
		// pass view transformations (camera position and perspective)
		shader.bind();
		shader.setUniformMatrix("projection", projection, false);
		shader.setUniformValue("cameraPos", camera.getEye());
		shader.setUniformMatrix("view", camera.getViewMatrix(), false);
		

		shader.setUniformValue("flashLight.ambient", 0.1f * glm::vec3{ 1.0f, 1.0f, 1.0f });
		shader.setUniformValue("flashLight.diffuse", 0.8f * glm::vec3{ 1.0f, 1.0f, 1.0f });
		shader.setUniformValue("flashLight.specular", 1.0f * glm::vec3{ 1.0f, 1.0f, 1.0f });
		shader.setUniformValue("flashLight.position", pointLights.at(0).eye);
		shader.setUniformValue("flashLight.direction", glm::vec3{ -0.2f,0.2f,-1.0f });
		shader.setUniformValue("flashLight.cutoff", glm::cos(glm::radians(20.0f)));
		shader.setUniformValue("flashLight.outerCutoff", glm::cos(glm::radians(25.0f)));
		shader.setUniformValue("flashLight.linear", 0.01f);
		shader.setUniformValue("flashLight.quadratic", 0.3f);
		shader.setUniformValue("flashLight.constant", 0.1f);

		// SunLights
		for (int i = 0; i < suns.size(); i++) {
			suns.at(i).cast("sun[" + std::to_string(i) + "]", shader);
			sunShadows.at(i).passUniforms(shader, "shadowMap[" + std::to_string(i) + "]", "lightSpaceMatrix[" + std::to_string(i) + "]", suns.at(i).getViewMatrix());
		}

		// PointLights
		for (size_t i = 0; i < pointLights.size(); i++)
		{
			pointLights.at(i).cast("pointLights[" + std::to_string(i) + "]", shader);
			pointShadows.at(i).passUniforms(shader, "cubeDepthMap[" + std::to_string(i) + "]", "farPlane");
		}


		shader.unbind();


		// cube maps shader debug
		//debug//cubeMapShader.bind();
		//debug//cubeMapShader.setUniformMatrix("view", camera.getViewMatrix(), false);
		//debug//cubeMapShader.setUniformMatrix("projection", projection, false);
		//debug//cubeMapShader.setUniformValue("cubeMap", 4);
		//debug//glActiveTexture(GL_TEXTURE0 + 4);
		//debug//glBindTexture(GL_TEXTURE_CUBE_MAP, omniTextureID);
		//debug//cubeMapShader.unbind();
		//debug//cube.draw(1.0f, displayPosition, glm::vec3{ 0.0f,0.0f,0.0f }, cubeMapShader);

		// draw calls
		// enable HDR framebuffer
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO));
		window.clearColorBufferBit(0.5f, 0.5f, 0.5f, 1.0f);
		// draw stuff
		sphere.draw(1.0f, spherePosition, glm::vec3{ 0.0f,0.0f,0.0f }, shader);
		cube.draw(1.0f, cubePosition, glm::vec3{ 0.0f,0.0f,0.0f }, shader);
		parquet.draw(1.0f, glm::vec3{ 0.0f,-1.0f,0.0f }, glm::vec3{ 0.0f,0.0f,0.0f }, shader);
		// disable HDR framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// render on screen
		hdrShader.bind();
		hdrShader.setTexture(GL_TEXTURE_2D, "hdrBuffer", colorBufferTexture);
		renderQuad();
		hdrShader.unbind();

		// lamps shader
		lampShader.bind();
		lampShader.setUniformMatrix("view", camera.getViewMatrix(), false);
		lampShader.setUniformMatrix("projection", projection, false);
		lampShader.unbind();
		for (size_t i = 0; i < pointLights.size(); i++)
		{	
			cube.draw(.1, pointLights.at(i).eye, glm::vec3{ 0.0f }, lampShader);
		}


		/* 2 - commands */
				camera.processCommands(window);
				for (size_t i = 0; i < pointLights.size(); i++)
				{
					moveMyVector(window, pointLights.at(i).eye, 2.5f);
				}
				//moveMyVector(window, displayPosition, 2.5f);


				/* 3 - clear buffers, swap buffers, poll events and update time */
				window.swapBuffers();
				window.pollEvents();
				window.updateLastFrameTime();
		
	}

	window.terminate();

	return 0;
}