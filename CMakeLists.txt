cmake_minimum_required(VERSION 3.15)
project(dgraph)

set(CMAKE_CXX_STANDARD 17)
link_directories(/usr/local/lib)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}
        ${GLUT_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -D_GCXX_DEBUG")

set(SOURCE context.cpp
           context.h)

add_executable(dgraph main.cpp ${SOURCE})

target_link_libraries(dgraph ${OPENGL_LIBRARIES}
                             ${GLUT_LIBRARY}
                             ${GLEW_LIBRARIES}
                             glfw)

