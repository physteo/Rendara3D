#pragma once

#include "Renderer.h"
#include <deque>


class Simple3DRenderer : public Renderer
{
	std::vector< std::pair<Shader*, std::deque<const Model*    > >  >      m_modelsTable;
	std::vector< std::pair<Shader*, std::deque<Transform*> >  >  m_transformsTable;

public:
	Simple3DRenderer() : Simple3DRenderer(50) {}
	Simple3DRenderer(size_t reservedSize)
	{
		m_modelsTable.reserve(reservedSize);
		m_transformsTable.reserve(reservedSize);
	}

	virtual void submit(const Model* model, // what to draw
		Transform* transform)   // where to draw
	{
		bool shaderAlreadyExists = false;
		// loop over tables to see it there is already that shader
		for (size_t i = 0; i < m_modelsTable.size(); i++)
		{
			const Shader* shader = m_modelsTable.at(i).first;
			std::deque<const Model*>&     models     = m_modelsTable.at(i).second;
			std::deque<Transform*>& transforms = m_transformsTable.at(i).second;

			if (model->getShader() == shader)
			{
				models.push_back(model);
				transforms.push_back(transform);
				shaderAlreadyExists = true;
				break;
			}
		}
		if (!shaderAlreadyExists)
		{
			// add entry to the vectors
			m_modelsTable.push_back(std::make_pair(model->getShader(), std::deque<const Model*>{}));
			m_transformsTable.push_back(std::make_pair(model->getShader(), std::deque<Transform*>{}));
			m_modelsTable.back().second.push_back(model);
			m_transformsTable.back().second.push_back(transform);
		}
	}



	virtual void clear() override
	{
		size_t currentSize = m_modelsTable.size();
		m_modelsTable.clear();
		m_transformsTable.clear();

		// TODO: is this useful? reserving at each frame could make it slow
		//m_modelsTable.reserve(currentSize);
		//m_transformsTable.reserve(currentSize);
	}

	virtual void draw() override
	{
		// draw
		for (size_t i = 0; i < m_modelsTable.size(); i++)
		{
			Shader* shader = m_modelsTable.at(i).first;
			shader->bind();
			drawTable(i, *shader);
		}

	}

	virtual void draw(Shader& shader)
	{
		shader.bind();
		// draw
		for (size_t i = 0; i < m_modelsTable.size(); i++)
		{
			drawTable(i, shader);
		}
	}

private:
	void drawTable(size_t i, Shader& shader)
	{
		std::deque<const Model*>&     models = m_modelsTable.at(i).second;
		std::deque<Transform*>& transforms = m_transformsTable.at(i).second;
		for (size_t j = 0; j < models.size(); j++)
		{
			const Model*     model = models.at(j);
			Transform* transform = transforms.at(j);
			model->draw(transform->scale, transform->position, transform->rotation, shader);
		}
	}
};



