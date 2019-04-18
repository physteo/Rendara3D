#pragma once

#include "Model.h"
#include "Games/GameComponent.h"

#include <deque>
#include "ObjectPool.h"

#define POOL

class InstanceRenderer
{
#ifdef DEQUE
	std::deque<
#elif  defined POOL
	memory::ObjectPool<
#else
	std::vector<
#endif
		std::string> paths; //TODO: remember to flush this as well
	
#ifdef DEQUE
	std::deque <
#elif defined POOL
	memory::ObjectPool<
#else
	std::vector<
#endif
		std::pair<const Mesh*, std::vector<const GameComponent*> >
				> m_meshTable;
	

public: 
	InstanceRenderer() : 
#ifdef POOL
		paths{4000}, m_meshTable{4000}
#else
		paths{}, m_meshTable{}
#endif
	{
//		paths.reserve(400);
//		m_meshTable.reserve(400);
	}

	virtual void submit(const GameComponent& gameComponent)
	{
		// check if model already exists in the table
		std::string path = gameComponent.getModel()->getPath();
		bool exists = false;
		for (size_t i = 0; i < paths.size(); i++)
		{
			if (paths.at(i) == path)
			{
				// already exists: add the model to the appropriate vector
				m_meshTable.at(i).second.push_back(&gameComponent);
				break;
			}
		}

		if (!exists)
		{
			// add it
#ifdef DEQUE
			paths.push_front(path);
#elif defined POOL
			paths.push_back(path);
#else
			paths.push_back(path);
#endif
			const Mesh* mesh = &gameComponent.getModel()->getMeshes()->at(0);
			std::vector<const GameComponent*> gameComponents;
			gameComponents.push_back(&gameComponent);

#ifdef DEQUE
			m_meshTable.push_front(std::make_pair(mesh, gameComponents));
#elif defined POOL
			m_meshTable.push_back(std::make_pair(mesh, gameComponents));
#else
			m_meshTable.push_back(std::make_pair(mesh, gameComponents));
#endif
		}


	}

	virtual void flush(Shader* shader)
	{

#ifdef DEQUE
		while (!m_meshTable.empty())
		{
			// retrieve all the models having this mesh(to have data of positions, rotations, etc)
			std::vector<const GameComponent*>& gameComponents = m_meshTable.front().second;
			const Mesh* mesh = m_meshTable.front().first;
#elif defined POOL
		for (size_t i = 0; i < m_meshTable.size(); i++)
		{
			// retrieve all the models having this mesh(to have data of positions, rotations, etc)
			std::vector<const GameComponent*>& gameComponents = m_meshTable.at(i).second;
			const Mesh* mesh = m_meshTable.at(i).first;
#else
		for (size_t i = 0; i < m_meshTable.size(); i++)
		{
			// retrieve all the models having this mesh(to have data of positions, rotations, etc)
			std::vector<const GameComponent*>& gameComponents = m_meshTable.at(i).second;
			const Mesh* mesh = m_meshTable.at(i).first;
#endif


			// create the list of model matrices (for optimization, this should be done once for each shader used; at the submit stage )
			std::vector<glm::mat4> modelMatrices;
			std::vector<glm::mat4> normalMatrices;
			modelMatrices.reserve(gameComponents.size());
			normalMatrices.reserve(gameComponents.size());

			for (unsigned int j = 0; j < gameComponents.size(); j++)
			{
				glm::mat4 modelMatrix{ 1.0 };
				modelMatrix = glm::translate(modelMatrix, gameComponents.at(j)->position);
				modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), glm::vec3{ 0.0f,0.0f,1.0f });
				modelMatrix = glm::rotate(modelMatrix, glm::radians(gameComponents.at(j)->rotation.z), glm::vec3{ 0.0f,0.0f,1.0f });
				modelMatrix = glm::rotate(modelMatrix, glm::radians(gameComponents.at(j)->rotation.y), glm::vec3{ 0.0f,1.0f,0.0f });
				modelMatrix = glm::rotate(modelMatrix, glm::radians(gameComponents.at(j)->rotation.x), glm::vec3{ 1.0f,0.0f,0.0f });
				modelMatrix = glm::scale(modelMatrix, gameComponents.at(j)->scale);
				modelMatrices.push_back(modelMatrix);

				glm::mat3 normalMatrix = glm::inverse(glm::transpose(modelMatrix));
				normalMatrices.push_back(normalMatrix);
			}

			// activate the shader
			mesh->passMaterialUniforms(*shader);

			// prepare the attributes and then draw
			unsigned int bufferModelMatrix;
			glGenBuffers(1, &bufferModelMatrix);
			glBindBuffer(GL_ARRAY_BUFFER, bufferModelMatrix);
			glBufferData(GL_ARRAY_BUFFER, gameComponents.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);


			// bind vao and enable vertex attributes (specifying the layout)
			mesh->bindVao();
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(7);
			glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);
			glVertexAttribDivisor(7, 1);


			unsigned int bufferNormalMatrix;
			glGenBuffers(1, &bufferNormalMatrix);
			glBindBuffer(GL_ARRAY_BUFFER, bufferNormalMatrix);
			glBufferData(GL_ARRAY_BUFFER, gameComponents.size() * sizeof(glm::mat4), &normalMatrices[0], GL_STATIC_DRAW);


			glEnableVertexAttribArray(8);
			glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
			glEnableVertexAttribArray(9);
			glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
			glEnableVertexAttribArray(10);
			glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(11);
			glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

			glVertexAttribDivisor(8, 1);
			glVertexAttribDivisor(9, 1);
			glVertexAttribDivisor(10, 1);
			glVertexAttribDivisor(11, 1);

			glDrawElementsInstanced(GL_TRIANGLES, mesh->getIndices(), GL_UNSIGNED_INT, 0, gameComponents.size());

			glBindVertexArray(0);
			glDeleteBuffers(1, &bufferModelMatrix);
			glDeleteBuffers(1, &bufferNormalMatrix);

#ifdef DEQUE
			m_meshTable.pop_front();
			paths.pop_front();
		}
#elif defined POOL
			m_meshTable.deleteElement(i);
			paths.deleteElement(i);
		}	
#else
		}
		// maybe not necessary to do it in every frame?
		m_meshTable.clear();
		paths.clear();
#endif

		
		
	}
};
