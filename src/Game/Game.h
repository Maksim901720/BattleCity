#pragma once

#include <glm/vec2.hpp>
#include <array>
#include <memory>

class Tank;

class Game {
public:
	Game(const glm::ivec2& g_windowSize);
	~Game();

	void render();
	void update(const uint64_t delta);
	void setKey(const int key, const int action);
	bool init();

private:

	// флаги обработки нажатия клавиш(всего 348 возможных комбинаций нажатий на клавиши)
	std::array<bool, 349> m_keys;

	enum class EGameState {
		Active, Pause
	};

	glm::ivec2 m_windowSize;
	EGameState m_eCurrentGameState;
	std::unique_ptr<Tank> m_pTank;
};