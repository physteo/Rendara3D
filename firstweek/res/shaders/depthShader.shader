#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
	gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0f);
}

#shader fragment

void main()
{
	gl_FragDepth = gl_FragCoord.z;   // <- this happens automatically
	//gl_FragDepth -= gl_FrontFacing ? 0.01 : 0.0;
}