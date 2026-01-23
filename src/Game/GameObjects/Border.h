#pragma once

#include "IGameObject.h"

#include <memory>
#include <array>

namespace RendererEngine {
	class Sprite;
}

class Border : public IGameObject {
public:

	Border(const glm::vec2& position, const glm::vec2& size, const float rotation, const float layer);
	virtual void render() const override;

private:
	std::shared_ptr<RendererEngine::Sprite> m_sprite;
};