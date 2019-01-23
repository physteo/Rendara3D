#shader vertex
#version 330 core
#pragma optionNV unroll all

#define NR_SUNS 1
#define NR_POINT_LIGHTS 1

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;

struct FlashLight {
	vec3 position;
	vec3 direction;
	float cutoff;
	float outerCutoff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
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

uniform FlashLight flashLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];

// sun 
uniform Sun  sun[NR_SUNS];
uniform mat4 lightSpaceMatrix[NR_SUNS]; // shadow
out vec4 FragPosLightSpace[NR_SUNS];    // shadow

out vec3 FragPos;
out vec3 FragPos_tan;

out vec2 TexCoords;


out VS_OUT{
	FlashLight flashLight_tan;
	PointLight pointLights_tan[NR_POINT_LIGHTS];
	Sun sun_tan[NR_SUNS];
} vs_out;

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


	vs_out.flashLight_tan.position = iTBN * flashLight.position;
	vs_out.flashLight_tan.direction = normalize(iTBN * flashLight.direction);
	vs_out.flashLight_tan.cutoff = flashLight.cutoff;
	vs_out.flashLight_tan.outerCutoff = flashLight.outerCutoff;
	vs_out.flashLight_tan.constant = flashLight.constant;
	vs_out.flashLight_tan.linear = flashLight.linear;
	vs_out.flashLight_tan.quadratic = flashLight.quadratic;
	vs_out.flashLight_tan.ambient = flashLight.ambient;
	vs_out.flashLight_tan.diffuse = flashLight.diffuse;
	vs_out.flashLight_tan.specular = flashLight.specular;


	for (int i = 0; i < NR_SUNS; i++) {
		vs_out.sun_tan[i].direction = normalize(iTBN * sun[i].direction);
		vs_out.sun_tan[i].ambient  = sun[i].ambient;
		vs_out.sun_tan[i].diffuse  = sun[i].diffuse;
		vs_out.sun_tan[i].specular = sun[i].specular;
		FragPosLightSpace[i] = lightSpaceMatrix[i] * vec4(FragPos, 1.0f);

	}


	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		vs_out.pointLights_tan[i].position = iTBN * pointLights[i].position;
		vs_out.pointLights_tan[i].position_world = pointLights[i].position;

		vs_out.pointLights_tan[i].constant = pointLights[i].constant;
		vs_out.pointLights_tan[i].linear = pointLights[i].linear;
		vs_out.pointLights_tan[i].quadratic = pointLights[i].quadratic;
		vs_out.pointLights_tan[i].ambient = pointLights[i].ambient;
		vs_out.pointLights_tan[i].diffuse = pointLights[i].diffuse;
		vs_out.pointLights_tan[i].specular = pointLights[i].specular;
	}




};







#shader fragment
#version 330 core

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	float shininess;
};

struct FlashLight {
	vec3 position;
	vec3 direction;
	float cutoff;
	float outerCutoff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
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

in VS_OUT{
	FlashLight flashLight_tan;
	PointLight pointLights_tan[NR_POINT_LIGHTS];
	Sun sun_tan[NR_SUNS];
} fs_in;






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
		float closestDepth = texture(cubeDepthMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= farPlane;   // Undo mapping [0;1]
		if (currentDepth - bias > closestDepth)
			shadow += 1.0;
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

vec3 calc_flashlight(FlashLight light, vec3 FragPos, vec3 viewDir, vec3 norm)
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

	// compute intensity 
	float theta = dot(-lightDir, normalize(light.direction));
	float epsilon = light.cutoff - light.outerCutoff;
	float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
	// compute attenuation
	float distance = length(light.position - FragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	// final result
	vec3 result = attenuation * (ambient + intensity * (diffuse + specular));

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
				float temp = texture(shadowTexture, projCoords.xy + vec2(x, y) * texelSize).r;
				shadow += currentDepth - shadowBias > temp ? 1.0 : 0.0;
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










void main()
{
	//before normalMap //
	vec3 norm = texture(material.normal, TexCoords).rgb;
	norm = normalize(norm * 2.0 - 1.0);
	
	vec3 viewDir_tan = normalize(cameraPos_tan - FragPos_tan);
	
	vec3 result = vec3(0.0f, 0.0f, 0.0f);
	
	// flashlights
	//result += calc_flashlight(fs_in.flashLight_tan, FragPos_tan, viewDir_tan, norm);
	
	// dirlight
	for (int i = 0; i < NR_SUNS; i++)
	{
		float shadowBias = max(0.002 * (1.0 - dot(norm, fs_in.sun_tan[i].direction)), 0.002);
		result += calc_dirlight(fs_in.sun_tan[i], viewDir_tan, norm, FragPosLightSpace[i], shadowBias, shadowMap[i]);
	}
	
	
	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		float bias = 0.1;
		result += calc_pointlight_wshadow(fs_in.pointLights_tan[i], FragPos_tan, viewDir_tan, norm, bias, FragPos, cameraPos_world, cubeDepthMap[i]);
	}

	color = vec4(result, 1.0);
	//// Tone mapping
	//const float exposure = 0.5;	
	//const float gamma = 2.2;
	//
	//// Exposure tone mapping
	//vec3 mapped = vec3(1.0) - exp(-result * exposure);
	//
	//
	//// Gamma correction 
	//mapped = pow(mapped, vec3(1.0 / gamma));
	//color = vec4(mapped, 1.0);

};

