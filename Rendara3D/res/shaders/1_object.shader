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
#define NR_POINT_LIGHTS 4

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


out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
out vec3 cameraPos_tan;

out VS_OUT{
	FlashLight flashLight_tan;
	PointLight pointLights_tan[NR_POINT_LIGHTS];
	DirLight dirLight_tan;
} vs_out;

void main()
{
	gl_Position = projection * view * model *  vec4(aPos, 1.0f);
	FragPos = vec3(model * vec4(aPos, 1.0));
	TexCoords = aTexCoords; // no need to change to world coordinates... why?

	Normal = normalize(normalMat * vec4(aNormal, 0.0f)).xyz;//mat3(transpose(inverse(model))) * aNormal;////
	vec3 Tangent = normalize(normalMat * vec4(aTangent, 0.0f)).xyz;
	Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
	vec3 Bitangent = normalize(cross(Normal, Tangent));
	mat3 TBN = mat3(Tangent, Bitangent, Normal); 

	mat3 iTBN = transpose(TBN);
	Normal = normalize(iTBN * Normal );
	FragPos = iTBN * FragPos;
	cameraPos_tan = iTBN * cameraPos;


	vs_out.flashLight_tan.position =  iTBN * flashLight.position;
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
		vs_out.pointLights_tan[i].position   = iTBN * pointLights[i].position;
		vs_out.pointLights_tan[i].constant   = pointLights[i].constant;
		vs_out.pointLights_tan[i].linear     = pointLights[i].linear;
		vs_out.pointLights_tan[i].quadratic  = pointLights[i].quadratic;
		vs_out.pointLights_tan[i].ambient	 = pointLights[i].ambient;
		vs_out.pointLights_tan[i].diffuse	 = pointLights[i].diffuse;
		vs_out.pointLights_tan[i].specular   = pointLights[i].specular;
	}

	vs_out.dirLight_tan.direction = normalize(iTBN * dirLight.direction);
	vs_out.dirLight_tan.ambient	  = dirLight.ambient;
	vs_out.dirLight_tan.diffuse	  = dirLight.diffuse;
	vs_out.dirLight_tan.specular  = dirLight.specular;

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
#define NR_POINT_LIGHTS 4

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 color;

uniform Material material;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in vec3 cameraPos_tan;


in VS_OUT{
	FlashLight flashLight_tan;
	PointLight pointLights_tan[NR_POINT_LIGHTS];
	DirLight dirLight_tan;
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

vec3 calc_flashlight(FlashLight light, vec3 FragPos, vec3 viewDir, vec3 norm )
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
	vec3 result = attenuation * ( ambient + intensity * (diffuse + specular));

	return result;
}

vec3 calc_dirlight(DirLight light, vec3 viewDir, vec3 norm)
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
	vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse  = light.diffuse  * (diff * vec3(texture(material.diffuse, TexCoords)));
	vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));

	return (ambient + diffuse + specular);

}




void main()
{
	//before normalMap //
	//vec3 norm = normalize(Normal);
	vec3 norm = texture(material.normal, TexCoords).rgb;
	norm = normalize(norm * 2.0 - 1.0);
	//norm = normalize(TBN * norm);

	vec3 viewDir = normalize(cameraPos_tan - FragPos);


	vec3 result = vec3(0.0f, 0.0f, 0.0f);
	
	// flashlights
	result += calc_flashlight(fs_in.flashLight_tan, FragPos, viewDir, norm);
	// dirlight
	result += calc_dirlight(fs_in.dirLight_tan, viewDir, norm);
	// pointLights
	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		result += calc_pointlight(fs_in.pointLights_tan[i], FragPos, viewDir, norm);
	}

	color = vec4(result, 1.0);

};



