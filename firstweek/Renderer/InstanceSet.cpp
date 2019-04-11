//for (size_t i = 0; i < m_meshTable.size(); i++)
//{
//	// retrieve all the models having this mesh(to have data of positions, rotations, etc)
//	std::vector<const GameObject*>& GameObjects = m_meshTable.at(i).second;
//	const Mesh* mesh = m_meshTable.at(i).first;
//
//	// create the list of model matrices (for optimization, this should be done once for each shader used; at the submit stage )
//	std::vector<glm::mat4> modelMatrices;
//	std::vector<glm::mat4> normalMatrices;
//	modelMatrices.reserve(GameObjects.size());
//	normalMatrices.reserve(GameObjects.size());
//
//	for (unsigned int j = 0; j < GameObjects.size(); j++)
//	{
//		glm::mat4 modelMatrix{ 1.0 };
//		modelMatrix = glm::translate(modelMatrix, GameObjects.at(j)->position);
//		modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), glm::vec3{ 0.0f,0.0f,1.0f });
//		modelMatrix = glm::rotate(modelMatrix, glm::radians(GameObjects.at(j)->rotation.z), glm::vec3{ 0.0f,0.0f,1.0f });
//		modelMatrix = glm::rotate(modelMatrix, glm::radians(GameObjects.at(j)->rotation.y), glm::vec3{ 0.0f,1.0f,0.0f });
//		modelMatrix = glm::rotate(modelMatrix, glm::radians(GameObjects.at(j)->rotation.x), glm::vec3{ 1.0f,0.0f,0.0f });
//		modelMatrix = glm::scale(modelMatrix, GameObjects.at(j)->scale);
//		modelMatrices.push_back(modelMatrix);
//
//		glm::mat3 normalMatrix = glm::inverse(glm::transpose(modelMatrix));
//		normalMatrices.push_back(normalMatrix);
//	}
//
//	// activate the shader
//	mesh->passMaterialUniforms(*shader);
//
//	// prepare the attributes and then draw
//	unsigned int bufferModelMatrix;
//	glGenBuffers(1, &bufferModelMatrix);
//	glBindBuffer(GL_ARRAY_BUFFER, bufferModelMatrix);
//	glBufferData(GL_ARRAY_BUFFER, GameObjects.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
//
//
//		// bind vao and enable vertex attributes (specifying the layout)
//		mesh->bindVao();
//		glEnableVertexAttribArray(4);
//		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
//		glEnableVertexAttribArray(5);
//		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
//		glEnableVertexAttribArray(6);
//		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
//		glEnableVertexAttribArray(7);
//		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
//
//		glVertexAttribDivisor(4, 1);
//		glVertexAttribDivisor(5, 1);
//		glVertexAttribDivisor(6, 1);
//		glVertexAttribDivisor(7, 1);
//
//
//	unsigned int bufferNormalMatrix;
//	glGenBuffers(1, &bufferNormalMatrix);
//	glBindBuffer(GL_ARRAY_BUFFER, bufferNormalMatrix);
//	glBufferData(GL_ARRAY_BUFFER, GameObjects.size() * sizeof(glm::mat4), &normalMatrices[0], GL_STATIC_DRAW);
//
//
//	glEnableVertexAttribArray(8);
//	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
//	glEnableVertexAttribArray(9);
//	glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
//	glEnableVertexAttribArray(10);
//	glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
//	glEnableVertexAttribArray(11);
//	glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
//
//	glVertexAttribDivisor(8, 1);
//	glVertexAttribDivisor(9, 1);
//	glVertexAttribDivisor(10, 1);
//	glVertexAttribDivisor(11, 1);
//
//	glDrawElementsInstanced(GL_TRIANGLES, mesh->getIndices(), GL_UNSIGNED_INT, 0, GameObjects.size());
//
//	glBindVertexArray(0);
//	glDeleteBuffers(1, &bufferModelMatrix);
//	glDeleteBuffers(1, &bufferNormalMatrix);
//
//
//	m_meshTable.deleteElement(i);
//	paths.deleteElement(i);
//}
//