#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <chrono>

#include "Renderer/ShaderProgram.h"
#include "Renderer/Texture2D.h"
#include "Resource/ResourceManager.h"
#include "Renderer/Sprite.h"
#include "Renderer/AnimatedSprite.h"

GLfloat point[] = {
     0.0f,   50.f, 0.0f,
     50.f,  -50.f, 0.0f,
    -50.f,  -50.f, 0.0f
};

GLfloat colors[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
};

GLfloat texCoord[] = {
    0.5f, 1.0f, 
    1.0f, 0.0f, 
    0.0f, 0.0f, 
};

glm::ivec2 g_windowSize(640, 480);

bool isEagle = false;

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
    } else if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
        isEagle = !isEagle;
    }
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
    glClearColor(1, 1, 0, 1);

    {   // создаём отдельную область видимости, так как нам нужно, чтобы менеджер ресурсов освободил ресурсы до 
        // вызова glfwTerminate, который закроет контекст OpenGL
        ResourceManager resourceManager(argv[0]);
        auto pDefaultShaderProgram = resourceManager.loadShaderProgram("DefaultShader", "res/shaders/vertex.txt", "res/shaders/fragment.txt");
        if (!pDefaultShaderProgram) {
            std::cerr << "Can't create shader program: " << "DefaultShader" << std::endl;
            return -1;
        }

        auto pSpriteShaderProgram = resourceManager.loadShaderProgram("SpriteShader", "res/shaders/vSprite.txt", "res/shaders/fSprite.txt");
        if (!pSpriteShaderProgram) {
            std::cerr << "Can't create shader program: " << "SpriteShader" << std::endl;
            return -1;
        }
        
        auto tex = resourceManager.loadTexture("DefaultTexture", "res/textures/map_16x16.png");

        std::vector<std::string> subTexturesNames = {
             "block",
            "topBlock",
            "bottomBlock",
            "leftBlock",
            "rightBlock",
            "topLeftBlock",
            "topRightBlock",
            "bottomLeftBlock",

            "bottomRightBlock",
            "beton",
            "topBeton",
            "bottomBeton",
            "leftBeton",
            "rightBeton",
            "topLeftBeton",
            "topRightBeton",

            "bottomLeftBeton",
            "bottomRightBeton",
            "water1",
            "water2",
            "water3",
            "trees",
            "ice",
            "wall",

            "eagle",
            "deadEagle",
            "nothing",
            "respawn1",
            "respawn2",
            "respawn3",
            "respawn4"
        };
        auto pTextureAtlas = resourceManager.loadTextureAtlas("DefaultTextureAtlas", "res/textures/map_16x16.png", 
                        std::move(subTexturesNames), 16, 16);

        auto pSprite = resourceManager.loadSprite("NewSprite", "DefaultTextureAtlas", "SpriteShader", 100, 100, "beton");
        if (!pSprite) {
            std::cerr << "Can't create sprite: " << "NewSprite" << std::endl;
            return -1;
        }
        pSprite->setPosition(glm::vec2(300, 100));

        auto pAnimatedSprite = resourceManager.loadAnimatedSprite("NewAnimatedSprite", "DefaultTextureAtlas", 
                            "SpriteShader", 100, 100, "beton");
        pAnimatedSprite->setPosition(glm::vec2(300, 300));

        std::vector<std::pair<std::string, uint64_t>> waterState;
        waterState.emplace_back(std::make_pair<std::string, uint64_t>("water1", 1000000000));
        waterState.emplace_back(std::make_pair<std::string, uint64_t>("water2", 1000000000));
        waterState.emplace_back(std::make_pair<std::string, uint64_t>("water3", 1000000000));

        std::vector<std::pair<std::string, uint64_t>> eagleState;
        eagleState.emplace_back(std::make_pair<std::string, uint64_t>("eagle", 1000000000));
        eagleState.emplace_back(std::make_pair<std::string, uint64_t>("deadEagle", 1000000000));

        pAnimatedSprite->insertState("waterState", std::move(waterState));
        pAnimatedSprite->insertState("eagleState", std::move(eagleState));

        pAnimatedSprite->setState("waterState");

        // сгенерировать один объектный буфер вершин(VBO) для позиций
        GLuint points_vbo = 0;
        glGenBuffers(1, &points_vbo);
        // подключаем его и делаем текущим
        glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
        // указываем массив point в оперативной памяти для VBO, даём подсказку, 
        // что память в массиве point не будет записываться
        // (выполняется для текущего буфера), теперь этот массив есть в памяти GPU
        glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);
        
        // аналогично для буфера цветов
        GLuint colors_vbo = 0;
        glGenBuffers(1, &colors_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

        GLuint texCoord_vbo = 0;
        glGenBuffers(1, &texCoord_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, texCoord_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texCoord), texCoord, GL_STATIC_DRAW);
        
        // создаём 1 объект массива векторов(VAO)
        // при его выборе он выдаёт GPU информацию о всех привязанных VBO и как их читать
        GLuint vao = 0;
        glGenVertexArrays(1, &vao);
        // делаем его текущим
        glBindVertexArray(vao);

        // включить позицию 0 в шейдере(location = 0)
        glEnableVertexAttribArray(0);
        // привязываем буфер points_vbo к позиции 0(указываем брать по 3 элемента из VBO,
        // тип данных, не переводить в относительные координаты, все данные идут в перемешку,
        // и нет смещения в массиве)
        glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        // аналогично с позицией 1
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, texCoord_vbo);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        // устанавливаем перемеенную tex в текстуру из 0-го слота
        pDefaultShaderProgram->use();
        pDefaultShaderProgram->setInt("tex", GL_TEXTURE0);

        // создаём еденичную матрицу
        glm::mat4 modelMatrix_1 = glm::mat4(1.f);
        // перемещаем эту матрицу на вектор (100, 200, 0)
        modelMatrix_1 = glm::translate(modelMatrix_1, glm::vec3(100.f, 200.f, 0.f));

        glm::mat4 modelMatrix_2 = glm::mat4(1.f);
        modelMatrix_2 = glm::translate(modelMatrix_2, glm::vec3(590.f, 200.f, 0.f));

        // матрица перевода в офрографическое отображение камеры 
        glm::mat4 projectionMatrix = glm::ortho(0.f, static_cast<float>(g_windowSize.x), 0.f, static_cast<float>(g_windowSize.y), -100.f, 100.f);

        pDefaultShaderProgram->setMatrix4("projectionMat", projectionMatrix);

        pSpriteShaderProgram->use();
        pSpriteShaderProgram->setInt("tex", GL_TEXTURE0);
        pSpriteShaderProgram->setMatrix4("projectionMat", projectionMatrix);

        // последнее время отсчёта
        auto lastTime = std::chrono::high_resolution_clock::now();

        // цикл до закрытия окна пользователем
        while (!glfwWindowShouldClose(pWindow))
        {
            if (isEagle)
                pAnimatedSprite->setState("eagleState");
            else
                pAnimatedSprite->setState("waterState");

            // текущее время
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto durationTime = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - lastTime);
            uint64_t duration = static_cast<uint64_t>(durationTime.count());
            lastTime = currentTime;

            pAnimatedSprite->update(duration);

            // очистить экран
            glClear(GL_COLOR_BUFFER_BIT);

            // подключаем шейдеры для зарисовки
            pDefaultShaderProgram->use();
            // выбираем нужный VAO(в случае если он 1 можно пропустить)
            glBindVertexArray(vao);
            tex->bind();

            pDefaultShaderProgram->setMatrix4("modelMat", modelMatrix_1);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            pDefaultShaderProgram->setMatrix4("modelMat", modelMatrix_2);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            pSprite->render();
            pAnimatedSprite->render();

            // поменять местами передний и задний буферы
            glfwSwapBuffers(pWindow);

            // обработка событий окна(ввод пользователя изменения размеров окна и т.д.)
            glfwPollEvents();
        }
    }

    glfwTerminate();  // освобаждаем ресурсы
    return 0;
}
