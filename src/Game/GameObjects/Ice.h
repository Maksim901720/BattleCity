#pragma once

#include "IGameObject.h"

#include <memory>
#include <array>

namespace RendererEngine {
	class Sprite;
}

class Ice : public IGameObject {
public:

    enum class EBlockLocation : uint8_t {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };

    Ice(const glm::vec2& position, const glm::vec2& size, const float rotation, const float layer);
	virtual void render() const override;

private:
    void renderBrick(const EBlockLocation eBrickLocation) const;

	std::shared_ptr<RendererEngine::Sprite> m_sprite;
    std::array<glm::vec2, 4> m_blockOffsets;
};