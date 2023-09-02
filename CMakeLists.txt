cmake_minimum_required(VERSION 3.10)

project(OpenGL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_cxx_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

set(LIBRARIES_DIR "${CMAKE_SOURCE_DIR}/src/libraries")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/src/include")

set(LINKER_LIBRARIES "${LIBRARIES_DIR}/libGLEW.a;${LIBRARIES_DIR}/libglfw3.a")

file(GLOB SHADER_FILES "${CMAKE_SOURCE_DIR}/src/shaders/*")
file(COPY ${SHADER_FILES} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/shaders/")

add_executable(outputApp.app src/Application.cpp) 

include_directories(${INCLUDE_DIR})
link_directories(${LINKER_LIBRARIES})


target_link_libraries(outputApp.app ${LINKER_LIBRARIES}) 



target_link_libraries(outputApp.app
    "-framework OpenGL"
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
    "-framework CoreFoundation"
    "-framework GLUT"
    )

