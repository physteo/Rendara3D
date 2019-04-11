#include "PointLight.h"


void PointLight::cast(const std::string& uniformName, Shader& shader)
{
	shader.setUniformValue(uniformName+".ambient", ambientColor);
	shader.setUniformValue(uniformName+".diffuse", diffuseColor);
	shader.setUniformValue(uniformName+".specular",specularColor);
	shader.setUniformValue(uniformName+".position", eye);
	shader.setUniformValue(uniformName+".constant", attenuation.constant);
	shader.setUniformValue(uniformName+".linear", attenuation.linear);
	shader.setUniformValue(uniformName+".quadratic", attenuation.quadratic);
}