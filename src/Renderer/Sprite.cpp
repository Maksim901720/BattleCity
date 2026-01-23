#include "Sprite.h"

#include "ShaderProgram.h"
#include "Texture2D.h"
#include "Renderer.h"

#include <glm/mat4x2.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace RendererEngine {
	Sprite::Sprite(std::shared_ptr<Texture2D> pTexture, 
				   const std::string &initialSubTexture, 
				   std::shared_ptr<ShaderProgram> pShaderProgram)
			: m_pSharedProgram(std::move(pShaderProgram)), m_pTexture(std::move(pTexture)), m_lastFrameId(0)
	{
		m_framesDescriptions.clear();

		const GLfloat  vertexCoords[] = {
			// 1---2
			// | / |
			// 0---3

			// X     Y 

			0.0f, 0.0f,
			0.0f, 1.0f, 
			1.0f, 1.0f,
			1.0f, 0.0f	
		};
		 
		auto subTexture = m_pTexture->getSubTexture(initialSubTexture);

		const GLfloat textureCoords[] = {
			// U     V
			subTexture.leftBottomUV.x, subTexture.leftBottomUV.y,
			subTexture.leftBottomUV.x, subTexture.rightTopUV.y,
			subTexture.rightTopUV.x,    subTexture.rightTopUV.y,
			subTexture.rightTopUV.x,    subTexture.leftBottomUV.y,
		};

		const GLuint indces[] = {
			0, 1, 2,
			2, 3, 0
		};

		m_vertexCoordsBuffer.init(vertexCoords, 4 * 2 * sizeof(GLfloat));
		VertexBufferLayout vertexCoordsLayout;
		vertexCoordsLayout.addElementLayoutFloat(2, false);
		m_vertexArray.addBuffer(m_vertexCoordsBuffer, vertexCoordsLayout);

		m_textureCoordsBuffer.init(textureCoords, 4 * 2 * sizeof(GLfloat));
		VertexBufferLayout textureCoordsLayout;
		textureCoordsLayout.addElementLayoutFloat(2, false);
		m_vertexArray.addBuffer(m_textureCoordsBuffer, textureCoordsLayout);

		m_indexBuffer.init(indces, 6);

		m_vertexArray.unbind();
		m_indexBuffer.unbind();
	}

	Sprite::~Sprite()
	{
	}

	void Sprite::render(const glm::vec2& position, const glm::vec2& size, const float rotation, 
		const float layer, const size_t frameId) const
	{
		if (m_lastFrameId != frameId && !m_framesDescriptions.empty()) {
			const FrameDescription& currentFrameDescription = m_framesDescriptions[frameId];

			const GLfloat textureCoords[] = {
				// U     V
				currentFrameDescription.leftBottomUV.x, currentFrameDescription.leftBottomUV.y,
				currentFrameDescription.leftBottomUV.x, currentFrameDescription.rightTopUV.y,
				currentFrameDescription.rightTopUV.x,    currentFrameDescription.rightTopUV.y,
				currentFrameDescription.rightTopUV.x,    currentFrameDescription.leftBottomUV.y,
			};

			m_textureCoordsBuffer.update(textureCoords, 4 * 2 * sizeof(GLfloat));
			m_lastFrameId = frameId;
		}
		
		m_pSharedProgram->use();
		glm::mat4 model(1.f);

		// ÄÅÉÑÒÂÈß ÁÓÄÓÒ ÂÎÑÏÐÎÈÇÂÎÄÈÒÑß Â ÎÁÐÀÒÍÎÌ ÏÎÐßÄÊÅ
		// ìåíÿåì ïîçèöèþ ñïðàéòà
		model = glm::translate(model, glm::vec3(position, 0.0f));
		// âðàùàåì îòíîñèòåëüíî öåíòðàëüíîé îñè
		model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5 * size.y, 0.0f));
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5 * size.y, 0.0f));
		// èçìåíÿåì ðàçìåð
		model = glm::scale(model, glm::vec3(size, 1.0));

		m_pSharedProgram->setMatrix4("modelMat", model);
		m_pSharedProgram->setFloat("layer", layer);

		glActiveTexture(GL_TEXTURE0);
		m_pTexture->bind();		

		Renderer::draw(m_vertexArray, m_indexBuffer, *m_pSharedProgram);
	}

	void Sprite::insertFrames(std::vector<FrameDescription> framesDescriptions)
	{
		m_framesDescriptions = std::move(framesDescriptions);
	}

	double Sprite::getFrameDuration(const size_t frameId) const
	{
		return m_framesDescriptions[frameId].duration;
	}

	size_t Sprite::getFramesCount() const
	{
		return m_framesDescriptions.size();
	}
}