
#include "SunLight.h"

void SunLight::cast(const std::string& uniformName, Shader& shader)
{
	//shader.bind();
	shader.setUniformValue(uniformName + ".direction", center - eye);   //direction;
	shader.setUniformValue(uniformName + ".ambient",  ambientColor);//0.1f * glm::vec3{ 1.0f, 1.0f, 1.0f });
	shader.setUniformValue(uniformName + ".diffuse",  diffuseColor);//0.8f * glm::vec3{ 1.0f, 1.0f, 1.0f });
	shader.setUniformValue(uniformName + ".specular", specularColor);//1.0f * glm::vec3{ 1.0f, 1.0f, 1.0f });
	//shader.unbind();
}


