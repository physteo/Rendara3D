#include "Chapter4_shadows.h"


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



/* lights */
std::vector<glm::vec3> pointLightPositions = {
	glm::vec3( 1.0f,  1.0f, -3.0f),
	glm::vec3( 0.7f,  0.2f,  2.0f),
	//glm::vec3( 2.3f, -3.3f, -4.0f),
	//glm::vec3( -4.0f,  2.0f, -12.0f)
};
glm::vec3 lightPosition = { 0.05000162,0.800001025,0.95000362 };


 
// to debug ############################################################################
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
// ############################################################################

// to debug cubemaps
float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

float cubeVertices[] = {
	// positions          // normals
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,

	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,

	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,

	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
};

#include "stb_image/stb_image.h"
unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		stbi_set_flip_vertically_on_load(false);
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
//


void moveVector(Window& window, glm::vec3& vector, float speed)
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

int chapter4()
{

	Window window("Lighting", 1400, 1400, Monitor::G_NOTSPECIFIED);

	/* create shaders */
	Shader shader("./res/shaders/objects_shadows.shader");
	Shader lampShader("./res/shaders/1_lamp.shader");
	Shader depthShader("./res/shaders/depthShader.shader");
	Shader cubeDepthShader("./res/shaders/cubeDepth.shader");
	
	Shader debugDepth("./res/shaders/debugDepth.shader");
	Shader debugCubeDepth("./res/shaders/debugCubeDepth.shader");


	/* textures options */
	Texture::set2DTextureParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	Texture::set2DTextureParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	Texture::set2DTextureParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	Texture::set2DTextureParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	/* create models */
	std::vector<Texture> loadedTextures;
	Model cube{ "./res/model/monkey/cube.obj", &loadedTextures };
	Model simple_cube{ "./res/model/cube/cube.obj", &loadedTextures };

	Model hut{ "./res/model/hut/hut.obj", &loadedTextures };
	Model sphere{ "./res/model/sphere/sphere.obj", &loadedTextures };
	Model light{ "./res/model/lights/point_light.obj", &loadedTextures };



	/* camera */
	//Camera camera({ -1.54360628, -0.855141640, 2.99689507 }, { 0.0f, 0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f });
	Camera camera({ 0.0, 0.0, 3.0 }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });


	/* projection matrix */
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)(window.getWidth() / window.getHeight()), 0.1f, 100.0f);

	/* setting up shadows */
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
	unsigned int depthMapFBOdir;
	GLCall(glGenFramebuffers(1, &depthMapFBOdir));
	unsigned int depthMap;
	GLCall(glGenTextures(1, &depthMap));
	GLCall(glBindTexture(GL_TEXTURE_2D, depthMap));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
	Texture::set2DTextureParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	Texture::set2DTextureParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	Texture::set2DTextureParameter(GL_TEXTURE_BORDER_COLOR, borderColor);
	Texture::set2DTextureParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	Texture::set2DTextureParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOdir));
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0));
	GLCall(glDrawBuffer(GL_NONE));
	GLCall(glReadBuffer(GL_NONE));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));


	glm::vec3 sunEye = { 4.08030128,3.94482803,-2.77766037 };
	glm::vec3 sunCenter = { 0.0f, 0.0f, 0.0f };

	/* omnidirectional shadow mapping */
	glm::vec3 specialPointLightPos{1.0f, 0.2f, 1.0f};
	unsigned int depthMapFBOomni;
	GLCall(glGenFramebuffers(1, &depthMapFBOomni));
	unsigned int depthCubeMap;
	GLCall(glGenTextures(1, &depthCubeMap));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap));
	for (size_t i = 0; i < 6; i++)
	{
		GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
	}
	Texture::set2DTextureParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	Texture::set2DTextureParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	Texture::set2DTextureParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	Texture::set2DTextureParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//Texture::set2DTextureParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//Texture::set2DTextureParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//Texture::set2DTextureParameter(GL_TEXTURE_BORDER_COLOR, borderColor);
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOomni));
	GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0));
	GLCall(glDrawBuffer(GL_NONE));
	GLCall(glReadBuffer(GL_NONE));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));


	// create the projective matrix for the 6 faces of the cubemap
	float aspect = (float)SHADOW_HEIGHT / (float)SHADOW_WIDTH;
	float near = 1.0f;
	float far  = 25.0f;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(specialPointLightPos, specialPointLightPos + glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec3{0.0f,-1.0f, 0.0f}));
	shadowTransforms.push_back(shadowProj * glm::lookAt(specialPointLightPos, specialPointLightPos + glm::vec3{-1.0f, 0.0f, 0.0f }, glm::vec3{0.0f,-1.0f, 0.0f}));
	shadowTransforms.push_back(shadowProj * glm::lookAt(specialPointLightPos, specialPointLightPos + glm::vec3{ 0.0f, 1.0f, 0.0f }, glm::vec3{0.0f, 0.0f, 1.0f}));
	shadowTransforms.push_back(shadowProj * glm::lookAt(specialPointLightPos, specialPointLightPos + glm::vec3{ 0.0f,-1.0f, 0.0f }, glm::vec3{0.0f, 0.0f,-1.0f}));
	shadowTransforms.push_back(shadowProj * glm::lookAt(specialPointLightPos, specialPointLightPos + glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec3{0.0f,-1.0f, 0.0f}));
	shadowTransforms.push_back(shadowProj * glm::lookAt(specialPointLightPos, specialPointLightPos + glm::vec3{ 0.0f, 0.0f,-1.0f }, glm::vec3{0.0f,-1.0f, 0.0f}));


	// debugging
	// skybox VAO
	std::vector<std::string> faces
	{
		    "C:/Users/matte/source/repos/OpenGL/firstweek/res/textures/skybox1/right.jpg",
			"C:/Users/matte/source/repos/OpenGL/firstweek/res/textures/skybox1/left.jpg",
			"C:/Users/matte/source/repos/OpenGL/firstweek/res/textures/skybox1/top.jpg",
			"C:/Users/matte/source/repos/OpenGL/firstweek/res/textures/skybox1/bottom.jpg",
			"C:/Users/matte/source/repos/OpenGL/firstweek/res/textures/skybox1/front.jpg",
			"C:/Users/matte/source/repos/OpenGL/firstweek/res/textures/skybox1/back.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	// cube VAO
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	/* end omnidirectional shadow mapping */

	float cubeRotation = 0.0f;
	glm::vec3 spherePosition = {-1.0f, 0.2f, 0.0f};
	glm::vec3 sphereRadians = {0.0f, 0.0f, 0.0f};

	while (!window.isClosed())
	{
		window.updateTime();
		window.clearColorBufferBit(0.01f, 0.01f, 0.01f, 1.0f);

		/*  compute shadows by rendering the scene from light's perspective into
		a texture which will be used in the rendering of the scene*/
		//00//float nearPlane = 0.1f, farPlane = 10.0f;
		//00//glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
		//00//glm::mat4 lightView = glm::lookAt(sunEye,
		//00//								  sunCenter,
		//00//								  glm::vec3(0.0f, 1.0f, 0.0f));
		//00//glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		//00//depthShader.bind();
		//00//depthShader.setUniformMatrix("lightSpaceMatrix", lightSpaceMatrix, false);
		//00//window.setViewPort(SHADOW_WIDTH, SHADOW_HEIGHT);
		//00//GLCall(glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOdir));
		//00//GLCall(glClear(GL_DEPTH_BUFFER_BIT));
		//00////glCullFace(GL_FRONT);
		//00//glDisable(GL_CULL_FACE);
		//00//cube.draw(1.0f, { 0.0f,0.25f,0.0f }, { 0.0f, cubeRotation, 0.0f }, depthShader);
		//00//hut.draw(1.0, glm::vec3{ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, depthShader);
		//00//sphere.draw(0.2, spherePosition, sphereRadians, depthShader);
		//00//glEnable(GL_CULL_FACE);
		//00////glCullFace(GL_BACK);
		//00//
		//00//GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		//00//depthShader.unbind();
		//00//
		//00//
		//00//

		/* omnidirectional: filling the cubemap */
		window.setViewPort(SHADOW_WIDTH, SHADOW_HEIGHT);
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOomni));
		GLCall(glClear(GL_DEPTH_BUFFER_BIT));
		cubeDepthShader.bind();
		// pass uniforms
		cubeDepthShader.setUniformValue("lightPos", specialPointLightPos);
		cubeDepthShader.setUniformValue("far_plane", far);
		for (size_t i = 0; i < shadowTransforms.size(); i++) {
			cubeDepthShader.setUniformMatrix("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i], false);
		}
		
		cube.draw(1.0f, { 0.0f,0.25f,0.0f }, { 0.0f, cubeRotation, 0.0f }, cubeDepthShader);
		hut.draw(1.0, glm::vec3{ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, cubeDepthShader);
		sphere.draw(0.2, spherePosition, sphereRadians, cubeDepthShader);
		
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		cubeDepthShader.unbind();
		/* end omnidirectional */







		/* render stuff and mechanics here*/
		window.setViewPort(window.getWidth(), window.getHeight());
		window.clearColorBufferBit(0.01f, 0.01f, 0.01f, 1.0f);

		// render Depth map to quad for visual debugging
		// ---------------------------------------------
		//debugDepth.bind();
		//debugDepth.setUniformValue("near_plane", nearPlane);
		//debugDepth.setUniformValue("far_plane", farPlane);
		//debugDepth.setUniformValue("depthMap", 3);
		//glActiveTexture(GL_TEXTURE0+3);
		//glBindTexture(GL_TEXTURE_2D, depthMap);
		//renderQuad();


		// renderCubeDepthMap as a skybox
		glDepthMask(GL_FALSE);
		debugCubeDepth.bind();
		// ... set view and projection matrix
		debugCubeDepth.setUniformMatrix("view", glm::mat4(glm::mat3(camera.getViewMatrix())), false);
		debugCubeDepth.setUniformMatrix("projection", projection, false);

		glBindVertexArray(skyboxVAO);
		
		debugCubeDepth.setUniformValue("skybox", 5);
		glActiveTexture(GL_TEXTURE0+5);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);


#if 0
		if(false)
		{	
			shader.bind();
			shader.setUniformValue("shadowMap", 3);
			glActiveTexture(GL_TEXTURE0 + 3);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			shader.setUniformMatrix("lightSpaceMatrix", lightSpaceMatrix, false);


			/* omni */
			shader.setUniformValue("cubeDepthMap", 4);
			glActiveTexture(GL_TEXTURE0 + 4);
			glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
			shader.setUniformValue("farPlane", far);
			/* end omni */


			/* pass uniforms to the currently bound shader*/
			shader.setUniformMatrix("projection", projection, false);
			shader.setUniformMatrix("view", camera.getViewMatrix(), false);
	
			shader.setUniformValue("flashLight.ambient", 0.1f * glm::vec3{ 1.0f, 1.0f, 1.0f });
			shader.setUniformValue("flashLight.diffuse", 0.8f * glm::vec3{ 1.0f, 1.0f, 1.0f });
			shader.setUniformValue("flashLight.specular", 1.0f * glm::vec3{ 1.0f, 1.0f, 1.0f });
			shader.setUniformValue("flashLight.position", lightPosition);
			shader.setUniformValue("flashLight.direction", glm::vec3{ -0.2f,0.2f,-1.0f });
			shader.setUniformValue("flashLight.cutoff", glm::cos(glm::radians(20.0f)));
			shader.setUniformValue("flashLight.outerCutoff", glm::cos(glm::radians(25.0f)));
			shader.setUniformValue("flashLight.linear", 0.01f);
			shader.setUniformValue("flashLight.quadratic", 0.3f);
			shader.setUniformValue("flashLight.constant", 0.1f);
	
			glm::vec3 sunDir = normalize(sunEye - sunCenter);
			float costheta = -0.5 * glm::dot(sunDir, { 0.0f, 1.0f, 0.0f });

			shader.setUniformValue("dirLight.ambient",  0.0f * 0.1f *  glm::vec3{ 1.0f + costheta, 0.5f, 0.5f });
			shader.setUniformValue("dirLight.diffuse",  0.0f * 0.8f *  glm::vec3{ 1.0f + costheta, 0.5f, 0.5f });
			shader.setUniformValue("dirLight.specular", 0.0f * 1.0f *  glm::vec3{ 1.0f + costheta, 0.5f, 0.5f });
			shader.setUniformValue("dirLight.direction", sunCenter - sunEye);
	
			for (size_t i = 0; i < pointLightPositions.size(); i++) {
				shader.setUniformValue("pointLights[" + std::to_string(i) + "].ambient", 0.1f * 0.1f * glm::vec3{ 1.0f, 1.0f, 1.0f });
				shader.setUniformValue("pointLights[" + std::to_string(i) + "].diffuse", 0.1f * 0.8f * glm::vec3{ 1.0f, 1.0f, 1.0f });
				shader.setUniformValue("pointLights[" + std::to_string(i) + "].specular", 0.1f * 1.0f * glm::vec3{ 1.0f, 1.0f, 1.0f });
				shader.setUniformValue("pointLights[" + std::to_string(i) + "].position", pointLightPositions.at(i));
				shader.setUniformValue("pointLights[" + std::to_string(i) + "].linear", 0.01f);
				shader.setUniformValue("pointLights[" + std::to_string(i) + "].quadratic", 0.10f);
				shader.setUniformValue("pointLights[" + std::to_string(i) + "].constant", 0.01f);
			}
	
			shader.setUniformValue("specialPointLight.ambient",  0.1f * 0.1f * glm::vec3{ 1.0f, 1.0f, 1.0f });
			shader.setUniformValue("specialPointLight.diffuse",  0.1f * 0.8f * glm::vec3{ 1.0f, 1.0f, 1.0f });
			shader.setUniformValue("specialPointLight.specular", 0.1f * 1.0f * glm::vec3{ 1.0f, 1.0f, 1.0f });
			shader.setUniformValue("specialPointLight.position", specialPointLightPos);
			shader.setUniformValue("specialPointLight.linear",    0.01f);
			shader.setUniformValue("specialPointLight.quadratic", 0.10f);
			shader.setUniformValue("specialPointLight.constant",  0.01f);

			shader.setUniformValue("cameraPos", camera.getEye().x, camera.getEye().y, camera.getEye().z);
	
			glm::mat4 model{ 1.0 };
			shader.setUniformValue("material.shininess", 256.0f);
			shader.setUniformMatrix("normalMat", glm::inverse(glm::transpose(model)), false);
			shader.setUniformMatrix("model", model, false);
			shader.unbind();
	
			// pass uniforms to lamp shader
			lampShader.bind();
			lampShader.setUniformMatrix("projection", projection, false);
			lampShader.setUniformMatrix("view", camera.getViewMatrix(), false);
			lampShader.unbind();
	
			cube.draw(1.0f, { 0.0f,0.25f,0.0f }, { 0.0f, cubeRotation,0.0f }, shader);
			hut.draw(1.0, glm::vec3{ 0.0f,0.0f,0.0f }, { 0.0f, 0.0f,0.0f }, shader);
			sphere.draw(0.2, spherePosition, glm::vec3{ sphereRadians.z, sphereRadians.y,-sphereRadians.x }, shader);
			
	
			// flashlight
			light.draw(0.25f, lightPosition, { 0.0f,0.0f,0.0f }, lampShader);
			// point lights
			for (unsigned int i = 0; i < pointLightPositions.size(); i++)
			{
				light.draw(0.25f, pointLightPositions.at(i), { 0.0f,0.0f,0.0f }, lampShader);
			}
			// special light
			light.draw(0.45f, specialPointLightPos, { 0.0f,0.0f,0.0f }, lampShader);

		}
#endif
		/* commands */
		pointLightPositions.at(0).z += 0.001;

		camera.processCommands(window);

		
		moveVector(window, sunEye, 1.0);
		
		//moveVector(window, spherePosition, 2.0 / (2.0 * 3.14159) );
		//moveVector(window, sphereRadians,  2.0 / (2.0 * 3.14159 / (360.0)));



		/* finalize frame */
		window.swapBuffers();
		window.pollEvents();
		window.updateLastFrameTime();
	}
	return 0;
}