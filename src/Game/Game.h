#pragma once

#include <glm/vec2.hpp>
#include <array>
#include <memory>

class IGameState;
namespace RendererEngine {
	class ShaderProgram;
}

class Game {
public:
	enum class GameMode {
		OnePlayer, TwoPlayer
	};

	Game(const glm::uvec2& g_windowSize);
	~Game();

	void render();
	void update(const double delta);
	void setKey(const int key, const int action);
	bool init();
	size_t getCurrentWidth() const;
	size_t getCurrentHeight() const;
	void startNewLevel(const size_t level, const GameMode eGameMode);
	void setWindowSize(const glm::uvec2& g_windowSize);
	void updateViewport();
	void nextLevel(const GameMode eGameMode);

private:

	// флаги обработки нажатия клавиш(всего 348 возможных комбинаций нажатий на клавиши)
	std::array<bool, 349> m_keys;

	enum class EGameState {
		StartScreen, LevelStart, Level, Pause, Scores, GameOver
	};

	glm::ivec2 m_windowSize;
	EGameState m_eCurrentGameState;

	std::shared_ptr<IGameState> m_pCurrentGameState;
	std::shared_ptr<RendererEngine::ShaderProgram> m_pSpriteShaderProgram;
	size_t m_currentLevelIndex;
};