cmake_minimum_required(VERSION 3.0.0)
project(ShallowWave VERSION 0.1.0)

include(CTest)
enable_testing()

set(GLM_SRC glm/glm.hpp
            glm/gtc/matrix_transform.hpp
            glm/gtc/type_ptr.hpp)
set(IMGUI_SRC   imgui/imgui_demo.cpp 
                imgui/imgui_draw.cpp 
                imgui/imgui_impl_glfw.cpp 
                imgui/imgui_impl_opengl3.cpp
                imgui/imgui_tables.cpp 
                imgui/imgui_widgets.cpp 
                imgui/imgui.cpp)
add_executable(ShallowWave main.cpp glad.c ${GLM_SRC} ${IMGUI_SRC}
                water_surface.cpp box_collider.cpp ray.cpp cube.cpp)
target_link_libraries(ShallowWave glfw3 GL X11 pthread Xrandr Xi dl)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
