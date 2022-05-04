include(CTest)
enable_testing()

IF (WIN32)
    find_package(OpenGL REQUIRED)
ENDIF ()

add_executable(${PROJECT_NAME} main.cpp glad.c water_surface.cpp box_collider.cpp ray.cpp cube.cpp)

IF (WIN32)
	MESSAGE(STATUS "Now is windows")
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARY} imgui glm glfw)
ELSEIF (APPLE)
	MESSAGE(STATUS "Now is Apple systens.")
ELSEIF (UNIX)
	MESSAGE(STATUS "Now is UNIX-like OS's.")
    target_link_libraries(${PROJECT_NAME} imgui glm glfw GL X11 pthread Xrandr Xi dl)
ENDIF ()


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
