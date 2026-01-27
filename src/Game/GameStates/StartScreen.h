#pragma once

#include <vector>
#include <string>
#include <memory>
#include <glm/vec2.hpp>

#include "IGameState.h"
#include "../../Renderer/SpriteAnimator.h"

namespace RendererEngine {
	class Sprite;
}

class Game;

class StartScreen : public IGameState {
public:
	static constexpr unsigned int BLOCK_SIZE = 8;
	static constexpr unsigned int STARTSCREEN_WIDTH = 15 * BLOCK_SIZE * 2;
	static constexpr unsigned int STARTSCREEN_HEIGHT = 14 * BLOCK_SIZE * 2;
	static constexpr unsigned int MENU_WIDTH= 128;
	static constexpr unsigned int MENU_HEIGHT = 64;
	static constexpr unsigned int SIZE_TANK = BLOCK_SIZE * 2;

	StartScreen(const std::vector<std::string>& startScreenDescription, Game *pGame);
	void render() const;
	void update(const double delta);
	unsigned int getStateWidth() const override;
	unsigned int getStateHeight() const override;
	void processInput(std::array<bool, 349>& keys) override;

private:
	Game* m_pGame;
	int m_currentMenuSelevtion;
	bool m_keyReleased;
	std::vector<std::pair<std::shared_ptr<RendererEngine::Sprite>, glm::vec2>> m_sprites;
	std::pair<std::shared_ptr<RendererEngine::Sprite>, glm::vec2> m_menuSprite;
	std::pair<std::shared_ptr<RendererEngine::Sprite>, glm::vec2> m_tankSprite;
	RendererEngine::SpriteAnimator m_tankSpriteAnimator;
};