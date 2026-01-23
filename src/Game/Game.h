#pragma once

#include <glm/vec2.hpp>
#include <array>
#include <memory>

class Tank;
class Level;

class Game {
public:
	Game(const glm::ivec2& g_windowSize);
	~Game();

	void render();
	void update(const double delta);
	void setKey(const int key, const int action);
	bool init();
	size_t getCurrentLevelWidth() const;
	size_t getCurrentLevelHeight() const;

private:

	// флаги обработки нажатия клавиш(всего 348 возможных комбинаций нажатий на клавиши)
	std::array<bool, 349> m_keys;

	enum class EGameState {
		Active, Pause
	};

	glm::ivec2 m_windowSize;
	EGameState m_eCurrentGameState;
	std::shared_ptr<Tank> m_pTank;
	std::shared_ptr<Level> m_pLevel;
};