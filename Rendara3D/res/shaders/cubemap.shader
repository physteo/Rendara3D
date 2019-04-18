#shader vertex
#version 330 core

layout (location = 0) in vec3 aPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 fragPos_mod;

void main()
{
	fragPos_mod = vec4(aPosition, 1.0f);

	gl_Position = projection * view * model * fragPos_mod;
};


#shader fragment
#version 330 core

in vec4 fragPos_mod;

uniform samplerCube cubeMap;

out vec4 color;

void main()
{
	// for visualizing depth
	float depth = texture(cubeMap, fragPos_mod.xyz).r;
	color = vec4(vec3(depth), 1.0f);
	//color = texture(cubeMap, fragPos_mod.xyz);

};

//#shader vertex
//#version 330 core
//layout(location = 0) in vec3 aPos;
//
//out vec3 TexCoords;
//
//uniform mat4 projection;
//uniform mat4 view;
//
//void main()
//{
//	TexCoords = aPos;
//	gl_Position = projection * view * vec4(aPos, 1.0);
//}
//
//#shader fragment
//#version 330 core
//out vec4 FragColor;
//
//in vec3 TexCoords;
//
//uniform samplerCube cubeMap;
//
//void main()
//{
//	FragColor = texture(cubeMap, TexCoords);
//	//float depthValue = texture(cubeMap, TexCoords).r;
//	//FragColor = vec4(vec3(depthValue), 1.0);
//}