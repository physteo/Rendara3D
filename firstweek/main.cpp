
#include <iostream>


#include "Window.h"
#include "Camera.h"
#include "Model.h"


#if 1
int main()
{
	// create window
	Window window{ "Example", 1000, 1000 };

	// create camera and projection
	Camera camera{ glm::vec3{0.0f, 0.0f, 5.0f}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f} };
	glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)window.getWidth() / (float)window.getHeight(), 0.1f, 50.0f);
	//glm::mat4 projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 1.0f, 10.0f);

	// create a model
	std::vector<Texture> loadedTextures;
	Model box{ "./res/model/cube/cube.obj", glm::vec3{1.0f, 0.0f,0.0f},&loadedTextures };
	Model paper{ "./res/model/cube/cube.obj", glm::vec3{0.0f, 1.0f,0.0f},&loadedTextures };
	Model iron{ "./res/model/cube/cube.obj", glm::vec3{0.0f, 0.0f,1.0f},&loadedTextures };


	Shader objectsShader;
	objectsShader.generate("./res/shaders/objects_default.shader");


	while (!window.isClosed())
	{
		// ******* first stuff to do
		window.updateTime();
		window.clearColorBufferBit(0.5f, 0.5, 0.5f, 1.0f);


		// render stuff
		objectsShader.bind();
		objectsShader.setUniformMatrix("projection", projection, false);
		objectsShader.setUniformMatrix("view", camera.getViewMatrix(), false);

		objectsShader.setUniformValue("brightness", 2.0f);
		box.draw(1.0f, glm::vec3{ 0.0f, -1.5f, 0.0f }, glm::vec3{ 0.0f, 15.0f * window.getCurrentTime(), 0.0f }, objectsShader);

		objectsShader.bind();
		objectsShader.setUniformValue("brightness", 1.0f);
		iron.draw(1.0f, glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, - 50.0f + 15.0f * window.getCurrentTime(), 0.0f }, objectsShader);

		objectsShader.bind();
		objectsShader.setUniformValue("brightness", 0.75f);
		paper.draw(1.0f, glm::vec3{ 0.0f, +1.5f, 0.0f }, glm::vec3{ 0.0f, -150.0f - 15.0f * window.getCurrentTime(), 0.0f }, objectsShader);


		// game logic stuff

		// ******* last stuff to do
		window.swapBuffers();
		window.pollEvents();
		window.updateLastFrameTime();
	}

}
#endif

#if 0
int main()
{
	// create window
	Window window{ "Example", 1000, 1000 };

	// create camera and projection
	Camera camera{ glm::vec3{0.0f, 0.0f, 5.0f}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f} };
	glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)window.getWidth() / (float)window.getHeight(), 0.1f, 50.0f);
	//glm::mat4 projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 1.0f, 10.0f);

	// create a model
	std::vector<Texture> loadedTextures;
	Model man{ "./res/model/quad/quad_walking_man.obj", &loadedTextures };
	glm::vec3 man_position = glm::vec3{ 0.0f, 0.0f, 0.0f };

	Shader objectsShader;
	objectsShader.generate("./res/shaders/objects_default.shader");

	Shader quads_with_alpha_shader;
	quads_with_alpha_shader.generate("./res/shaders/quads_with_alpha.shader");


	while (!window.isClosed())
	{
		// ******* first stuff to do
		window.updateTime();
		window.clearColorBufferBit(0.5f, 0.5, 0.5f, 1.0f);

		float t = window.getCurrentTime();
		float dt = window.getLastFrameTime();
		// render stuff

		quads_with_alpha_shader.bind();
		quads_with_alpha_shader.setUniformMatrix("projection", projection, false);
		quads_with_alpha_shader.setUniformMatrix("view", camera.getViewMatrix(), false);
		quads_with_alpha_shader.setUniformValue("brightness", 1.0f);

		if (isKeyPressed(GLFW_KEY_LEFT, window.getGLFWwindow()))
		{
			glm::vec3 toAdd = -glm::vec3{ 2.0 * dt ,0.0f,0.0f };
			man_position += toAdd;
			quads_with_alpha_shader.setUniformValue("animation", 1);
			quads_with_alpha_shader.setUniformValue("t", (int)(t / 0.25f) % 4);
		}
		if (isKeyPressed(GLFW_KEY_RIGHT, window.getGLFWwindow()))
		{
			glm::vec3 toAdd = glm::vec3{ 2.0 * dt,0.0f,0.0f };
			man_position += toAdd;
			quads_with_alpha_shader.setUniformValue("animation", 0);
			quads_with_alpha_shader.setUniformValue("t", (int)(t / 0.25f) % 4);
		}

		man.draw(1.0f, man_position, glm::vec3{ 90.0f, 0.0f * window.getCurrentTime(), 0.0f }, quads_with_alpha_shader);



		// game logic stuff

		// ******* last stuff to do
		window.swapBuffers();
		window.pollEvents();
		window.updateLastFrameTime();
	}

}
#endif

#if 0
#include "Chapters/Chapter1_gettingstarted.h"
#include "Chapters/Chapter2_lighting.h"
#include "Chapters/Chapter4_shadows.h"
#include "Chapters/Chapter_practice.h"
#include "Games/Game.h"
int main()
{
	//return chapter1();
	//return chapter2();
	//return chapter4();
	//return chapter_practice();

	Game breakout{ 1000, 1000 };
	return breakout.execute();
}
#endif

#if 0
int maina()
{
	Window window{ "VAO testing", 1024, 768, Monitor::G_NOTSPECIFIED };;
	
	// create shader
	unsigned int program = createShader(vertexShader, fragmentShader);
	
	/* create VAO */
	unsigned int vao;
	GLCall(glGenVertexArrays(1, &vao));
	GLCall(glBindVertexArray(vao));
	/* Create VBO and bind -> gets connected to the currently bound VAO*/
	unsigned int vbo;
	GLCall(glGenBuffers(1, &vbo));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), verticesRectangle, GL_STATIC_DRAW));


	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0));
	GLCall(glEnableVertexAttribArray(0));

	/* create index buffer and bind -> gets connected to current VAO */
	unsigned int ibo;
	GLCall(glGenBuffers(1, &ibo));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indicesRectangle, GL_STATIC_DRAW));

	/* first unbind vao */
	GLCall(glBindVertexArray(0));

	/* then unbind the vbo and ibo: not necessary but avoids accidental writing */
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));


	while (!window.isClosed())
	{
		window.clearColorBufferBit();

		/* draw calls here */
		GLCall(glUseProgram(program));
		GLCall(glBindVertexArray(vao));
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
		GLCall(glBindVertexArray(0));
		
		window.swapBuffers();

		window.pollEvents();
	}

	window.terminate();
	
	return 0;
}

int maino()
{
	Window window{"First week", 1920, 1080, Monitor::G_NOTSPECIFIED };

	float r = 0.25;
	float t_x = 0.75;
	float t_y = 0.75;
	const float pi = 3.14159;
	const float to_rad = 2.0 * pi / 360.0;
	Vertex verticesPentagon[] =
	{
		{t_x + 0,                     t_y + 0},
		{t_x + r * cos(30 * to_rad),  t_y + r *  sin(30 * to_rad)},
		{t_x + r * cos(90 * to_rad),  t_y + r *  sin(90 * to_rad)},
		{t_x + r * cos(150 * to_rad), t_y + r * sin(150 * to_rad)},
		{t_x + r * cos(210 * to_rad), t_y + r * sin(210 * to_rad)},
		{t_x + r * cos(270 * to_rad), t_y + r * sin(270 * to_rad)},
		{t_x + r * cos(330 * to_rad), t_y + r * sin(330 * to_rad)},
	};
	unsigned int indicesPentagon[] =
	{
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 6,
		0, 6, 1
	};

	unsigned int rectangleBuffer = createShapeBuffer(verticesRectangle, 4);
	unsigned int rectangleIBO = createShapeIBO(indicesRectangle, 6);

	unsigned int pentagonBuffer = createShapeBuffer(verticesPentagon, 7);
	unsigned int pentagonIBO    = createShapeIBO(indicesPentagon, 18);




	unsigned int shader = createShader(vertexShader, fragmentShader);


	/* Loop until the user closes the window */
	while (!window.isClosed())
	{
		/* Render here */
		window.clearColorBufferBit();

		
		GLCall(glUseProgram(shader));

		GLCall(glBindBuffer(GL_ARRAY_BUFFER, rectangleBuffer));
		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x)));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectangleIBO));
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		GLCall(glDisableVertexAttribArray(0));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));



		GLCall(glBindBuffer(GL_ARRAY_BUFFER, pentagonBuffer));
		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x)));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pentagonIBO));
		GLCall(glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		GLCall(glDisableVertexAttribArray(0));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));



		GLCall(glUseProgram(0));





		/* Swap Front and Back buffers */
		window.swapBuffers();

		/* Poll for and process events */
		window.pollEvents();
	}

	glfwTerminate();
	return 0;

}

#endif