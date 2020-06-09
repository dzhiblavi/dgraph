cmake_minimum_required(VERSION 3.15)
project(dgraph)

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
set(CMAKE_CXX_STANDARD 17)

link_directories(/usr/local/lib)
include_directories(/usr/local/include)

find_package(GLEW   REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT   REQUIRED)
find_package(GLM    REQUIRED)

include_directories(
        ${OPENGL_INCLUDE_DIRS}
        ${GLUT_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        ${GLM_INCLUDE_DIRS})

include_directories(.)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -D_GCXX_DEBUG")

set(LIB_SRC ${PROJECT_SOURCE_DIR}/lib/stb/stb_image.h
            ${PROJECT_SOURCE_DIR}/lib/json/json.hpp)

set(DGL_SRC ${PROJECT_SOURCE_DIR}/dgl/Shader.h
            ${PROJECT_SOURCE_DIR}/dgl/Shader.cpp
            ${PROJECT_SOURCE_DIR}/dgl/Texture.h
            ${PROJECT_SOURCE_DIR}/dgl/Texture.cpp
            ${PROJECT_SOURCE_DIR}/dgl/Context.h
            ${PROJECT_SOURCE_DIR}/dgl/Context.cpp
            ${PROJECT_SOURCE_DIR}/dgl/error.h
            ${PROJECT_SOURCE_DIR}/dgl/error.cpp
            ${PROJECT_SOURCE_DIR}/dgl/buffers.h
            ${PROJECT_SOURCE_DIR}/dgl/buffers.cpp
            ${PROJECT_SOURCE_DIR}/dgl/FpsCamera.h
            ${PROJECT_SOURCE_DIR}/dgl/FpsCamera.cpp)

set(UTIL_FILES ${PROJECT_SOURCE_DIR}/util/logger.h
               ${PROJECT_SOURCE_DIR}/util/logger.cpp
               ${PROJECT_SOURCE_DIR}/util/cargs.h
               ${PROJECT_SOURCE_DIR}/util/cargs.cpp
               ${PROJECT_SOURCE_DIR}/util/intrusive_list.hpp
               ${PROJECT_SOURCE_DIR}/util/cb_signal.h
               ${PROJECT_SOURCE_DIR}/util/timer.h
               ${PROJECT_SOURCE_DIR}/util/timer.cpp)

add_executable(dgraph 
            ${PROJECT_SOURCE_DIR}/main.cpp
            ${PROJECT_SOURCE_DIR}/app_window.h
            ${PROJECT_SOURCE_DIR}/app_window.cpp
            ${LIB_SRC}
            ${DGL_SRC}
            ${UTIL_FILES})

target_link_libraries(dgraph ${OPENGL_LIBRARIES}
                             ${GLUT_LIBRARY}
                             ${GLEW_LIBRARIES}
                             glfw glew)

