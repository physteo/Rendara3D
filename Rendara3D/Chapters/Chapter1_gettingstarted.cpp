#include "Chapter1_gettingstarted.h"

int chapter1()
{
	Window window{ "VAO testing", 1024, 1024, Monitor::G_NOTSPECIFIED };;
	float clearColor[] = { 0.8039f, 0.3608f, 0.3608f, 1.0f };

	float to_rad = glm::two_pi<float>() / 360.0;
	float t_x = +0.0;
	float t_y = -0.0;
	float r = 1.0;

	std::vector<unsigned int> indicesRectangle =
	{
		0, 1, 2, 0, 2, 3
	};

	// create shader
	Shader shader{ "./res/shaders/basic.shader" };

	std::vector<float> coordinates;
	glm::vec3 center{ 0.0f, 0.0f, 0.0f };
	glm::vec3 radius{ 1.0f, 0.0f, 0.0f };

	int edges = 6;
	coordinates.push_back(center.x);
	coordinates.push_back(center.y);
	coordinates.push_back(center.z);
	for (size_t i = 0; i < edges; i++)
	{
		glm::vec4 position{ 0.0f, 0.0f, 0.0f, 1.0f };
		glm::mat4 trans1 = glm::translate(glm::mat4{ 1.0 }, center);
		glm::mat4 trans2 = glm::translate(glm::mat4{ 1.0 }, radius);

		float angle = 0 + i * glm::degrees(glm::two_pi<float>() / (edges));
		glm::mat4 rotate = glm::rotate(glm::mat4{ 1.0 }, glm::radians(angle), glm::vec3{ 0.0f,0.0f,1.0f });

		position = trans1 * (rotate * trans2) * position;

		coordinates.push_back(position.x);
		coordinates.push_back(position.y);
		coordinates.push_back(position.z);
	}

	std::vector<float> color = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	std::vector<float> textureCoords = { 0.5f + 0.0f, 0.5f + 0.0f,
		  0.5f + 0.5f * cos(30 * to_rad),  0.5f + 0.5f * sin(30 * to_rad),
		  0.5f + 0.5f * cos(90 * to_rad),  0.5f + 0.5f * sin(90 * to_rad),
		  0.5f + 0.5f * cos(150 * to_rad), 0.5f + 0.5f * sin(150 * to_rad),
		  0.5f + 0.5f * cos(210 * to_rad), 0.5f + 0.5f * sin(210 * to_rad),
		  0.5f + 0.5f * cos(270 * to_rad), 0.5f + 0.5f * sin(270 * to_rad),
		  0.5f + 0.5f * cos(330 * to_rad), 0.5f + 0.5f * sin(330 * to_rad),
	};

	std::vector<unsigned int> indices{
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 6,
		0, 6, 1
	};

	// create a VAO
	VertexArray test{};
	test.generate();
	test.bind();
	test.fillData({ coordinates, color, textureCoords }, { 3, 3, 2 });
	test.setIbo(indices);
	test.unbind();

	coordinates[2] += 0.21;
	coordinates[5] += 0.21;
	coordinates[8] += 0.21;
	coordinates[11] += 0.21;
	coordinates[14] += 0.21;
	coordinates[17] += 0.21;
	coordinates[20] += 0.21;
	// create a VAO
	VertexArray test2{};
	test2.generate();
	test2.bind();
	test2.fillData({ coordinates, color, textureCoords }, { 3, 3, 2 });
	test2.setIbo(indices);
	test2.unbind();

	/* load texture*/
	Texture::set2DTextureParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	Texture::set2DTextureParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	Texture::set2DTextureParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	Texture::set2DTextureParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	Texture textureGround;
	textureGround.generate("./res/textures/ground.jpg", Format::sRGBA);
	Texture textureFace;
	textureFace.generate("./res/textures/awesomeface.png", Format::sRGBA);

	VertexArray line{};
	line.generate();
	line.bind();
	line.fillData({ {0.0f, 0.0f, 0.0f,/**/ 0.5f, 0.0, 0.0f} }, { 3 });
	line.unbind();

	/* camera */
	Camera camera;
	camera.setCenter({ 0.0f,0.0f,-1.0f });
	camera.setEye({ 0.0f,0.0f,3.0f });
	camera.setUp({ 0.0f,1.0f,0.0f });

	float startingTime = glfwGetTime();
	while (!window.isClosed())
	{
		window.updateTime();
		window.clearColorBufferBit(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

		/* mechanics */
		float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;

		/* draw calls from here */
		shader.bind();

		// for textures
		shader.setUniformValue("texture1", 0);
		shader.setUniformValue("texture2", 1);
		Texture::setActiveTexture(GL_TEXTURE0);
		textureGround.bind();
		Texture::setActiveTexture(GL_TEXTURE1);
		textureFace.bind();

		// tutorial reference frames in opengl
		glm::mat4 model = glm::rotate(glm::mat4{ 1.0 }, glm::radians(0.0f), glm::vec3{ 1.0f, 0.0f, 0.0f });
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)(window.getWidth() / window.getHeight()), 0.01f, 100.0f);
		shader.setUniformMatrix("model", model, false);
		shader.setUniformMatrix("view", view, false);
		shader.setUniformMatrix("projection", projection, false);

		test2.bind();
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
		test2.unbind();

		test.bind();
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
		test.unbind();

		glBindTexture(GL_TEXTURE_2D, 0);

		shader.unbind();

		/* process commands */
		camera.processCommands(window);

		window.swapBuffers();
		window.pollEvents();
		window.updateLastFrameTime();
	}

	window.terminate();

	return 0;
}