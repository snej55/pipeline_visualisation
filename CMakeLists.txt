cmake_minimum_required(VERSION 3.30)
project(main)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_SYSTEM_NAME Windows)

set(GL_LIBS glfw3 assimp)

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
    include_directories(${CMAKE_SOURCE_DIR}/include/linux ${CMAKE_SOURCE_DIR}/src/glad)
    link_directories(${CMAKE_SOURCE_DIR}/lib/linux)
    set(GL_LIBS GL GLU glfw3 assimp)
endif()

set(SOURCES main.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${GL_LIBS})

# add_custom_target(copy_assets
#         COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data
# )
# add_dependencies(${PROJECT_NAME} copy_assets)

# add_custom_target(copy_shaders
#         COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/src/shaders ${CMAKE_CURRENT_BINARY_DIR}/shaders
# )
# add_dependencies(${PROJECT_NAME} copy_shaders)
