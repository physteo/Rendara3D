
#if PR_DEBUG == 1
#define LOG(x) std::cout << x << std::endl;
#else
#define LOG(x)
#endif


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