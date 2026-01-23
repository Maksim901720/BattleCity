#pragma once

#include "Sprite.h"

#include <map>
#include <vector>

namespace RendererEngine {

	class AnimatedSprite : public Sprite {
	public:
		using state_map = std::vector<std::pair<std::string, uint64_t>>;

		AnimatedSprite(std::shared_ptr<Texture2D> pTexture,
			   const std::string &initialSubTexture,
			   std::shared_ptr<ShaderProgram> pSharedProgram);

		void insertState(std::string state, state_map subTexturesDuration);
		void render(const glm::vec2& position, const glm::vec2& size, const float rotation) const override;

		void setState(const std::string& newState);
		void update(const uint64_t delta);
		
	private:
		std::map<std::string, state_map> m_stateMap;
		size_t m_currentFrame = 0;
		uint64_t m_currentAnimationTime = 0;
		std::map<std::string, state_map>::const_iterator m_pCurrentAnimationDurations;
		mutable bool m_dirty = false;
	};
}