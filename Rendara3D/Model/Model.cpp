#include "Model.h"

void Model::draw(const glm::vec3& scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader) const
{
	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].draw(scale, position, radians, shader);
	}
}

void Model::draw(float scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader) const
{
	this->draw(glm::vec3{ scale }, position, radians, shader);
}


void Model::draw(const glm::vec3& scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader, Material& material) const
{
	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].draw(scale, position, radians, shader, material);
	}
}

void Model::draw(float scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader, Material& material) const
{
	this->draw(glm::vec3{ scale }, position, radians, shader, material);
}


void Model::loadModel(const std::string path, std::map<std::string, Texture>* loadedTextures)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "[Assimp error] :" << importer.GetErrorString() << std::endl;
		return;
	}

	//m_directory = path.substr(0, path.find_last_of('/'));
	

	processNode(scene->mRootNode, scene, loadedTextures);
}

void Model::processNode(const aiNode* node, const aiScene* scene, std::map<std::string, Texture>* loadedTextures)
{
	for (size_t i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(processMesh(aimesh, scene, loadedTextures));
	}

	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, loadedTextures);
	}
}

Mesh Model::processMesh(const aiMesh* aimesh, const aiScene* scene, std::map<std::string, Texture>* loadedTextures)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	Texture* diffuse = nullptr;
	Texture* specular = nullptr;
	Texture* normal = nullptr;
	float shininess; 

	/* retrieve vertices */
	for (size_t i = 0; i < aimesh->mNumVertices; i++)
	{
		Vertex vertex;

		vertex.position = { aimesh->mVertices[i].x, aimesh->mVertices[i].y, aimesh->mVertices[i].z };
		vertex.normal   = { aimesh->mNormals[i].x,   aimesh->mNormals[i].y,  aimesh->mNormals[i].z };

		if (aimesh->mTextureCoords[0]) // TODO: add to documentation that we process only the first texture coordinates (assimp allows for multiple)
		{
			vertex.texCoords = { aimesh->mTextureCoords[0][i].x,   aimesh->mTextureCoords[0][i].y };
			vertex.tangent = { aimesh->mTangents[i].x, aimesh->mTangents[i].y, aimesh->mTangents[i].z };
		}
		else {
			vertex.texCoords = glm::vec2(0.0f, 0.0f);
			vertex.tangent = { 1.0, 0.0, 0.0 };

			std::cerr << "The mesh does not have UV coordinates: cannot assign proper tangent vectors." << std::endl;
			assert(false);
		}


		vertices.push_back(vertex);
	}


	/* retrieve indices */
	for (size_t i = 0; i < aimesh->mNumFaces; i++)
	{
		aiFace face = aimesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	/* retrive material */
	if (aimesh->mMaterialIndex >= 0)
	{
		aiMaterial* aimaterial = scene->mMaterials[aimesh->mMaterialIndex];

		// Check what kinds of texture that are present
		for (int i = 0; i < aiTextureType::aiTextureType_UNKNOWN; i++) {
			enum aiTextureType type = (enum aiTextureType) i;
			if (aimaterial->GetTextureCount(type)) {
				aiString texturePath;
				aimaterial->GetTexture(type, 0, &texturePath);
				std::cout << "Texture type: " << type << ", path: " << texturePath.C_Str() << std::endl;
			}
		}

		diffuse  = loadTextureFromMaterial(aimaterial, aiTextureType_DIFFUSE,  Format::sRGBA, loadedTextures);
		specular = loadTextureFromMaterial(aimaterial, aiTextureType_SPECULAR, Format::RGBA, loadedTextures);
		normal   = loadTextureFromMaterial(aimaterial, aiTextureType_HEIGHT,   Format::RGBA, loadedTextures);

		if (diffuse == nullptr)
		{
			int width = 1;
			int height = 1;
			int nrChannels = 3;
			unsigned char* color = (unsigned char*) alloca(nrChannels);
			color[0] = 255 * m_defaultColor[0];
			color[1] = 255 * m_defaultColor[1];
			color[2] = 255 * m_defaultColor[2];
			std::string name_r = std::to_string((int)(255 * m_defaultColor[0]));
			std::string name_g = std::to_string((int)(255 * m_defaultColor[1]));
			std::string name_b = std::to_string((int)(255 * m_defaultColor[2]));
			std::string newTextureName = "default-RGB=" + name_r + "-" + name_g + "-" + name_b;
			auto alreadyLoaded = loadedTextures->find(newTextureName);
			if (alreadyLoaded == loadedTextures->end())
			{
				Texture newTexture{ width, height,nrChannels, newTextureName, Format::sRGBA, color };
				alreadyLoaded = loadedTextures->insert({ newTextureName, std::move(newTexture) }).first;
			}
			diffuse = &(alreadyLoaded->second);
		}

		if (specular == nullptr)
		{
			int width = 1;
			int height = 1;
			int nrChannels = 3;
			unsigned char* color = (unsigned char*)alloca(nrChannels);
			color[0] = 0;
			color[1] = 0;
			color[2] = 0;
			std::string newTextureName = "default-specularmap";
			auto alreadyLoaded = loadedTextures->find(newTextureName);
			if (alreadyLoaded == loadedTextures->end())
			{
				Texture newTexture{ width, height,nrChannels, newTextureName, Format::RGBA, color };
				alreadyLoaded = loadedTextures->insert({ newTextureName, std::move(newTexture) }).first;
			}
			specular = &(alreadyLoaded->second);
		}

		if (normal == nullptr)
		{
			int width = 1;
			int height = 1;
			int nrChannels = 3;
			unsigned char* color = (unsigned char*)alloca(nrChannels);
			color[0] = 133;
			color[1] = 133;
			color[2] = 255; // TODO: this does not work! You can see the bug by drawing (e.g.) a plane without a normal map just below a point light: only 
							// half of the plane gets correct lighting
			std::string newTextureName = "default-normalmap";
			auto alreadyLoaded = loadedTextures->find(newTextureName);
			if (alreadyLoaded == loadedTextures->end())
			{
				Texture newTexture{ width, height,nrChannels, newTextureName, Format::RGBA, color };
				alreadyLoaded = loadedTextures->insert({ newTextureName, std::move(newTexture) }).first;
			}
			normal = &(alreadyLoaded->second);
			
		}

		shininess = 256.0f; // TODO: specify the shininess by reading from some custom data written in the .obj file. Can be done in blender.
	}

	Material material;
	material.fill(diffuse, specular, normal, shininess);
	Mesh newMesh;
	newMesh.fill(vertices, indices, material);
	return newMesh;
}

Texture* Model::loadTextureFromMaterial(const aiMaterial* mat, aiTextureType type, Format internalFormat, std::map<std::string, Texture>* loadedTextures)
{
	if (mat->GetTextureCount(type)) {

		aiString texturePath;
		mat->GetTexture(type, 0, &texturePath);
		//std::string fullPath = m_directory + "/" + std::string(texturePath.C_Str());
		//std::string fullPath = std::string(texturePath.C_Str());
		std::string fullPath = std::string(texturePath.C_Str());

		std::regex regex("\\\\res\\\\");

		std::vector<std::string> out(
			std::sregex_token_iterator(fullPath.begin(), fullPath.end(), regex, -1),
			std::sregex_token_iterator()
		);

		fullPath = (out.size() == 2) ? ".\\res\\" + out.at(1) : fullPath;


		auto alreadyLoaded = loadedTextures->find(fullPath);
		if (alreadyLoaded == loadedTextures->end())
		{
			// load new texture
			Texture newTexture{ fullPath, internalFormat };
			alreadyLoaded = loadedTextures->insert({fullPath, std::move(newTexture) }).first;
		}
		
		return &(alreadyLoaded->second);

	}
	else
	{
		return nullptr;
	}
 }
