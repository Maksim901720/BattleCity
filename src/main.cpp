#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include <iostream>
#include <chrono>

#include "Resource/ResourceManager.h"
#include "Game/Game.h"
#include "Renderer/Renderer.h"
#include "Physics/PhysicsEngine.h"

static constexpr unsigned int SCALE = 3;
static constexpr unsigned int BLOCK_SIZE = 16;
glm::uvec2 g_windowSize(16 * BLOCK_SIZE, 15 * BLOCK_SIZE);
std::unique_ptr<Game> g_game = std::make_unique<Game>(g_windowSize);

// обрабатываем событие изменения окна
void glfwWindowSizeCallback(GLFWwindow* pWindow, int width, int height)
{
    g_windowSize.x = width;
    g_windowSize.y = height;
    g_game->setWindowSize(g_windowSize);
}

// обрабатываем события нажатия клавиш
void glfwKeyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mode)
{
    // если нажата клвиша esc
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        // устанавливаем флаг закрытия окна в true
        glfwSetWindowShouldClose(pWindow, GL_TRUE);
    } 
    g_game->setKey(key, action);
}

int main(int argc, char *argv[])
{
    // иницилизировать библиотеку
    if (!glfwInit()) {
        std::cerr << "error initilize library glfw" << std::endl;
        return -1;
    }

    // устанавливаем минимальную версию GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    // исключаем функции нужные для обратной совместимости
    // с предыдущими версиями OpenGL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Создать окно оконного режима и его контекст OpenGL 
    GLFWwindow *pWindow = glfwCreateWindow(g_windowSize.x, g_windowSize.y, "Battle city", nullptr, nullptr);
    if (!pWindow) {
        std::cerr << "failed create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // зарегестрируем обработчики событий окна
    glfwSetWindowSizeCallback(pWindow, glfwWindowSizeCallback);
    glfwSetKeyCallback(pWindow, glfwKeyCallback);

    // выбираем этот OpenGL контекст окна текущим, чтобы дальнейшые 
    // вызовы применялись к нему
    glfwMakeContextCurrent(pWindow);

    // иницилизируем библиотеку glad
    if(!gladLoadGL()) {
	    std::cerr << "Can't load GLAD" << std::endl;
	    return -1;
    }

    // печатаем информацию о версии и рендеринге OpenGL
    std::cout << "Render: " << RendererEngine::Renderer::getRendererStr() << std::endl;
    std::cout << "OpenGL version: " << RendererEngine::Renderer::getVersionStr() << std::endl;
    //std::cout << "OpenGL " << GLVersion.major << "." << GLVersion.minor << std::endl;

    RendererEngine::Renderer::setClearColor(0, 0, 0, 1);
    RendererEngine::Renderer::setDepthTest(true);
    
    {   // создаём отдельную область видимости, так как нам нужно, чтобы менеджер ресурсов освободил ресурсы до 
        // вызова glfwTerminate, который закроет контекст OpenGL
        ResourceManager::setExacutablePath(argv[0]);
        Physics::PhysicsEngine::init();
        g_game->init();
        
        // устанавливаем размер окна
        //glfwSetWindowSize(pWindow, static_cast<int>(g_game->getCurrentWidth()), static_cast<int>(g_game->getCurrentHeight()));

        // последнее время отсчёта
        auto lastTime = std::chrono::high_resolution_clock::now();
        
        // цикл до закрытия окна пользователем
        while (!glfwWindowShouldClose(pWindow))
        {
            // обработка событий окна(ввод пользователя изменения размеров окна и т.д.)
            glfwPollEvents();

            // текущее время
            auto currentTime = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double, std::milli>(currentTime - lastTime).count();
            lastTime = currentTime;
            g_game->update(duration);
            Physics::PhysicsEngine::update(duration);

            RendererEngine::Renderer::clear();
            g_game->render();

            // поменять местами передний и задний буферы
            glfwSwapBuffers(pWindow);
        }
    }
    g_game = nullptr;
    ResourceManager::unloadAllResources();

    glfwTerminate();  // освобаждаем ресурсы
    return 0;
}
