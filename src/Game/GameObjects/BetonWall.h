#pragma once

#include "IGameObject.h"

#include <memory>
#include <array>

namespace RendererEngine {
	class Sprite;
}

class BetonWall : public IGameObject {
public:

	enum class EBetonWallType {
		All, Top, Bottom, Left, Right, TopLeft, TopRight, BottomLeft, BottomRight
	};

	enum class EBetonState : uint8_t {
        Enabled = 0,
        Destroyed
	};

    enum class EBlockLocation : uint8_t {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };

    BetonWall(const EBetonWallType eBrickWallType, const glm::vec2& position, const glm::vec2& size, const float rotation, const float layer);
	virtual void render() const override;
	virtual void update(const double delta) override;

private:
    void renderBrick(const EBlockLocation eBrickLocation) const;

    std::array<EBetonState, 4> m_eCurrentBlockState;
	std::shared_ptr<RendererEngine::Sprite> m_sprite;
    std::array<glm::vec2, 4> m_blockOffsets;
};