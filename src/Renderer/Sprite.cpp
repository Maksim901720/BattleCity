#include "Sprite.h"

#include "ShaderProgram.h"
#include "Texture2D.h"

#include <glm/mat4x2.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Renderer {
	Sprite::Sprite(std::shared_ptr<Texture2D> pTexture, std::shared_ptr<ShaderProgram> pShaderProgram,
		const glm::vec2& position, const glm::vec2& size, const float rotation)
		: m_pSharedProgram(std::move(pShaderProgram)), m_pTexture(std::move(pTexture)), 
		m_position(position), m_size(size), m_rotation(rotation)
	{
		const GLfloat  vertexCoords[] = {
			// 2-3  1
			// |/  /|
			// 1  3-2

			// X     Y 

			0.0f, 0.0f,
			0.0f, 1.0f, 
			1.0f, 1.0f,

			1,0.f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f
		};

		const GLfloat textureCoords[] = {
			// U     V
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,

			1,0.f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f
		};

		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		glGenBuffers(1, &m_vertexCoordsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexCoordsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexCoords), vertexCoords, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

		glGenBuffers(1, &m_textureCoordsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_textureCoordsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	Sprite::~Sprite()
	{
		// освобождаем пам€ть в GPU, выделенную дл€ этих данных
		glDeleteBuffers(1, &m_vertexCoordsVBO);
		glDeleteBuffers(1, &m_textureCoordsVBO);
		glDeleteVertexArrays(1, &m_VAO);
	}

	void Sprite::render() const
	{
		m_pSharedProgram->use();

		glm::mat4 model(1.f);

		// ƒ≈…—“¬»я Ѕ”ƒ”“ ¬ќ—ѕ–ќ»«¬ќƒ»“—я ¬ ќЅ–ј“Ќќћ ѕќ–яƒ ≈
		// мен€ем позицию спрайта
		model = glm::translate(model, glm::vec3(m_position, 0.0f));
		// вращаем относительно центральной оси
		model = glm::translate(model, glm::vec3(0.5f * m_size.x, 0.5 * m_size.y, 0.0f));
		model = glm::rotate(model, glm::radians(m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-0.5f * m_size.x, -0.5 * m_size.y, 0.0f));
		// измен€ем размер
		model = glm::scale(model, glm::vec3(m_size, 1.0));

		glBindVertexArray(m_VAO);
		m_pSharedProgram->setMatrix4("modelMat", model);

		glActiveTexture(GL_TEXTURE0);
		m_pTexture->bind();

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	void Sprite::setPosition(const glm::vec2& position)
	{
		m_position = position;
	}

	void Sprite::setSize(const glm::vec2& size)
	{
		m_size = size;
	}

	void Sprite::setRotation(const float rotation)
	{
		m_rotation = rotation;
	}
}