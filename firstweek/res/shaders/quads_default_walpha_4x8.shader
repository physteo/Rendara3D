#shader vertex
#version 330 core
#pragma optionNV unroll all

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;

void main()
{
	gl_Position = projection * view * model *  vec4(aPos, 1.0f);
	TexCoords = aTexCoords; // no need to change to world coordinates... why?
};


#shader fragment
#version 330 core

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	float shininess;
};

uniform Material material;
out vec4 color;
in vec2 TexCoords;

uniform float brightness;
uniform int x;
uniform int y;

void main()
{
	vec4 result = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	float x_step = 0.25f;
	float y_step = 0.125;
	result += texture(material.diffuse, TexCoords + vec2(x * x_step, y * y_step));

	if (result.a == 0.0)
		discard;

	color = vec4(brightness * result);
};

