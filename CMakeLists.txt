cmake_minimum_required(VERSION 3.10)
project(MeuCubo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4
)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG master
)

FetchContent_MakeAvailable(glfw glm)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/common)
include_directories(${CMAKE_SOURCE_DIR}/include/glad)
include_directories(${glm_SOURCE_DIR})

add_compile_options(-Wno-pragmas)

if(WIN32)
    set(OPENGL_LIBS opengl32)
elseif(APPLE)
    find_library(OpenGL_LIBRARY OpenGL)
    set(OPENGL_LIBS ${OpenGL_LIBRARY})
else()
    find_package(OpenGL REQUIRED)
    set(OPENGL_LIBS ${OPENGL_gl_LIBRARY})
endif()

set(GLAD_C_FILE "${CMAKE_SOURCE_DIR}/common/glad.c")

if (NOT EXISTS ${GLAD_C_FILE})
    message(FATAL_ERROR "glad.c não encontrado em common/")
endif()

add_executable(Cubo src/Cubo.cpp ${GLAD_C_FILE})
target_include_directories(Cubo PRIVATE ${CMAKE_SOURCE_DIR}/include/glad ${glm_SOURCE_DIR})
target_link_libraries(Cubo glfw ${OPENGL_LIBS})
