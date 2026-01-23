#pragma once

#include "IGameObject.h"
#include "../../Renderer/SpriteAnimator.h"

#include <memory>
#include <array>

namespace RendererEngine {
	class Sprite;
}

class Water : public IGameObject {
public:

    enum class EBlockLocation {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };

    Water(const glm::vec2& position, const glm::vec2& size, const float rotation, const float layer);
	void render() const override;
    void update(const double delta);
    bool collides(const EObjectType objectType) override;

private:
    void renderBrick(const EBlockLocation eBrickLocation) const;

	std::shared_ptr<RendererEngine::Sprite> m_sprite;
    RendererEngine::SpriteAnimator m_spriteAnimator;
    std::array<glm::vec2, 4> m_blockOffsets;
};