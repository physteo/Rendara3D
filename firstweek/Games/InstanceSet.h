#pragma once

#include "../Model.h"
#include "../ObjectPool.h"



template <class HasTransform>
class InstanceSet
{
	size_t m_numberOfMeshes;
	Model* m_model;
	//memory::ObjectPool< std::pair<  std::pair<glm::mat4, glm::mat4>, HasTransform>  >  instances;

	memory::ObjectPool<HasTransform>    m_objects;
	memory::ObjectPool<glm::mat4>       m_modelMatrices;
	memory::ObjectPool<glm::mat4>       m_normalMatrices;


public:

	void drawInstances(Shader& shader)
	{
		const std::vector<Mesh>* meshes = this->m_model->getMeshes();
	
		for (size_t i = 0; i < meshes->size(); i++)
		{
			const Mesh* mesh = &meshes->at(i);
	
			mesh->passMaterialUniforms(shader);
	
			// prepare the attributes and then draw
			unsigned int bufferModelMatrix;
			glGenBuffers(1, &bufferModelMatrix);
			glBindBuffer(GL_ARRAY_BUFFER, bufferModelMatrix);
			glBufferData(GL_ARRAY_BUFFER, m_objects.size() * sizeof(glm::mat4), m_modelMatrices.getPointerToFirst(), GL_STATIC_DRAW);
	
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
			glBufferData(GL_ARRAY_BUFFER, m_objects.size() * sizeof(glm::mat4), m_normalMatrices.getPointerToFirst(), GL_STATIC_DRAW);


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

			glDrawElementsInstanced(GL_TRIANGLES, mesh->getIndices(), GL_UNSIGNED_INT, 0, m_objects.size());

			glBindVertexArray(0);
			glDeleteBuffers(1, &bufferModelMatrix);
			glDeleteBuffers(1, &bufferNormalMatrix);

		}
	
	}

	InstanceSet(size_t maxElements) : m_objects{ maxElements }, m_modelMatrices(maxElements), m_normalMatrices(maxElements), m_model(nullptr) {}

	InstanceSet(size_t maxElements, Model* modelIn) : m_objects{ maxElements }, m_modelMatrices(maxElements), m_normalMatrices(maxElements), m_model(modelIn)
	{
		m_numberOfMeshes = m_model->getMeshes()->size();
	}

	size_t size() const
	{
		return m_objects.size();
	}

	void deleteElement(size_t i)
	{
		m_objects.deleteElement(i);
		m_modelMatrices.deleteElement(i);
		m_normalMatrices.deleteElement(i);
	}

	void push_back(const HasTransform& h)
	{
		m_objects.addBackElement();
		m_modelMatrices.addBackElement();
		m_normalMatrices.addBackElement();
		
		// fill the last 
		m_objects.back() = h;
		recomputeMatrices(m_objects.size() - 1);
	}

	void setModel(Model* modelIn)
	{ 
		m_model = modelIn;
		m_numberOfMeshes = modelIn->getMeshes()->size();
	}


	const HasTransform& getElement(size_t i) const { return m_objects.at(i); }
	void setElement(size_t i, const HasTransform& hasTransform)
	{ 
		m_objects.at(i) = hasTransform;
		recomputeMatrices(i);
	}

private:


	void recomputeMatrices(size_t i)
	{
		Transform& transform = m_objects.at(i).transform;

		glm::mat4 modelMatrix{ 1.0 };
		modelMatrix = glm::translate(modelMatrix, transform.position);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), glm::vec3{ 0.0f,0.0f,1.0f });
		modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotation.z), glm::vec3{ 0.0f,0.0f,1.0f });
		modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotation.y), glm::vec3{ 0.0f,1.0f,0.0f });
		modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotation.x), glm::vec3{ 1.0f,0.0f,0.0f });
		modelMatrix = glm::scale(modelMatrix, transform.scale);

		glm::mat3 normalMatrix = glm::inverse(glm::transpose(modelMatrix));

		m_modelMatrices.at(i) = modelMatrix;
		m_normalMatrices.at(i) = normalMatrix;
	}

};


template <class HasTransformHasColor>
class InstanceSetQuads
{
	Model* m_model;

	memory::ObjectPool<HasTransformHasColor>    m_objects;
	memory::ObjectPool<glm::mat4>       m_modelMatrices;
	memory::ObjectPool<glm::vec4>       m_colors;


public:

	void drawInstances(Shader& shader)
	{
		const std::vector<Mesh>* meshes = this->m_model->getMeshes();

		for (size_t i = 0; i < meshes->size(); i++)
		{
			const Mesh* mesh = &meshes->at(i);

			mesh->passMaterialUniforms(shader);

			// prepare the attributes and then draw
			unsigned int bufferModelMatrix;
			glGenBuffers(1, &bufferModelMatrix);
			glBindBuffer(GL_ARRAY_BUFFER, bufferModelMatrix);
			glBufferData(GL_ARRAY_BUFFER, m_objects.size() * sizeof(glm::mat4), m_modelMatrices.getPointerToFirst(), GL_STATIC_DRAW);

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

			unsigned int bufferColor;
			glGenBuffers(1, &bufferColor);
			glBindBuffer(GL_ARRAY_BUFFER, bufferColor);
			glBufferData(GL_ARRAY_BUFFER, m_objects.size() * sizeof(glm::mat4), m_colors.getPointerToFirst(), GL_STATIC_DRAW);

			glEnableVertexAttribArray(8);
			glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
			glVertexAttribDivisor(8, 1);

			glDrawElementsInstanced(GL_TRIANGLES, mesh->getIndices(), GL_UNSIGNED_INT, 0, m_objects.size());

			glBindVertexArray(0);
			glDeleteBuffers(1, &bufferModelMatrix);
			glDeleteBuffers(1, &bufferColor);

		}

	}

	InstanceSetQuads(size_t maxElements) : m_objects{ maxElements }, m_modelMatrices(maxElements), m_colors(maxElements), m_model(nullptr) {}

	InstanceSetQuads(size_t maxElements, Model* modelIn) : m_objects{ maxElements }, m_modelMatrices(maxElements), m_colors(maxElements), m_model(modelIn)
	{
	}

	size_t size() const
	{
		return m_objects.size();
	}

	void deleteElement(size_t i)
	{
		m_objects.deleteElement(i);
		m_modelMatrices.deleteElement(i);
		m_colors.deleteElement(i);
	}

	void push_back(const HasTransformHasColor& h)
	{
		m_objects.addBackElement();
		m_modelMatrices.addBackElement();
		m_colors.addBackElement();

		// fill the last 
		m_objects.back() = h;
		recomputeMatrices(m_objects.size() - 1);
	}

	void setModel(Model* modelIn)
	{
		m_model = modelIn;
	}


	const HasTransformHasColor& getElement(size_t i) const { return m_objects.at(i); }
	void setElement(size_t i, const HasTransformHasColor& hasTransform)
	{
		m_objects.at(i) = hasTransform;
		recomputeMatrices(i);
	}

private:


	void recomputeMatrices(size_t i)
	{
		Transform& transform = m_objects.at(i).transform;

		glm::mat4 modelMatrix{ 1.0 };
		modelMatrix = glm::translate(modelMatrix, transform.position);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), glm::vec3{ 0.0f,0.0f,1.0f });
		modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotation.z), glm::vec3{ 0.0f,0.0f,1.0f });
		modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotation.y), glm::vec3{ 0.0f,1.0f,0.0f });
		modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotation.x), glm::vec3{ 1.0f,0.0f,0.0f });
		modelMatrix = glm::scale(modelMatrix, transform.scale);


		m_modelMatrices.at(i) = modelMatrix;
		m_colors = m_objects.at(i).color;
	}
};