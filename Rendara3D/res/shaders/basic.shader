#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTextureCoords;

out vec3 ourPosition;
out vec3 ourColor;
out vec2 ourTextureCoords;
uniform float offset;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	ourPosition = aPosition;// +vec4(offset, 0.0f, 0.0f, 0.0f);
	//gl_Position = projection * view * model * vec4(aPosition, 1.0f, 1.0f);
	gl_Position = projection * view * model *  vec4(aPosition, 1.0f);
	ourColor = aColor;
	ourTextureCoords = aTextureCoords;
};


#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 ourColor;
in vec3 ourPosition;
in vec2 ourTextureCoords;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	//color = vec4(ourColor, 1.0);
	color = mix(texture(texture1, ourTextureCoords), texture(texture2, ourTextureCoords), 0.2f);
};
