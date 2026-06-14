cmake_minimum_required(VERSION 3.20)
project(BlindBoxBattle)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(GLUT   REQUIRED)

add_executable(BlindBoxBattle
    main.cpp
    Character.cpp
    Wolverine.cpp
    Deadpool.cpp
    Arena.cpp
    Particles.cpp
    HUD.cpp
    Game.cpp
)

target_include_directories(BlindBoxBattle PRIVATE ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
# GLU (gluPerspective/gluLookAt/gluOrtho2D) is bundled in the OpenGL framework
# on macOS but is a separate library (glu32) on Windows/Linux.
target_link_libraries(BlindBoxBattle OpenGL::GL OpenGL::GLU GLUT::GLUT)

# Suppress the common warning about gluPerspective/gluLookAt being deprecated on macOS
if(APPLE)
    target_compile_definitions(BlindBoxBattle PRIVATE GL_SILENCE_DEPRECATION)
endif()
