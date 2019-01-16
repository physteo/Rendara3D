#include "Chapter2_lighting.h"


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

std::vector<float> normals = {
	  0.0f,  0.0f, -1.0f,
	  0.0f,  0.0f, -1.0f,
	  0.0f,  0.0f, -1.0f,
	  0.0f,  0.0f, -1.0f,
	  0.0f,  0.0f, -1.0f,
	  0.0f,  0.0f, -1.0f,

	  0.0f,  0.0f, 1.0f,
	  0.0f,  0.0f, 1.0f,
	  0.0f,  0.0f, 1.0f,
	  0.0f,  0.0f, 1.0f,
	  0.0f,  0.0f, 1.0f,
	  0.0f,  0.0f, 1.0f,

	 -1.0f,  0.0f,  0.0f,
	 -1.0f,  0.0f,  0.0f,
	 -1.0f,  0.0f,  0.0f,
	 -1.0f,  0.0f,  0.0f,
	 -1.0f,  0.0f,  0.0f,
	 -1.0f,  0.0f,  0.0f,

	  1.0f,  0.0f,  0.0f,
	  1.0f,  0.0f,  0.0f,
	  1.0f,  0.0f,  0.0f,
	  1.0f,  0.0f,  0.0f,
	  1.0f,  0.0f,  0.0f,
	  1.0f,  0.0f,  0.0f,

	  0.0f, -1.0f,  0.0f,
	  0.0f, -1.0f,  0.0f,
	  0.0f, -1.0f,  0.0f,
	  0.0f, -1.0f,  0.0f,
	  0.0f, -1.0f,  0.0f,
	  0.0f, -1.0f,  0.0f,

	  0.0f,  1.0f,  0.0f,
	  0.0f,  1.0f,  0.0f,
	  0.0f,  1.0f,  0.0f,
	  0.0f,  1.0f,  0.0f,
	  0.0f,  1.0f,  0.0f,
	  0.0f,  1.0f,  0.0f
};

std::vector<float> texCoords = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,

	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,

	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,

	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f
};

std::vector<float> vertices = {
	   -0.5f, -0.5f, -0.5f, // 0 0 -1
		0.5f, -0.5f, -0.5f,	// 0 0 -1
		0.5f,  0.5f, -0.5f,	// 0 0 -1
		0.5f,  0.5f, -0.5f,	// 0 0 -1
	   -0.5f,  0.5f, -0.5f,	// 0 0 -1
	   -0.5f, -0.5f, -0.5f,	// 0 0 -1

	   -0.5f, -0.5f,  0.5f,	 // 0 0 1
		0.5f, -0.5f,  0.5f,	 // 0 0 1
		0.5f,  0.5f,  0.5f,	 // 0 0 1
		0.5f,  0.5f,  0.5f,	 // 0 0 1
	   -0.5f,  0.5f,  0.5f,	 // 0 0 1
	   -0.5f, -0.5f,  0.5f,  // 0 0 1

	   -0.5f,  0.5f,  0.5f,	 // -1 0 0
	   -0.5f,  0.5f, -0.5f,	 // -1 0 0
	   -0.5f, -0.5f, -0.5f,	 // -1 0 0
	   -0.5f, -0.5f, -0.5f,	 // -1 0 0
	   -0.5f, -0.5f,  0.5f,	 // -1 0 0
	   -0.5f,  0.5f,  0.5f,  // -1 0 0

		0.5f,  0.5f,  0.5f,	// 1 0 0
		0.5f,  0.5f, -0.5f,	// 1 0 0
		0.5f, -0.5f, -0.5f,	// 1 0 0
		0.5f, -0.5f, -0.5f,	// 1 0 0
		0.5f, -0.5f,  0.5f,	// 1 0 0
		0.5f,  0.5f,  0.5f, // 1 0 0

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

std::vector<glm::vec3> cubePositions = {
	glm::vec3(3.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};

std::vector<glm::vec3> pointLightPositions = {
	glm::vec3(1.0f,  1.0f, -3.0f),
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f)
};


#include "../Mesh.h"
#include "../Model.h"

int chapter2()
{

	Window window("Lighting", 1400, 1400, Monitor::G_NOTSPECIFIED);

	/* shaders */
	Shader shader("./res/shaders/1_object.shader");
	Shader lampShader("./res/shaders/1_lamp.shader");
	Shader outlineShader("./res/shaders/4_stencil_outline.shader");

	/* objects */
	std::vector<Vertex> allAttributes;
	allAttributes.resize(36);
	std::vector<unsigned int> indices;

	size_t j = 0;
	for (size_t i = 0; i < 36; i+=3, j+=2)
	{
		Vertex v;
		allAttributes.at(i).position = { vertices.at(i), vertices.at(i+1), vertices.at(i+2) };
		allAttributes.at(i).normal = { normals.at(i),  normals.at(i+1), normals.at(i+2) };	
	}

	for (size_t i = 0; i < 36; i += 2)
	{
		allAttributes.at(i).texCoords = { texCoords.at(i), texCoords.at(i+1) };
	}

	for (size_t i = 0; i < 36; i++)
	{
		indices.push_back(i);
	}


	/* textures */
	Texture::set2DTextureParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	Texture::set2DTextureParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	Texture::set2DTextureParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	Texture::set2DTextureParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	Texture specularMap;
	specularMap.generate("./res/textures/container2_specular.png", Format::RGBA);
	Texture diffuseMap;
	diffuseMap.generate("./res/textures/brickwall.jpg", Format::sRGBA);
	Texture textureNormal;
	textureNormal.generate("./res/textures/brickwall_normal.jpg", Format::RGBA);

	Material material;
	material.fill(diffuseMap, specularMap, textureNormal, 256.0f);
	//Mesh mesh;
	//mesh.fill(vertices, normals, texCoords, indices, material);
	

	VertexArray object;
	object.generate();
	object.bind();
	object.fillData({ vertices, normals, texCoords }, { 3, 3, 2 });
	object.unbind();

	// models
	std::vector<Texture> loadedTextures;
	//Model nanosuit{ "./res/model/nanosuit/nanosuit.obj", &loadedTextures };
	//Model earth{ "./res/model/earth/earth.obj", &loadedTextures };
	//Model flat{ "./res/model/surface/flat.obj", &loadedTextures };
	Model cube{ "./res/model/monkey/cube.obj", &loadedTextures };
	Model floor{ "./res/model/hut/hut.obj", &loadedTextures };

	
	VertexArray lamp;
	lamp.generate();
	lamp.bind();
	lamp.fillData({ vertices }, { 3 });
	lamp.unbind();

	//glm::vec3 lightPosition = glm::vec3{ 3.0f, 3.0f, 3.0f };
	glm::vec3 lightPosition = { 0.05000162,0.800001025,0.95000362 };
	
	/* camera */ 
	Camera camera({ -1.54360628, -0.855141640, 2.99689507 }, { 0.0f, 0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f });
	
	/* projection matrix */
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)(window.getWidth() / window.getHeight()), 0.1f, 100.0f);

	double zrot = 0.0f;


	while (!window.isClosed())
	{
		window.updateTime();
		window.clearColorBufferBit(0.01f, 0.01f, 0.01f, 1.0f);

		//lightPosition.x = 4.0f * sin(2.0*window.getCurrentTime());
		//lightPosition.z = 4.0f * cos(2.0*window.getCurrentTime());
		//lightPosition.y = 4.0f * cos(2.0*window.getCurrentTime());

		/* render stuff and mechanics here*/
		shader.bind();
		/* pass uniforms to the currently bound shader*/
		shader.setUniformMatrix("projection", projection, false);
		shader.setUniformMatrix("view", camera.getViewMatrix(), false);

		shader.setUniformValue("flashLight.ambient",  0.1f * glm::vec3{ 1.0f, 1.0f, 1.0f });
		shader.setUniformValue("flashLight.diffuse",  0.8f * glm::vec3{ 1.0f, 1.0f, 1.0f });
		shader.setUniformValue("flashLight.specular", 1.0f * glm::vec3{ 1.0f, 1.0f, 1.0f });
		shader.setUniformValue("flashLight.position", lightPosition);
		shader.setUniformValue("flashLight.direction", glm::vec3{ -0.2f,0.2f,-1.0f });
		shader.setUniformValue("flashLight.cutoff", glm::cos(glm::radians(20.0f)));
		shader.setUniformValue("flashLight.outerCutoff", glm::cos(glm::radians(25.0f)));
		shader.setUniformValue("flashLight.linear", 0.01f);
		shader.setUniformValue("flashLight.quadratic", 0.3f);
		shader.setUniformValue("flashLight.constant", 0.1f);

		shader.setUniformValue("dirLight.ambient",  1.0f * 0.1f * glm::vec3{ 1.0f, 1.0f, 1.0f });
		shader.setUniformValue("dirLight.diffuse",  1.0f * 0.8f * glm::vec3{ 1.0f, 1.0f, 1.0f });
		shader.setUniformValue("dirLight.specular", 1.0f * 1.0f * glm::vec3{ 1.0f, 1.0f, 1.0f });
		shader.setUniformValue("dirLight.direction", glm::vec3{ -1.0f,-1.0f,-1.0f });


		for (size_t i = 0; i < pointLightPositions.size(); i++) {
			shader.setUniformValue("pointLights[" + std::to_string(i) + "].ambient",   0.1f * 0.1f * glm::vec3{ 1.0f, 1.0f, 1.0f });
			shader.setUniformValue("pointLights[" + std::to_string(i) + "].diffuse",   0.1f * 0.8f * glm::vec3{ 1.0f, 1.0f, 1.0f });
			shader.setUniformValue("pointLights[" + std::to_string(i) + "].specular",  0.1f * 1.0f * glm::vec3{ 1.0f, 1.0f, 1.0f });
			shader.setUniformValue("pointLights[" + std::to_string(i) + "].position",  pointLightPositions.at(i));
			shader.setUniformValue("pointLights[" + std::to_string(i) + "].linear",    0.01f);
			shader.setUniformValue("pointLights[" + std::to_string(i) + "].quadratic", 0.10f);
			shader.setUniformValue("pointLights[" + std::to_string(i) + "].constant",  0.01f);
		}


		shader.setUniformValue("cameraPos", camera.getEye().x, camera.getEye().y, camera.getEye().z);

		// about the single object
		//shader.setUniformValue("material.shininess", 256.0f);
		//shader.setUniformValue("material.diffuse", 0);
		//shader.setUniformValue("material.specular", 1);
		//
		//diffuseMap.setActiveTexture(GL_TEXTURE0);
		//diffuseMap.bind();
		//specularMap.setActiveTexture(GL_TEXTURE1);
		//specularMap.bind();
		//
		//object.bind();
		//
		//for (unsigned int i = 0; i < cubePositions.size(); i++)
		//{
		//	glm::mat4 model{ 1.0 };
		//	model = glm::translate(model, cubePositions[i]);
		//	float angle = 20.0f * i;
		//	model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		//	shader.setUniformMatrix("normalMat", glm::inverse(glm::transpose(model)), false);
		//	shader.setUniformMatrix("model", model, false);
		//
		//	glDrawArrays(GL_TRIANGLES, 0, 36);
		//}
		//
		//object.unbind();
		//
		//
		//
		//specularMap.unbind();
		//diffuseMap.unbind();
		
		// test new mesh

		glm::mat4 model{ 1.0 };
		shader.setUniformValue("material.shininess", 256.0f);
		shader.setUniformMatrix("normalMat", glm::inverse(glm::transpose(model)), false);
		shader.setUniformMatrix("model", model, false);

		// Draw object normally, but writing to the stencil buffer
		//mesh.draw(1.0f, glm::vec3{ 0.0f,0.0f,0.0f }, glm::vec3{ 0.0f,0.0f,0.0f }, shader);
		// Draw object scaled up, disabling stencil writing.


		// end of stencil
		// draw floor as normal, but dont alter the stencil buffer
		//nanosuit.draw(0.1f, glm::vec3{ 1.0f,0.0f,0.0f }, glm::vec3{ 45.0f, 90.0f,0.0f }, shader);
		//earth.draw(1.0f, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, shader);

		//shader.setUniformValue("material.normalMap", 2);
		//Texture::setActiveTexture(GL_TEXTURE2);
		//textureNormal.bind();
		//flat.draw(1.0f, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, shader);
		cube.draw(1.0f, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,zrot }, shader);
		//cube.draw(1.0f, { 0.7f, -1.0f, 2.0f }, { 0.0f,0.0f, zrot += 10.0f * window.getLastFrameTime() }, shader);

		float floorScale = 1.0f;
		floor.draw(floorScale, 2.0f * floorScale * glm::vec3{ 0.0f,0.0f,0.0f }, { 0.0f,90.0f,0.0f }, shader);
		//floor.draw(floorScale, 2.0f * floorScale * glm::vec3{ 0.0f,0.0f,1.0f }, { 0.0f,90.0f,0.0f }, shader);
		//floor.draw(floorScale, 2.0f * floorScale * glm::vec3{ 1.0f,0.0f,1.0f }, { 0.0f,90.0f,0.0f }, shader);
		//floor.draw(floorScale, 2.0f * floorScale * glm::vec3{ 1.0f,0.0f,0.0f }, { 0.0f,90.0f,0.0f }, shader);

		//textureNormal.unbind();
		shader.unbind();

		/* render the lamp */
		lampShader.bind();
		lampShader.setUniformMatrix("projection", projection, false);
		lampShader.setUniformMatrix("view", camera.getViewMatrix(), false);

		lamp.bind();
		// flash light
		glm::mat4 modelLamp = glm::scale(glm::translate(glm::mat4{ 1.0 }, lightPosition), glm::vec3{ 0.2 });
		lampShader.setUniformMatrix("model", modelLamp, false);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// point lights
		for (unsigned int i = 0; i < pointLightPositions.size(); i++)
		{
			glm::mat4 modelLamp = glm::scale(glm::translate(glm::mat4{ 1.0 }, pointLightPositions.at(i)), glm::vec3{ 0.2 });
			lampShader.setUniformMatrix("model", modelLamp, false);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		lamp.unbind();
		
		lampShader.unbind();

		/* commands */
		pointLightPositions.at(0).z += 0.001;

		camera.processCommands(window);
		
		float lightSpeed = 1.0f;
		if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_UP) == GLFW_PRESS)
			lightPosition.x += lightSpeed * window.getLastFrameTime();
		if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_DOWN) == GLFW_PRESS)
			lightPosition.x -= lightSpeed * window.getLastFrameTime();

		if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_RIGHT) == GLFW_PRESS)
			lightPosition.y += lightSpeed * window.getLastFrameTime();
		if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_LEFT) == GLFW_PRESS)
			lightPosition.y -= lightSpeed * window.getLastFrameTime();

		if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_Z) == GLFW_PRESS)
			lightPosition.z += lightSpeed * window.getLastFrameTime();
		if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_X) == GLFW_PRESS)
			lightPosition.z -= lightSpeed * window.getLastFrameTime();


		/* finalize frame */
		window.swapBuffers();
		window.pollEvents();
		window.updateLastFrameTime();
	}
	return 0;
}