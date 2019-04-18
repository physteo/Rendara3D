#include "demo_instancing.h"

void      position_cubes(InstanceSet<Particle>& cubes)
{
	std::vector<float> rads;
	std::vector<int> howMany;
	float tot = 0;
	for (size_t i = 0; i < 40; i++)
	{
		rads.push_back(0.25*i+2);
		howMany.push_back( 4.0f*int((2.0f*3.14159f)*rads.back()) );
		tot += howMany.back();
	}

	float ypos = 0.22f;

	for (size_t i = 0; i < rads.size(); i++)
	{
		for (size_t j = 0; j < howMany.at(i); j++)
		{
			float r = rads.at(i) + 0.2f*FLATRAND;
			float angle = (j/float(howMany.at(i))) * 360.0f + 5.0*FLATRAND;

			Particle newCube;
			newCube.transform.position = { r*cos(angle*2.0f*3.1415f/360.0f),ypos,r*sin(angle*2.0f*3.1415f / 360.0f) };
			newCube.transform.rotation = { 0.0f, -angle, 0.0f };
			newCube.transform.scale = 0.5f * glm::vec3{ 0.25f, 0.5f + 0.4f*FLATRAND, 0.25f };
			cubes.push_back(newCube);
		}
	}
}



float     command_sun(float angle, float dt, Window& window);
void      update_fire(InstanceSetQuads<Particle>& coloredQuads, float dt);
glm::vec3 sky_color(float sunHeight, float maxHeight);


int demo_instancing()
{
	/* window */
	float width = 1600.0;
	float height = 900.0;

	Window window{ "Practice", width, height, G_NOTSPECIFIED };

	/* camera */
	Camera camera{ glm::vec3{-3.0f, 3.0f, 3.0f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 1.0f, 0.0f} };
	glm::mat4 projection = glm::perspective(glm::radians(90.0f), width / height, 0.1f, 50.0f);

	/* load models */
	std::vector<Texture> loadedTextures;
	loadedTextures.reserve(100);
	Model cube{ "./res/model/cube/cube.obj",glm::vec3{115.,194.,251.} / 255.0f, &loadedTextures };
	Model sphere{ "./res/model/sphere/sphere.obj", &loadedTextures };
	Model piramid{ "./res/model/piramid/piramid.obj",glm::vec3{255.,60.,60.} / 255.0f,  &loadedTextures };
	Model parquet{ "./res/model/parquet/concentric.obj", &loadedTextures };
	//Model parquet{ "./res/model/floating/floating_place.obj", &loadedTextures };
	//Model parquet{ "./res/model/floating/floating_place.obj", &loadedTextures };
	ScreenQuad hdrQuad{};




	/* shaders */
	Shader hdrShader("./res/shaders/hdr.shader");
	Shader shadowShader("./res/shaders/depth.shader");
	Shader cubeMapShader("./res/shaders/cubemap.shader");
	Shader lampShader("./res/shaders/1_lamp.shader");
	Shader cubeDepthShader("./res/shaders/cubeDepth.shader");
	Shader shader{ "./res/shaders/objects_wlights.shader" };
	Shader debugDepth("./res/shaders/debugDepth.shader");
	glm::vec3 displayPosition{ 2.0f, 6.0f, 0.0f };


	Simple3DRenderer simple3DRenderer;
	Transform cubeTransform{   { -3.5f, 0.6f, 0.0f }, glm::vec3{0.0f}, glm::vec3{1.0f} };
	Transform cubeTransform2{   { -0.5f, 0.6f, 6.0f }, glm::vec3{0.0f}, glm::vec3{2.0f,1.0f,1.0f} };
	Transform cubeTransform3{   { +6.5f, 0.6f, -6.0f }, glm::vec3{0.0f,45.0f,0.0f}, glm::vec3{1.0f,2.0f,1.0f} };
	Transform sphereTransform{ { +0.75f, 0.25f, 0.0f }, glm::vec3{0.0f}, glm::vec3{1.0f} };
	Transform piramidTransform{ { -0.75f, 0.25f, 0.0f }, glm::vec3{0.0f}, glm::vec3{0.42f} };
	Transform parquetTransform{ { 0.0f,0.0f,0.0f }, glm::vec3{0.0f}, glm::vec3{1.25f} };

	// instancing
	Model quad{ "./res/model/quad/quad.obj", &loadedTextures };
	Shader instancesColoredQuadsShader{ "./res/shaders/instances_colored_quads.shader" };
	InstanceSetQuads<Particle> coloredQuads{ 20000 };
	coloredQuads.setModel(&quad);

	// objects
	Shader instancesObjectsShader  { "./res/shaders/instances_objects_wlights.shader" };
	Shader instancesSunShadowShader{ "./res/shaders/instances_depth.shader" };
	Shader instancesCubeDepthShader{ "./res/shaders/instances_cubeDepth.shader" };
	InstanceSet<Particle> cubesSet{ 7000 };
	cubesSet.setModel(&cube);
	position_cubes(cubesSet);

	/* lights */
	// sun's colors
	glm::vec3 orangeColor = glm::vec3{ 249.f, 113.f, 255.f } / 255.0f;
	glm::vec3 blueColor = glm::vec3{ 255.f, 255.f, 0.f } / 255.0f;

	glm::vec3 sunAmbient0 = 1.0f*0.1f * glm::vec3{1.0f};//orangeColor;
	glm::vec3 sunAmbient1 = 1.0f*0.1f * glm::vec3{1.0f};//blueColor;
	glm::vec3 sunAmbient2 = 1.0f*0.1f * glm::vec3{1.0f};//blueColor;
	glm::vec3 sunAmbient3 = 1.0f*0.1f * glm::vec3{1.0f};//orangeColor;
	glm::vec3 sunDiffuse0 = 1.0f*0.8f * glm::vec3{1.0f};//orangeColor;
	glm::vec3 sunDiffuse1 = 1.0f*0.8f * glm::vec3{1.0f};//blueColor;
	glm::vec3 sunDiffuse2 = 1.0f*0.8f * glm::vec3{1.0f};//blueColor;
	glm::vec3 sunDiffuse3 = 1.0f*0.8f * glm::vec3{1.0f};//orangeColor;
	glm::vec3 sunSpecular = 1.0f*1.0f * glm::vec3{ 1.0f, 1.0f, 1.0f };
	float sun_nearPlane = 1.1f; float sun_farPlane = 50.0f; float sun_left = -10.0f; float sun_right = 10.0f; float sun_down = -10.0f; float sun_up = 10.0f;

	// create lights
	std::vector<SunLight> suns;
	suns.push_back(SunLight{ glm::vec3{+4.08030128,-4.94482803,-2.77766037}, glm::vec3{0.0f,0.0f,0.0f}, sunAmbient0, sunDiffuse0, sunSpecular });
	//suns.push_back(SunLight{ glm::vec3{-4.08030128,3.94482803,-2.77766037}, glm::vec3{0.0f,0.0f,0.0f}, sunAmbient1, sunDiffuse1, sunSpecular });
	//suns.push_back(SunLight{ glm::vec3{+4.08030128,3.94482803,+2.77766037}, glm::vec3{0.0f,0.0f,0.0f}, sunAmbient2, sunDiffuse2, sunSpecular });
	//suns.push_back(SunLight{ glm::vec3{-4.08030128,3.94482803,+2.77766037}, glm::vec3{0.0f,0.0f,0.0f}, sunAmbient3, sunDiffuse3, sunSpecular });



	//  create shadowmaps
	std::vector<ShadowMap2D> sunShadows;
	for (size_t i = 0; i < suns.size(); i++)
	{
		sunShadows.emplace_back(ShadowMap2D{ sun_nearPlane, sun_farPlane, sun_left, sun_right, sun_down, sun_up, 1024, 1024 });
	}


	// create a frustrum to be used by the light, and create a vector of lightSpaceMatrices that will be filled and used
	// during the game loop
	std::vector<glm::mat4> lightSpaceMatrices{ suns.size() };

	// Setting up pointlights
	glm::vec3 ambient =  1.0f * 0.1f * 0.1f * glm::vec3{ 1.0f, 0.65f, 0.65f/ 5.0f };
	glm::vec3 diffuse =  1.0f * 1.0f * 0.8f * glm::vec3{ 1.0f, 0.65f, 0.65f/ 5.0f };
	glm::vec3 specular = 1.0f * 1.0f * 1.0f * glm::vec3{ 1.0f, 0.65f, 0.65f/ 5.0f };
	float constant = 0.001f;
	float linear = 0.05f;
	float quadratic = 0.1f;
	std::vector<PointLight> pointLights;
	pointLights.push_back(PointLight{ glm::vec3{+0.0f, 0.1f, 0.0f}, ambient, diffuse, specular, constant, linear, quadratic });


	std::vector<ShadowCubeMap> pointShadows;
	for (size_t i = 0; i < pointLights.size(); i++)
	{
		pointShadows.push_back(std::move(ShadowCubeMap(1024, 1024)));
	}

	// HDRframebuffer
	FrameBuffer hdrFB;
	hdrFB.attach2DTexture(GL_COLOR_ATTACHMENT0, window.getWidth(), window.getHeight(), 4, RGBA16, GL_FLOAT);
	hdrFB.attachRenderBuffer(GL_DEPTH_COMPONENT, window.getWidth(), window.getHeight());

	if (!hdrFB.iscomplete())
	{
		std::cerr << "Framebuffer not complete!" << std::endl;
		// TODO: add proper logging
	}


	hdrShader.bind();
	hdrShader.setUniformValue("exposure", 1.0f);
	hdrShader.unbind();

	float backR = 0.0f;
	float backG = 0.0f;
	float backB = 0.0f;
	float angle = -6.28f;

	while (!window.isClosed())
	{

		/* 0 -  time logic */
		window.updateTime();
		window.clearColorBufferBit(backR, backG, backB, 1.0f);
		float dt = window.getLastFrameTime();
		float t = window.getCurrentTime();

		// update fire's particles
		update_fire(coloredQuads, dt);
		if (int(t / dt) % 10 == 0)
		{
			float addLight = 0.05 * ((rand() / (RAND_MAX + 1.0f)) - 0.5);
			pointLights.at(0).ambientColor = ambient + 0.5f * addLight;
			pointLights.at(0).specularColor = specular + 2.0f*addLight;
			pointLights.at(0).diffuseColor = diffuse + 2.0f*addLight;
		}
		if (int(t / dt) % 3 == 0)
		{
			float addLight = 0.02 * ((rand() / (RAND_MAX + 1.0f)) - 0.5);
			pointLights.at(0).ambientColor = ambient + 0.5f * addLight;
			pointLights.at(0).specularColor = specular + 2.0f*addLight;
			pointLights.at(0).diffuseColor = diffuse + 2.0f*addLight;
			float z_shift = 0.05f*((rand() / (RAND_MAX + 1.0f)) - 0.5);
			pointLights.at(0).eye = glm::vec3{ +0.0f, 1.1f + z_shift, 0.0f };
		}


		/* 1 - Rendering  */
		simple3DRenderer.submit({ &cube, cubeTransform , &shader });
		simple3DRenderer.submit({ &cube, cubeTransform2, &shader });
		simple3DRenderer.submit({ &cube, cubeTransform3, &shader });

		simple3DRenderer.submit({ &sphere, sphereTransform  , &shader });
		simple3DRenderer.submit({ &piramid, piramidTransform, &shader });
		simple3DRenderer.submit({ &parquet, parquetTransform, &shader });
		for (size_t i = 0; i < pointLights.size(); i++)
		{
			simple3DRenderer.submit({ &cube, Transform{ pointLights.at(0).eye, glm::vec3{0.0f}, glm::vec3{.1f} }, &lampShader });
		}

		/* render shadowmaps */
		// clear shadowmaps of SunLights
		for (size_t i = 0; i < suns.size(); i++)
		{
			sunShadows.at(i).clearShadows();
		}
		// clear shadowmaps of PointLights
		for (size_t i = 0; i < pointLights.size(); i++)
		{
			pointShadows.at(i).clearShadows();
		}
		// SunLights
		for (size_t i = 0; i < suns.size(); i++)
		{
			sunShadows.at(i).startShadows(window, shadowShader, &suns.at(i));
			simple3DRenderer.draw(&shadowShader);
			sunShadows.at(i).stopShadows(window, shadowShader);
		}
		for (size_t i = 0; i < suns.size(); i++)
		{
			sunShadows.at(i).startShadows(window, instancesSunShadowShader, &suns.at(i));
			cubesSet.drawInstances(instancesSunShadowShader);
			sunShadows.at(i).stopShadows(window, instancesSunShadowShader);
		}

		// PointLights
		for (size_t i = 0; i < pointLights.size(); i++)
		{
			pointShadows.at(i).startShadows(window, cubeDepthShader, pointLights.at(i));
			simple3DRenderer.draw(&cubeDepthShader);
			pointShadows.at(i).stopShadows(window, cubeDepthShader);
		}
		// PointLights
		for (size_t i = 0; i < pointLights.size(); i++)
		{
			pointShadows.at(i).startShadows(window, instancesCubeDepthShader, pointLights.at(i));
			cubesSet.drawInstances(instancesCubeDepthShader);
			pointShadows.at(i).stopShadows(window, instancesCubeDepthShader);
		}

		// draw calls
		// enable HDR framebuffer
		hdrFB.bind();
		window.clearColorBufferBit(backR, backG, backB, 1.0f);

		// pass view transformations (camera position and perspective)
		shader.bind();
		shader.setUniformMatrix("projection", projection, false);
		shader.setUniformValue("cameraPos", camera.getEye());
		shader.setUniformMatrix("view", camera.getViewMatrix(), false);

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
		
		instancesObjectsShader.bind();
		instancesObjectsShader.setUniformMatrix("view", camera.getViewMatrix(), false);
		instancesObjectsShader.setUniformMatrix("projection", projection, false);
		instancesObjectsShader.setUniformValue("cameraPos", camera.getEye());

		suns.at(0).cast("sun[0]", instancesObjectsShader);
		sunShadows.at(0).passUniforms(instancesObjectsShader, "shadowMap[0]", "lightSpaceMatrix[0]", suns.at(0).getViewMatrix());
		pointLights.at(0).cast("pointLights[0]", instancesObjectsShader);
		pointShadows.at(0).passUniforms(instancesObjectsShader, "cubeDepthMap[0]", "farPlane");
		//instancesObjectsShader.unbind();


		// draw stuff
		simple3DRenderer.draw(); // they're using their own shaders
		cubesSet.drawInstances(instancesObjectsShader);

		instancesColoredQuadsShader.bind();
		instancesColoredQuadsShader.setUniformMatrix("view", camera.getViewMatrix(), false);
		instancesColoredQuadsShader.setUniformMatrix("projection", projection, false);
		instancesColoredQuadsShader.setUniformValue("brightness", 1.0f);
		coloredQuads.drawInstances(instancesColoredQuadsShader);

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


		simple3DRenderer.clear();

		// moving the sun
		float height = 4.0;
		angle = command_sun(angle, dt, window);
		suns.at(0).eye = glm::vec3{4.0f, height * glm::sin(0.25f*angle), height * glm::cos(0.25f*angle)};
		suns.at(0).ambientColor  = (0.5f*(suns.at(0).eye.y/height) +0.5f)* sunAmbient0;
		suns.at(0).diffuseColor  = (0.5f*(suns.at(0).eye.y/height) +0.5f)* sunDiffuse0;
		suns.at(0).specularColor = (0.5f*(suns.at(0).eye.y/height) +0.5f)* sunSpecular;
		glm::vec3 backColor = sky_color(suns.at(0).eye.y, height);
		backR = backColor.r;
		backG = backColor.g;
		backB = backColor.b;

		/* 2 - commands */
		camera.processCommands(window);


		/* 3 - clear buffers, swap buffers, poll events and update time */
		window.swapBuffers();
		window.pollEvents();
		window.updateLastFrameTime();

	}

	window.terminate();

	return 0;
}


void create_new_particle(Particle& newParticle, int i, int j)
{
	float radius = 0.01 + i / 60.0 + 0.01 *(rand() / (RAND_MAX + 1.0f));
	float angle  = 0.0f + j * 360.0 / 30.0;

	float x = radius * sin(angle + 1.0 * (rand() / (RAND_MAX + 1.0f)));
	float z = radius * cos(angle + 1.0 * (rand() / (RAND_MAX + 1.0f)));

	float y = 0.5f;

	glm::vec3 scale{ 0.1f, 10.6f + 0.05 * (rand() / (RAND_MAX + 1.0f)), 0.1f };
	glm::vec3 rotation{ 90.0f, angle, 0.0f };

	newParticle.transform.position = glm::vec3{ x, y, z };
	newParticle.transform.scale = scale;
	newParticle.transform.rotation = rotation;

	float random_green = 0.4 + 0.5 * (rand()) / (RAND_MAX + 1.0);
	newParticle.color = glm::vec4{ 1.0f, random_green, random_green / 5.0f, 1.0f };
	newParticle.tau = 1.0f + 0.5*(rand() / (RAND_MAX + 1.0f));
	newParticle.direction = glm::vec3{ 0.0f,1.0f,0.0f };

}

void update_fire(InstanceSetQuads<Particle>& coloredQuads, float dt)
{
	for (int i = 0; i < 10; i++) 
	{
		Particle newParticle;
		create_new_particle(newParticle, 30.0 * (rand() / (RAND_MAX + 1.0f)), 30.0 * (rand() / (RAND_MAX + 1.0f)));
		coloredQuads.push_back(newParticle);
	}

	for (size_t i = 0; i < coloredQuads.size(); i++)
	{
		Particle particle = coloredQuads.getElement(i);
		particle.tau -= dt;
		particle.transform.position += dt * particle.direction;

		if (particle.tau <= 0.0f)
		{
			coloredQuads.deleteElement(i);
		}
		else
		{
			coloredQuads.setElement(i, particle);
		}
	}
}



glm::vec3 sky_color(float sunHeight, float maxHeight)
{
	// sun Height is between maxHeight and -maxHeight
	// when it's zero, i want the red color
	// when it's maxHeight, I want the blue color
	// below zero, keep it red#7ecbff
	const int numColors = 17;
	std::array<glm::vec3, numColors> colors = {
		glm::vec3(145, 186, 207)/255.0f,
		glm::vec3(139, 170, 188)/255.0f,
		glm::vec3(140, 161, 176)/255.0f,
		glm::vec3(146, 147, 155)/255.0f,
		glm::vec3(161, 142, 142)/255.0f,
		glm::vec3(161, 130, 124)/255.0f,
		glm::vec3(155, 114, 105)/255.0f,
		glm::vec3(149, 103, 96)	/255.0f,
		glm::vec3(138, 88, 81)	/255.0f,
		glm::vec3(123, 69, 66)	/255.0f,
		glm::vec3(115, 61, 60)	/300.0f,
		glm::vec3(99, 49,  52)	/400.0f,
		glm::vec3(86, 40,  46)	/500.0f,
		glm::vec3(36, 26,  36)   /600.0f,
		glm::vec3(39, 21,  32)   /800.0f,
		glm::vec3(34, 19,  32)   /1000.0f,
		glm::vec3(0.0f)};

	std::reverse(std::begin(colors), std::end(colors));

	if (sunHeight < 0.0 )
	{
		return colors.front();
	}
	else {
		double choice = ((sunHeight / maxHeight) * (numColors-1.0));

		glm::vec3 first  = colors.at(floor(choice));
		glm::vec3 second = colors.at(floor(choice) + 1);
		
		return first + (second - first) * (float)(choice - floor(choice));
	}


}


float command_sun(float angle, float dt, Window& window)
{
	if (isKeyPressed(GLFW_KEY_UP, window))
	{
		return angle + 1.5f*dt;
	}
	if (isKeyPressed(GLFW_KEY_DOWN, window))
	{
		return angle - 1.5f*dt;
	}
}