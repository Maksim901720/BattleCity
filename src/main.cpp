#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Renderer/ShaderProgram.h"

GLfloat point[] = {
     0.0f,   0.5f, 0.0f,
     0.5f,  -0.5f, 0.0f,
    -0.5f,  -0.5f, 0.0f
};

GLfloat colors[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
};

// программа вертексного шейдера
// будет вызываться отдельно для каждой вершины
const char* vertex_shader =
"#version 460\n"                                     // используемая версия OpenGL
"layout(location = 0) in vec3 vertex_position;"      // входная позиция вершины
"layout(location = 1) in vec3 vertex_color;"         // входной цвет вершины
"out vec3 color;"                                    
"void main() {"
"   color = vertex_color;"                           // цвет выходной вершины(попадает в фершинный шейдер)
"   gl_Position = vec4(vertex_position, 1.0);"       // позиция выходной вершины
"}";

// программа для фрагментного шейдера
//  будет вызываться для каждой тройки вершин
// и передавать каждый пиксель в этом треугольнике
// с интерполированным цветом
const char* fragment_shader =
"#version 460\n"
"in vec3 color;"                                      // интерполированный цвет пикселя
"out vec4 frag_color;"                                
"void main() {"
"   frag_color = vec4(color, 1.0f);"                  // выходной цвет
"}";

int g_windowWidth = 640,
    g_windowHeight = 480;

// обрабатываем событие изменения окна
void glfwWindowSizeCallback(GLFWwindow* pWindow, int width, int height)
{
    g_windowWidth = width;
    g_windowHeight = height;

    // показываем OpneGL, в какой области окна рисовать
    glViewport(0, 0, g_windowWidth, g_windowHeight);
}

// обрабатываем события нажатия клавиш
void glfwKeyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mode)
{
    // если нажата клвиша esc
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        // устанавливаем флаг закрытия окна в true
        glfwSetWindowShouldClose(pWindow, GL_TRUE);
    }
}

int main(void)
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
    GLFWwindow *pWindow = glfwCreateWindow(g_windowWidth, g_windowHeight, "Battle city", nullptr, nullptr);
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

    std::string vertexShader(vertex_shader);
    std::string fragmentShader(fragment_shader);
    Renderer::ShaderProgram shaderProgram(vertexShader, fragmentShader);
    if (!shaderProgram.isCompiled()) {
        std::cerr << "Can't create shader program" << std::endl;
        return -1;
    }

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

    // цикл до закрытия окна пользователем
    while (!glfwWindowShouldClose(pWindow))  
    {
        // очистить экран
        glClear(GL_COLOR_BUFFER_BIT);

        // подключаем шейдеры для зарисовки
        shaderProgram.use();
        // выбираем нужный VAO(в случае если он 1 можно пропустить)
        glBindVertexArray(vao);
        // рисуем треугольники(3 вершины)
        glDrawArrays(GL_TRIANGLES, 0, 3);;

        // поменять местами передний и задний буферы
        glfwSwapBuffers(pWindow);

        // обработка событий окна(ввод пользователя изменения размеров окна и т.д.)
        glfwPollEvents();
    }

    glfwTerminate();  // освобаждаем ресурсы
    return 0;
}
