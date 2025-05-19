cmake_minimum_required(VERSION 3.30)
project(main)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_SYSTEM_NAME Windows)

set(GL_LIBS glfw3 assimp freetype)

if (CMAKE_SYSTEM MATCHES Windows)
    message(STATUS "Target system is Windows")
    # link/include for Windows x86
    include_directories(${CMAKE_SOURCE_DIR}/include/windows)
    link_directories(${CMAKE_SOURCE_DIR}/lib/windows)
elseif (CMAKE_SYSTEM MATCHES Linux)
    message(STATUS "Target system is Linux")
    # link/include for linux x86
    # required packages:
    # mesa-libGL-devel mesa-libGLU-devel minizip
    include_directories(${CMAKE_SOURCE_DIR}/include/linux ${CMAKE_SOURCE_DIR}/src/extern/glad)
    link_directories(${CMAKE_SOURCE_DIR}/lib/linux)
    set(GL_LIBS GL GLU glfw3 assimp freetype)
endif()

set(SOURCES main.cpp
        # opengl
        src/opengl/app.cpp
        src/opengl/app.h
        src/extern/glad.c
        src/opengl/shader.cpp
        src/opengl/shader.h
        src/opengl/texture.cpp
        src/opengl/texture.h
        src/opengl/shapes.h
        src/opengl/objectShapes3D.h
        src/opengl/objects.h
        src/opengl/camera.h
        src/opengl/mesh.cpp
        src/opengl/mesh.h
        src/opengl/model.cpp
        src/opengl/model.h
        src/opengl/postprocessing.h
        src/opengl/cubemap.h
        src/opengl/skybox.h
        src/opengl/terrain.h
        src/opengl/terrain.cpp
        src/opengl/util.h
        src/opengl/fonts.h
        
        src/paper_loader.h
        src/paper_loader.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE ${GL_LIBS})

add_custom_target(copy_assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data
)
add_dependencies(${PROJECT_NAME} copy_assets)

add_custom_target(copy_shaders
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/src/shaders ${CMAKE_CURRENT_BINARY_DIR}/shaders
)
add_dependencies(${PROJECT_NAME} copy_shaders)
