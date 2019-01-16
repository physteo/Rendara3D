#shader vertex
#version 330 core
#pragma optionNV unroll all


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

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
#define NR_POINT_LIGHTS 2

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMat;
uniform FlashLight flashLight;
uniform vec3 cameraPos;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLight;
uniform PointLight specialPointLight;


out vec3 Normal;
out vec3 FragPos;
out vec3 FragPos_tan;

out vec2 TexCoords;
out vec3 cameraPos_tan;



uniform mat4 lightSpaceMatrix; // omni shadow
out vec4 FragPosLightSpace;    // omni shadow
out mat3 TBN; // omni shadow

out VS_OUT{
	FlashLight flashLight_tan;
	PointLight pointLights_tan[NR_POINT_LIGHTS];
	DirLight dirLight_tan;
	PointLight specialPointLight_tan;
} vs_out;

void main()
{
	gl_Position = projection * view * model *  vec4(aPos, 1.0f);
	FragPos = vec3(model * vec4(aPos, 1.0));
	TexCoords = aTexCoords; // no need to change to world coordinates... why?
	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0f);

	Normal = normalize(normalMat * vec4(aNormal, 0.0f)).xyz;//mat3(transpose(inverse(model))) * aNormal;////
	vec3 Tangent = normalize(normalMat * vec4(aTangent, 0.0f)).xyz;
	Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
	vec3 Bitangent = normalize(cross(Normal, Tangent));
	mat3 TBN = mat3(Tangent, Bitangent, Normal);

	mat3 iTBN = transpose(TBN);
	Normal = normalize(iTBN * Normal);
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

	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		vs_out.pointLights_tan[i].position = iTBN * pointLights[i].position;
		vs_out.pointLights_tan[i].constant = pointLights[i].constant;
		vs_out.pointLights_tan[i].linear = pointLights[i].linear;
		vs_out.pointLights_tan[i].quadratic = pointLights[i].quadratic;
		vs_out.pointLights_tan[i].ambient = pointLights[i].ambient;
		vs_out.pointLights_tan[i].diffuse = pointLights[i].diffuse;
		vs_out.pointLights_tan[i].specular = pointLights[i].specular;
	}

	vs_out.dirLight_tan.direction = normalize(iTBN * dirLight.direction);
	vs_out.dirLight_tan.ambient = dirLight.ambient;
	vs_out.dirLight_tan.diffuse = dirLight.diffuse;
	vs_out.dirLight_tan.specular = dirLight.specular;



	// special point light with shadows
	vs_out.specialPointLight_tan.position  = iTBN * specialPointLight.position;
	//vs_out.specialPointLight_world.position =  specialPointLight.position;
	vs_out.specialPointLight_tan.constant  = specialPointLight.constant;
	vs_out.specialPointLight_tan.linear    = specialPointLight.linear;
	vs_out.specialPointLight_tan.quadratic = specialPointLight.quadratic;
	vs_out.specialPointLight_tan.ambient   = specialPointLight.ambient;
	vs_out.specialPointLight_tan.diffuse   = specialPointLight.diffuse;
	vs_out.specialPointLight_tan.specular  = specialPointLight.specular;

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

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
#define NR_POINT_LIGHTS 2

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 color;

uniform Material material;
uniform sampler2D shadowMap; // shadows

in vec3 Normal;
in vec3 FragPos;
in vec3 FragPos_tan;

in vec2 TexCoords;
in vec3 cameraPos_tan;

in vec4 FragPosLightSpace; // shadows
uniform vec3 lightPos;      // shadows
//in mat3 TBN; // omni shadow


uniform float farPlane;  // omnidir shadows
uniform samplerCube cubeDepthMap; // omnidir shadows

in VS_OUT{
	FlashLight flashLight_tan;
	PointLight pointLights_tan[NR_POINT_LIGHTS];
	DirLight dirLight_tan;
	PointLight specialPointLight_tan;
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

float OmniShadowCalculation(vec3 lightPos, vec3 fragPos, float bias)
{
	vec3 fragToLight = fragPos - lightPos;
	float closestDepth = texture(cubeDepthMap, fragToLight).r;   // here the direction is in tan space.. TODO URGENT
	closestDepth *= farPlane;
	float currentDepth = length(fragToLight);
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	return shadow;
}

vec3 calc_pointlight_wshadow(PointLight light, vec3 FragPos, vec3 viewDir, vec3 norm, float bias)
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
	float shadow = OmniShadowCalculation(light.position, FragPos, bias);
	//shadow = 0.0;
	vec3 result = attenuation * (ambient + (1.0 - shadow) * (diffuse + specular)  );
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

float ShadowCalculation(vec4 FragPosLightSpace, float shadowBias)
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
		vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
		for (int x = -1; x <= 1; x++)
		{
			for (int y = -1; y <= 1; y++)
			{
				float temp = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
				shadow += currentDepth - shadowBias > temp ? 1.0 : 0.0;
			}
		}
		shadow /= 9.0;
		return shadow;
	}

}


vec3 calc_dirlight(DirLight light, vec3 viewDir, vec3 norm, vec4 FragPosLightSpace, float shadowBias)
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


	float shadow = ShadowCalculation(FragPosLightSpace, shadowBias);

	return (ambient + (1.0 - shadow) * diffuse + specular);

}




void main()
{
	//before normalMap //
	//vec3 norm = normalize(Normal);
	vec3 norm = texture(material.normal, TexCoords).rgb;
	norm = normalize(norm * 2.0 - 1.0);
	

	vec3 viewDir_tan = normalize(cameraPos_tan - FragPos_tan);


	vec3 result = vec3(0.0f, 0.0f, 0.0f);

	// flashlights
	result += calc_flashlight(fs_in.flashLight_tan, FragPos_tan, viewDir_tan, norm);

	// dirlight
	float shadowBias = max(0.05 * (1.0 - dot(norm, fs_in.dirLight_tan.direction)), 0.05);
	result += calc_dirlight(fs_in.dirLight_tan, viewDir_tan, norm, FragPosLightSpace, shadowBias);

	// pointLights
	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		result += calc_pointlight(fs_in.pointLights_tan[i], FragPos_tan, viewDir_tan, norm);
	}


	// omni shadow
	//vec3 norm_world = normalize(TBN * norm);
	////fs_in.specialPointLight_tan.position = TBN * fs_in.specialPointLight_tan.position;
	//vec3 viewDir_world = TBN * viewDir_tan;
	//result += calc_pointlight_wshadow(fs_in.specialPointLight_world, FragPos, viewDir_world, norm_world, 0.005);
	result += calc_pointlight_wshadow(fs_in.specialPointLight_tan, FragPos_tan, viewDir_tan, norm, 0.005);


	color = vec4(result, 1.0);

};

