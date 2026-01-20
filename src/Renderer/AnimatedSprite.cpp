#include "AnimatedSprite.h"
#include "Texture2D.h"

#include <iostream>

namespace Renderer {

	AnimatedSprite::AnimatedSprite(std::shared_ptr<Texture2D> pTexture,
		const std::string& initialSubTexture,
		std::shared_ptr<ShaderProgram> pSharedProgram,
		const glm::vec2& position,
		const glm::vec2& size,
		const float rotation)
		: Sprite(std::move(pTexture), initialSubTexture, std::move(pSharedProgram), position, size, rotation)
	{ 
		m_pCurrentAnimationDurations = m_stateMap.end();
	}
		 
	void AnimatedSprite::insertState(std::string state, state_map subTexturesDuration)
	{
		m_stateMap.emplace(std::move(state), std::move(subTexturesDuration));
	}

	void AnimatedSprite::setState(const std::string& newState)
	{
		auto it = m_stateMap.find(newState);
		if (it == m_stateMap.end()) {
			std::cout << "Can't find animation state: " << newState << std::endl;
			return;
		}

		if (it != m_pCurrentAnimationDurations) {
			m_currentAnimationTime = 0;
			m_currentFrame = 0;
			m_pCurrentAnimationDurations = it;
			m_dirty = true;
		}
	}

	void AnimatedSprite::update(const uint64_t delta)
	{
		if (m_pCurrentAnimationDurations != m_stateMap.end()) {
			m_currentAnimationTime += delta;

			while (m_currentAnimationTime >= m_pCurrentAnimationDurations->second[m_currentFrame].second) {
				m_currentAnimationTime -= m_pCurrentAnimationDurations->second[m_currentFrame].second;
				++m_currentFrame;
				m_dirty = true;

				if (m_currentFrame == m_pCurrentAnimationDurations->second.size()) {
					m_currentFrame = 0;
				}
			}
		}
	}

	void AnimatedSprite::render() const 
	{
		if (m_dirty) {
			auto subTexture = m_pTexture->getTexture(m_pCurrentAnimationDurations->second[m_currentFrame].first);

			const GLfloat textureCoords[] = {
				// U     V
				subTexture.leftBottomUV.x, subTexture.leftBottomUV.y,
				subTexture.leftBottomUV.x, subTexture.rightToUV.y,
				subTexture.rightToUV.x,    subTexture.rightToUV.y,

				subTexture.rightToUV.x,    subTexture.leftBottomUV.y,
				subTexture.rightToUV.x,    subTexture.rightToUV.y,
				subTexture.leftBottomUV.x, subTexture.leftBottomUV.y
			};

			glBindBuffer(GL_ARRAY_BUFFER, m_textureCoordsVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(textureCoords), textureCoords);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			m_dirty = false;
		}

		Sprite::render();
	}
}