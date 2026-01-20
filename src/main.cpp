#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include <iostream>
#include <chrono>

#include "Resource/ResourceManager.h"

#include "Game/Game.h"

glm::ivec2 g_windowSize(640, 480);
Game g_game(g_windowSize);

// обрабатываем событие изменения окна
void glfwWindowSizeCallback(GLFWwindow* pWindow, int width, int height)
{
    g_windowSize.x = width;
    g_windowSize.y = height;

    // показываем OpneGL, в какой области окна рисовать
    glViewport(0, 0, g_windowSize.x, g_windowSize.y);
}

// обрабатываем события нажатия клавиш
void glfwKeyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mode)
{
    // если нажата клвиша esc
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        // устанавливаем флаг закрытия окна в true
        glfwSetWindowShouldClose(pWindow, GL_TRUE);
    } 
    g_game.setKey(key, action);
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
    std::cout << "Render: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    //std::cout << "OpenGL " << GLVersion.major << "." << GLVersion.minor << std::endl;

    // установить цвет зарисовки окна при его очищении
    glClearColor(0, 0, 0, 1);

    {   // создаём отдельную область видимости, так как нам нужно, чтобы менеджер ресурсов освободил ресурсы до 
        // вызова glfwTerminate, который закроет контекст OpenGL
        ResourceManager::setExacutablePath(argv[0]);
        g_game.init();

        // последнее время отсчёта
        auto lastTime = std::chrono::high_resolution_clock::now();

        // цикл до закрытия окна пользователем
        while (!glfwWindowShouldClose(pWindow))
        {
            // текущее время
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto durationTime = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - lastTime);
            uint64_t duration = static_cast<uint64_t>(durationTime.count());
            lastTime = currentTime;
            g_game.update(duration);

            // очистить экран
            glClear(GL_COLOR_BUFFER_BIT);

            g_game.render();

            // поменять местами передний и задний буферы
            glfwSwapBuffers(pWindow);

            // обработка событий окна(ввод пользователя изменения размеров окна и т.д.)
            glfwPollEvents();
        }
    }
    ResourceManager::unloadAllResources();

    glfwTerminate();  // освобаждаем ресурсы
    return 0;
}
