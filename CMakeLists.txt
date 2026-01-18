cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(PROJECT_NAME BattleCity)
project(${PROJECT_NAME})

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_executable(${PROJECT_NAME} 
	src/main.cpp
	src/Renderer/ShaderProgram.cpp
	src/Renderer/ShaderProgram.h
	src/Resource/ResourceManager.cpp
	src/Resource/ResourceManager.h
)

# стандарт C++
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# подключаем GLFW
add_subdirectory(external/glfw)
target_link_libraries(${PROJECT_NAME} glfw)

# подключаем GLAD
add_subdirectory(external/glad)
target_link_libraries(${PROJECT_NAME} glad)

# указываем куда сохранять бинарные файлы
# CMAKE_BINARY_DIR - путь до директории сборки
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

# указываем скопировать все ресурсы в папку с собранной программой
# CMAKE_COMMAND - путь к программе cmake
# $<TARGET_FILE_DIR:${PROJECT_NAME}> указывает на директорию сборки
# CMAKE_SOURCE_DIR - путь до директории с главным файлом CMakeLists
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory 
	${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)
