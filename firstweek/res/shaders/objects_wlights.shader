#shader vertex
#version 330 core
#pragma optionNV unroll all

#define in1 -1
#define i0 0
#define i1 1
#define i2 2
#define i3 3
#define i4 4
#define i5 5
#define i6 6
#define i7 7
#define i8 8


#define NR_SUNS 1
#define NR_POINT_LIGHTS 1

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;



struct PointLight {
	vec3 position;
	vec3 position_world;
	float constant;
	float linear;
	float quadratic;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Sun {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMat;
uniform vec3 cameraPos;
out vec3	 cameraPos_world;
out vec3	 cameraPos_tan;

uniform PointLight pointLights[NR_POINT_LIGHTS];

// sun 
uniform Sun  sun[NR_SUNS];
uniform mat4 lightSpaceMatrix[NR_SUNS]; // shadow
out vec4 FragPosLightSpace[NR_SUNS];    // shadow

out vec3 FragPos;
out vec3 FragPos_tan;
out vec2 TexCoords;


out vec3  vs_out_pointLights_tan_position[NR_POINT_LIGHTS];
out vec3  vs_out_pointLights_tan_position_world[NR_POINT_LIGHTS];
out float vs_out_pointLights_tan_constant[NR_POINT_LIGHTS];
out float vs_out_pointLights_tan_linear[NR_POINT_LIGHTS];
out float vs_out_pointLights_tan_quadratic[NR_POINT_LIGHTS];
out vec3  vs_out_pointLights_tan_ambient[NR_POINT_LIGHTS];
out vec3  vs_out_pointLights_tan_diffuse[NR_POINT_LIGHTS];
out vec3  vs_out_pointLights_tan_specular[NR_POINT_LIGHTS];

out vec3 vs_out_sun_tan_direction[NR_SUNS];
out vec3 vs_out_sun_tan_ambient[NR_SUNS];
out vec3 vs_out_sun_tan_diffuse[NR_SUNS];
out vec3 vs_out_sun_tan_specular[NR_SUNS];

void main()
{
	gl_Position = projection * view * model *  vec4(aPos, 1.0f);
	FragPos = vec3(model * vec4(aPos, 1.0));
	TexCoords = aTexCoords; // no need to change to world coordinates... why?

	vec3 Normal = normalize(normalMat * vec4(aNormal, 0.0f)).xyz;//mat3(transpose(inverse(model))) * aNormal;////
	vec3 Tangent = normalize(normalMat * vec4(aTangent, 0.0f)).xyz;
	Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
	vec3 Bitangent = normalize(cross(Normal, Tangent));
	mat3 TBN = mat3(Tangent, Bitangent, Normal);

	mat3 iTBN = transpose(TBN);
	FragPos_tan = iTBN * FragPos;
	cameraPos_tan = iTBN * cameraPos;

	
	for (int i = 0; i < NR_SUNS; i++) {
		vs_out_sun_tan_direction[i] = normalize(iTBN * sun[i].direction);
		vs_out_sun_tan_ambient[i] =  sun[i].ambient;
		vs_out_sun_tan_diffuse[i] =  sun[i].diffuse;
		vs_out_sun_tan_specular[i] = sun[i].specular;
		FragPosLightSpace[i] = lightSpaceMatrix[i] * vec4(FragPos, 1.0f);
	}


	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		vs_out_pointLights_tan_position[i] = iTBN *pointLights[i].position;
		vs_out_pointLights_tan_position_world[i] = pointLights[i].position;
		vs_out_pointLights_tan_constant[i] =       pointLights[i].constant;
		vs_out_pointLights_tan_linear[i] =         pointLights[i].linear;
		vs_out_pointLights_tan_quadratic[i] =      pointLights[i].quadratic;
		vs_out_pointLights_tan_ambient[i] =        pointLights[i].ambient;
		vs_out_pointLights_tan_diffuse[i] =        pointLights[i].diffuse;
		vs_out_pointLights_tan_specular[i] =       pointLights[i].specular;
	}

};

#shader fragment
#version 330 core

#define in1 -1
#define i0 0
#define i1 1
#define i2 2
#define i3 3
#define i4 4
#define i5 5
#define i6 6
#define i7 7
#define i8 8

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	float shininess;
};


struct PointLight {
	vec3 position;
	vec3 position_world;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
#define NR_SUNS 1

#define NR_POINT_LIGHTS 1

struct Sun {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 color;

uniform Material material;
uniform sampler2D shadowMap[NR_SUNS]; // shadows

in vec3 FragPos;
in vec3 FragPos_tan;

in vec2 TexCoords;

in vec3 cameraPos_world;
in vec3 cameraPos_tan;

in vec4 FragPosLightSpace[NR_SUNS]; // shadows


uniform float farPlane;  // omnidir shadows
uniform samplerCube cubeDepthMap[NR_POINT_LIGHTS]; // omnidir shadows



vec3 calc_pointlight(PointLight light, vec3 FragPos, vec3 viewDir, vec3 norm)
{

	vec3 lightDir = normalize(light.position - FragPos);

	// diffuse
	float diff = max(dot(norm, lightDir), 0.0);
	// specular
	//vec3 reflectDir = normalize(reflect(-lightDir, norm));
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(halfwayDir, norm), 0.0), material.shininess);
	// all
	vec3 ambient = light.ambient  * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse = light.diffuse  * (diff * vec3(texture(material.diffuse, TexCoords)));
	vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));

	// compute attenuation
	float distance = length(light.position - FragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	// final result
	vec3 result = attenuation * (ambient + diffuse + specular);
	return result;
}


//vec3 sampleOffsetDirections[20] = vec3[]
//(
//	vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
//	vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
//	vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
//	vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
//	vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
//);
vec3 sampleOffsetDirections[9] = vec3[]
(
	vec3(0, 0, 0), 
	vec3(1, 1, 1), vec3(1, 1, -1), vec3(1, -1, 1), vec3(1, -1, -1),
	vec3(-1, 1, 1), vec3(-1, 1, -1), vec3(-1, -1, 1), vec3(-1, -1, -1)
);


float OmniShadowCalculation(vec3 lightPosOmni, vec3 fragPos, vec3 cameraPos_world, float bias, samplerCube cubeDepthMap)
{
	vec3 fragToLight = fragPos - lightPosOmni;
	float currentDepth = length(fragToLight);
	float shadow = 0.0f;
	int samples = 9;

	float viewDistance = length(cameraPos_world - fragPos);
	float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;
	for (int i = 0; i < samples; ++i)
	{
		if (i==i0) {
			float closestDepth = texture(cubeDepthMap, fragToLight + sampleOffsetDirections[i0] * diskRadius).r;
			closestDepth *= farPlane;   // Undo mapping [0;1]
			if (currentDepth - bias > closestDepth)
				shadow += 1.0;
		}
		if (i == i1) {
			float closestDepth = texture(cubeDepthMap, fragToLight + sampleOffsetDirections[i1] * diskRadius).r;
			closestDepth *= farPlane;   // Undo mapping [0;1]
			if (currentDepth - bias > closestDepth)
				shadow += 1.0;
		}
		if (i == i2) {
			float closestDepth = texture(cubeDepthMap, fragToLight + sampleOffsetDirections[i2] * diskRadius).r;
			closestDepth *= farPlane;   // Undo mapping [0;1]
			if (currentDepth - bias > closestDepth)
				shadow += 1.0;
		}
		if (i == i3) {
			float closestDepth = texture(cubeDepthMap, fragToLight + sampleOffsetDirections[i3] * diskRadius).r;
			closestDepth *= farPlane;   // Undo mapping [0;1]
			if (currentDepth - bias > closestDepth)
				shadow += 1.0;
		}
		if (i == i4) {
			float closestDepth = texture(cubeDepthMap, fragToLight + sampleOffsetDirections[i4] * diskRadius).r;
			closestDepth *= farPlane;   // Undo mapping [0;1]
			if (currentDepth - bias > closestDepth)
				shadow += 1.0;
		}
		if (i == i5) {
			float closestDepth = texture(cubeDepthMap, fragToLight + sampleOffsetDirections[i5] * diskRadius).r;
			closestDepth *= farPlane;   // Undo mapping [0;1]
			if (currentDepth - bias > closestDepth)
				shadow += 1.0;
		}
		if (i == i6) {
			float closestDepth = texture(cubeDepthMap, fragToLight + sampleOffsetDirections[i6] * diskRadius).r;
			closestDepth *= farPlane;   // Undo mapping [0;1]
			if (currentDepth - bias > closestDepth)
				shadow += 1.0;
		}
		if (i == i7) {
			float closestDepth = texture(cubeDepthMap, fragToLight + sampleOffsetDirections[i7] * diskRadius).r;
			closestDepth *= farPlane;   // Undo mapping [0;1]
			if (currentDepth - bias > closestDepth)
				shadow += 1.0;
		}
		if (i == i8) {
			float closestDepth = texture(cubeDepthMap, fragToLight + sampleOffsetDirections[i8] * diskRadius).r;
			closestDepth *= farPlane;   // Undo mapping [0;1]
			if (currentDepth - bias > closestDepth)
				shadow += 1.0;
		}
	}
	

	shadow /= float(samples);
	return shadow;
}

vec3 calc_pointlight_wshadow(PointLight light, vec3 FragPos_tan, vec3 viewDir, vec3 norm, float bias, vec3 FragPosWorld, vec3 cameraPos_world, samplerCube cubeDepthMap)
{

	vec3 lightDir = normalize(light.position - FragPos_tan);

	// diffuse
	float diff = max(dot(norm, lightDir), 0.0);
	// specular
	//vec3 reflectDir = normalize(reflect(-lightDir, norm));
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(halfwayDir, norm), 0.0), material.shininess);
	// all
	vec3 ambient = light.ambient  * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse = light.diffuse  * (diff * vec3(texture(material.diffuse, TexCoords)));
	vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));

	// compute attenuation
	float distance = length(light.position - FragPos_tan);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	// final result
	float shadow = OmniShadowCalculation(light.position_world, FragPosWorld, cameraPos_world, bias, cubeDepthMap);

	vec3 result = attenuation * (ambient + (1.0 - shadow) * (diffuse + specular));
	return result;
}


float ShadowCalculation(vec4 FragPosLightSpace, float shadowBias, sampler2D shadowTexture)
{
	// perform perspective divide
	vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	//float closestDepth = texture(shadowMap, projCoords.xy).r;
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	// check whether current frag pos is in shadow
	//float bias = 0.005;


	float shadow = 0.0;
	if (projCoords.z > 1.0) {
		return shadow;
	}
	else
	{
		vec2 texelSize = 1.0 / textureSize(shadowTexture, 0);
		for (int x = -1; x <= 1; x++)
		{
			for (int y = -1; y <= 1; y++)
			{
				if (x == in1 && y == in1) {
					float temp = texture(shadowTexture, projCoords.xy + vec2(in1, in1) * texelSize).r;
					shadow += currentDepth - shadowBias > temp ? 1.0 : 0.0;
				}
				if (x == in1 && y == i0) {
					float temp = texture(shadowTexture, projCoords.xy + vec2(in1, i0) * texelSize).r;
					shadow += currentDepth - shadowBias > temp ? 1.0 : 0.0;
				}
				if (x == in1 && y == i1) {
					float temp = texture(shadowTexture, projCoords.xy + vec2(in1, i1) * texelSize).r;
					shadow += currentDepth - shadowBias > temp ? 1.0 : 0.0;
				}
				if (x == i0 && y == in1) {
					float temp = texture(shadowTexture, projCoords.xy + vec2(i0, in1) * texelSize).r;
					shadow += currentDepth - shadowBias > temp ? 1.0 : 0.0;
				}
				if (x == i0 && y == i0) {
					float temp = texture(shadowTexture, projCoords.xy + vec2(i0, i0) * texelSize).r;
					shadow += currentDepth - shadowBias > temp ? 1.0 : 0.0;
				}
				if (x == i0 && y == i1) {
					float temp = texture(shadowTexture, projCoords.xy + vec2(i0, i1) * texelSize).r;
					shadow += currentDepth - shadowBias > temp ? 1.0 : 0.0;
				}
				if (x == i1 && y == in1) {
					float temp = texture(shadowTexture, projCoords.xy + vec2(i1, in1) * texelSize).r;
					shadow += currentDepth - shadowBias > temp ? 1.0 : 0.0;
				}
				if (x == i1 && y == i0) {
					float temp = texture(shadowTexture, projCoords.xy + vec2(i1, i0) * texelSize).r;
					shadow += currentDepth - shadowBias > temp ? 1.0 : 0.0;
				}
				if (x == i1 && y == i1) {
					float temp = texture(shadowTexture, projCoords.xy + vec2(i1, i1) * texelSize).r;
					shadow += currentDepth - shadowBias > temp ? 1.0 : 0.0;
				}

			}
		}
		shadow /= 9.0;
		return shadow;
	}

}


vec3 calc_dirlight(Sun light, vec3 viewDir, vec3 norm, vec4 FragPosLightSpace, float shadowBias, sampler2D shadowTexture)
{
	vec3 lightDir = normalize(-light.direction);

	// diffuse
	float diff = max(dot(norm, lightDir), 0.0);
	// specular
	//vec3 reflectDir = normalize(reflect(-lightDir, norm));
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);	
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(halfwayDir, norm), 0.0), material.shininess);
	// all
	vec3 ambient = light.ambient  * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse = light.diffuse  * (diff * vec3(texture(material.diffuse, TexCoords)));
	vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));


	float shadow = ShadowCalculation(FragPosLightSpace, shadowBias, shadowTexture);

	return (ambient + (1.0 - shadow) * (diffuse + specular) );

}




in vec3  vs_out_pointLights_tan_position[NR_POINT_LIGHTS];
in vec3  vs_out_pointLights_tan_position_world[NR_POINT_LIGHTS];
in float vs_out_pointLights_tan_constant[NR_POINT_LIGHTS];
in float vs_out_pointLights_tan_linear[NR_POINT_LIGHTS];
in float vs_out_pointLights_tan_quadratic[NR_POINT_LIGHTS];
in vec3  vs_out_pointLights_tan_ambient[NR_POINT_LIGHTS];
in vec3  vs_out_pointLights_tan_diffuse[NR_POINT_LIGHTS];
in vec3  vs_out_pointLights_tan_specular[NR_POINT_LIGHTS];

in vec3 vs_out_sun_tan_direction[NR_SUNS];
in vec3 vs_out_sun_tan_ambient[NR_SUNS];
in vec3 vs_out_sun_tan_diffuse[NR_SUNS];
in vec3 vs_out_sun_tan_specular[NR_SUNS];

void main()
{

	PointLight vs_out_pointLights_tan[NR_POINT_LIGHTS];
	for (int i = 0; i < 1; i++)
	{
		vs_out_pointLights_tan[i].position = vs_out_pointLights_tan_position[i];
		vs_out_pointLights_tan[i].position_world = vs_out_pointLights_tan_position_world[i];
		vs_out_pointLights_tan[i].constant = vs_out_pointLights_tan_constant[i];
		vs_out_pointLights_tan[i].linear = vs_out_pointLights_tan_linear[i];
		vs_out_pointLights_tan[i].quadratic = vs_out_pointLights_tan_quadratic[i];
		vs_out_pointLights_tan[i].ambient = vs_out_pointLights_tan_ambient[i];
		vs_out_pointLights_tan[i].diffuse = vs_out_pointLights_tan_diffuse[i];
		vs_out_pointLights_tan[i].specular = vs_out_pointLights_tan_specular[i];
	}

	Sun vs_out_sun_tan[NR_SUNS];
	vs_out_sun_tan[0].direction = vs_out_sun_tan_direction[0];
	vs_out_sun_tan[0].ambient   = vs_out_sun_tan_ambient[0];
	vs_out_sun_tan[0].diffuse   = vs_out_sun_tan_diffuse[0];
	vs_out_sun_tan[0].specular  = vs_out_sun_tan_specular[0];
	
	//before normalMap //
	vec3 norm = texture(material.normal, TexCoords).rgb;
	norm = normalize(norm * 2.0 - 1.0);
	
	vec3 viewDir_tan = normalize(cameraPos_tan - FragPos_tan);
	
	vec3 result = vec3(0.0f, 0.0f, 0.0f);
	
	// flashlights
	//result += calc_flashlight(fs_in.flashLight_tan, FragPos_tan, viewDir_tan, norm);
	
	// dirlight
	//for (int i = 0; i < NR_SUNS; i++)
	//{
		float shadowBias = max(0.002 * (1.0 - dot(norm, vs_out_sun_tan[i0].direction)), 0.002);
		result += calc_dirlight(vs_out_sun_tan[i0], viewDir_tan, norm, FragPosLightSpace[i0], shadowBias, shadowMap[i0]);
	//}
	
	
	//for (int i = 0; i < NR_POINT_LIGHTS; i++)
	//{
		float bias = 0.1;
		result += calc_pointlight_wshadow(vs_out_pointLights_tan[i0], FragPos_tan, viewDir_tan, norm, bias, FragPos, cameraPos_world, cubeDepthMap[i0]);
	//}

	color = vec4(result, 1.0);
};