cmake_minimum_required(VERSION 3.0.0)
project(SHALLOWWAVE VERSION 0.1.0)
include_directories(
        ${PROJECT_SOURCE_DIR}/deps/glad/include/
        ${PROJECT_SOURCE_DIR}/deps/glfw/include/
        ${PROJECT_SOURCE_DIR}/deps/glm/
        ${PROJECT_SOURCE_DIR}/deps/imgui/
        ${PROJECT_SOURCE_DIR}/deps/
        ${PROJECT_SOURCE_DIR}/deps/stb_image/
)
add_subdirectory(${PROJECT_SOURCE_DIR}/deps/imgui)
add_subdirectory(${PROJECT_SOURCE_DIR}/deps/glfw)
add_subdirectory(${PROJECT_SOURCE_DIR}/deps/glm)
add_subdirectory(src)