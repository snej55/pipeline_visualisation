cmake_minimum_required(VERSION 3.10...3.31)
project(main VERSION 0.2 
        DESCRIPTION "AI Pipeline Visualisation"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

#set(CMAKE_CXX_FLAGS "-Wall -Wextra")
#set(CMAKE_CXX_FLAGS_DEBUG "-g")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include(CMakePrintHelpers)
cmake_print_variables(CMAKE_BUILD_TYPE)

set(CMAKE_SYSTEM_NAME Windows)

if (CMAKE_SYSTEM MATCHES Linux)
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath lib/linux -lassimp -lGL -lGLU -lfreetype -lglfw3")
endif()

set(GL_LIBS glfw3 assimp freetype)

if (CMAKE_SYSTEM MATCHES Windows)
    message(STATUS "Target system is Windows")
    set(FREETYPE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include/windows/freetype)
    message(STATUS "Found Freetype in ${FREETYPE_INCLUDE_DIRS}")
    # link/include for Windows x86
    include_directories(${CMAKE_SOURCE_DIR}/include/windows)
    include_directories(${CMAKE_SOURCE_DIR}/include/windows/freetype)
    link_directories(${CMAKE_SOURCE_DIR}/lib/windows)
elseif (CMAKE_SYSTEM MATCHES Linux)
    message(STATUS "Target system is Linux")
    # link/include for linux x86
    # required packages:
    # mesa-libGL-devel mesa-libGLU-devel minizip freetype-devel bzip2
    # for issues loading shared library bzip2 try:
    # sudo ln -s /usr/lib64/libbz2.so.1.0.8 /usr/lib64/libbz2.so.1.0
    include_directories(${CMAKE_SOURCE_DIR}/include/linux ${CMAKE_SOURCE_DIR}/src/extern/glad)
    link_directories(${CMAKE_SOURCE_DIR}/lib/linux)
    set(GL_LIBS GL GLU glfw3 assimp freetype)
endif()

set(SOURCES main.cpp
        # opengl
        src/extern/glad.c
        src/opengl/app.cpp
        src/opengl/app.h
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
        src/opengl/fonts.cpp
        
        src/paper_loader.h
        src/paper_loader.cpp
        src/clusters.h
        src/clusters.cpp
        src/bar_chart.h
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

# add_custom_target(copy_libs
#         COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/libs/linux ${CMAKE_CURRENT_BINARY_DIR}/libs
# )
# add_dependencies(${PROJECT_NAME} copy_libs)