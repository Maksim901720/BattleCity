#pragma once

#include "IGameObject.h"

#include <memory>
#include <array>

namespace RendererEngine {
	class Sprite;
}

class BrickWall : public IGameObject {
public:

	enum class EBrickWallType : uint8_t {
		All, Top, Bottom, Left, Right, TopLeft, TopRight, BottomLeft, BottomRight
	};

	enum class EBrickState : uint8_t {
        All,
        TopLeft,
        TopRight,
        Top,
        BottomLeft,
        Left,
        TopRight_BottomLeft,
        Top_BottomLeft,
        BottomRight,
        TopLeft_BottomRight,
        Right,
        Top_BottomRight,
        Bottom,
        TopLeft_Bottom,
        TopRight_Bottom,
        Destroyed
	};

    enum class EBrickLocation : uint8_t {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };

	BrickWall(const EBrickWallType eBrickWallType, const glm::vec2& position, const glm::vec2& size, const float rotation, const float layer);
	virtual void render() const override;

private:
    void BrickWall::renderBrick(const EBrickLocation eBrickLocation) const;
    static EBrickState getBrickStateAfterCollision(const EBrickState currentState, const Physics::ECollisionDirection direction);
    static Physics::AABB getAABBForBrickState(const EBrickLocation location, const EBrickState eBrickState, const glm::vec2& size);
    void onCollisionCallback(const EBrickLocation location, const IGameObject& object, const Physics::ECollisionDirection direction);

    std::array<Physics::Collider*, 4> m_brickLocationToColliderMap;

    std::array<EBrickState, 4> m_eCurrentBrickState;
	std::array<std::shared_ptr<RendererEngine::Sprite>, 15> m_sprites;
    std::array<glm::vec2, 4> m_blockOffsets;
};