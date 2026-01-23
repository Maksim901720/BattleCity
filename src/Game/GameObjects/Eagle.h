#pragma once

#include "IGameObject.h"
#include "../../Renderer/SpriteAnimator.h"

#include <memory>
#include <array>

namespace RendererEngine {
	class Sprite;
}

class Eagle : public IGameObject {
public:

    enum class EEagleState : uint8_t {
        Alive = 0, 
        Dead
    };

    Eagle(const glm::vec2& position, const glm::vec2& size, const float rotation, const float layer);
	void render() const override;
    void update(const double delta);

private:

	std::array<std::shared_ptr<RendererEngine::Sprite>, 2> m_sprite;
    EEagleState m_eCurrentState;
};