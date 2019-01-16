#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMat;
uniform vec3 lightPos;

out vec3 Normal;
out vec3 FragPos;
out vec3 LightPos;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	//FragPos = (model * vec4(aPos, 1.0)).xyz;
	//Normal = normalize((normalMat * vec4(aNormal,0.0f)).xyz);

	FragPos = vec3(view * model * vec4(aPos, 1.0));
	Normal = normalize( vec3(normalMat *  vec4(aNormal,0.0f)) );
	LightPos = (view * vec4(lightPos, 1.0)).xyz;
}

#shader fragment
#version 330 core

uniform vec3 cameraPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

in vec3 LightPos;
in vec3 Normal;
in vec3 FragPos;
out vec4 color;

void main()
{
	// ambient

	// diffuse

	// specular
	float specStrength = 0.5;
	//vec3 lightFromFrag = normalize(lightPos - FragPos);
	//vec3 reflectedLight = reflect(-lightFromFrag, Normal);
	//vec3 cameraFromFrag = normalize(cameraPos - FragPos);
	//float coeffSpec = pow(max(dot(reflectedLight, cameraFromFrag), 0.0), 1024);

	vec3 lightFromFrag = normalize( LightPos - FragPos);
	vec3 reflectedLight = reflect(-lightFromFrag, Normal);
	vec3 cameraFromFrag = normalize(-FragPos);
	float coeffSpec = pow(max(dot(reflectedLight, cameraFromFrag), 0.0), 1024);


	vec3 spec = specStrength * coeffSpec * lightColor;
	
	vec3 result = spec * objectColor;
	color = vec4(result, 1.0);

}