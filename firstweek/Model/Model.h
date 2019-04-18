#pragma once

#include <regex>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"


//!< Class for 3D models.
/*!
	It stores a vector meshes that make up the model. The meshes are loaded from an .obj file
	specified through the path. If the .obj file does not have a diffuse texture map, then 
	the model will use a 1x1 pixel texture of the specified defaultColor color. If the model does not 
	have a specular texture map, it will be defaulted to black (no specular effect). If the model does not
	have a normal texture map, the normals will be orthogonal to the surfaces of the model.
	Takes and possibly modifies an array of textures: if the model uses already loaded textures (compared
	through the texture filepath), then it will use those. If not, they will be added to loadedTextures.
*/
class Model
{
public:
	Model()
	{
		// the default model has no meshes.
		m_path = "nopath-emptymodel";
		m_defaultColor = glm::vec3{1.0f};
		m_castsShadows = false;
	}
	Model(const std::string path, const glm::vec3& defaultColor, std::vector<Texture>* loadedTextures)
	{
		m_path = path;
		m_defaultColor = defaultColor;
		loadModel(path, loadedTextures);
		m_castsShadows = false;
	}

	Model(const std::string path, std::vector<Texture>* loadedTextures) : Model{ path, glm::vec3{1.0f}, loadedTextures }
	{}	
	
	inline void castsShadows(bool casts)           {	m_castsShadows = casts;	}
	inline bool castsShadows()               const { return m_castsShadows; }
	

	void draw(const glm::vec3& scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader) const;
	void draw(float scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader) const;

	void draw(const glm::vec3& scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader, Material& material) const;
	void draw(float scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader, Material& material) const;


	const std::vector<Mesh>* getMeshes() const { return &m_meshes; }
	const std::string& getPath() const { return m_path; }


private:
	std::vector<Mesh> m_meshes;
	std::string		  m_path;
	glm::vec3         m_defaultColor;

	bool              m_castsShadows;


	void loadModel(const std::string path, std::vector<Texture>* loadedTextures);
	void processNode(aiNode* node,   const aiScene* scene, std::vector<Texture>* loadedTextures);
	Mesh processMesh(aiMesh* aimesh, const aiScene* scene, std::vector<Texture>* loadedTextures);
	Texture* loadMaterialTexture(aiMaterial* mat, aiTextureType type, Format internalFormat, std::vector<Texture>* loadedTextures);

};