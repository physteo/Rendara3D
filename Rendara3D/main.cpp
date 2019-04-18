
#if 0
#include "./Demos/OutBreak/OutBreak.h"

int main()
{
	OutBreak outBreak{ 1000, 1000 };
	outBreak.execute();

	return 0;
}

#endif


// demo shadows
#if 1
#include "./Demos/Shadows/ShadowsDemoGame.h"
#include "./Demos/Instancing/demo_instancing.h"

int main()
{	
	//Window window{ "hi", 1000., 1000. };
	//std::vector<Texture> loadedTextures;
	//ShadowsDemo demo{ window, loadedTextures };
	//ShadowsDemoGame game{1000,1000};
	//game.execute();
	return demo_instancing();
}
#endif


#if 0

#include "Chapters/Chapter1_gettingstarted.h"
#include "Chapters/Chapter2_lighting.h"
#include "Chapters/Chapter4_shadows.h"
//#include "Chapters/Chapter_practice.h"
//#include "Games/Game.h"
#include <irrKlang.h>

using namespace irrklang;


int main()
{
	//ISoundEngine *SoundEngine = createIrrKlangDevice();
	//SoundEngine->play2D("./res/sounds/bleep.mp3", GL_TRUE);


	// create window
	Window window{ "Example", 1000, 1000 };

	// create camera and projection
	Camera camera{ glm::vec3{0.0f, 0.0f, 5.0f}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f} };
	glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)window.getWidth() / (float)window.getHeight(), 0.1f, 50.0f);
	//glm::mat4 projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 1.0f, 10.0f);


	Shader objectsShader;
	objectsShader.generate("./res/shaders/objects_default.shader");
	Shader quads_with_alpha_shader;
	quads_with_alpha_shader.generate("./res/shaders/quads_with_alpha.shader");

	// create a model
	std::vector<Texture> loadedTextures;
	Model man{ "./res/model/quad/quad_walking_man.obj", &loadedTextures };
	glm::vec3 man_position = glm::vec3{ 0.0f, 0.0f, 0.0f };



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
