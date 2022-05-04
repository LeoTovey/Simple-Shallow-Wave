#include <stdio.h>
#include <iostream>
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "shader.h"
#include "water_surface.h"
#include "ray.h"
#include "cube.h"
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
static void glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

WaterSurface *water_surface_ptr = nullptr;
Cube* cube_ptr = nullptr;
glm::mat4 *mat = nullptr;
glm::vec3 camera_pos(0.0f, 2.0f, 2.0f);
static float height = 0.018f;
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    double x = 0;
    double y = 0;
    glfwGetCursorPos(window, &x, &y);
    Ray ray = Ray::ScreenPointToRay(*mat, glm::vec2(SCR_WIDTH, SCR_HEIGHT), glm::vec2(x, SCR_HEIGHT - y), camera_pos);
      
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        std::cout << "mouse pressed"
                  << " " << x << " " << y << std::endl;
        glm::vec3 hit_point;
        if (water_surface_ptr->Hit(ray, hit_point))
        {
            std::cout << hit_point.x << " " << hit_point.y << " " << hit_point.z << std::endl;
            water_surface_ptr->CreateRipples(hit_point, height);
        }
    }

}

int main(int, char **)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Simple Shallow Wave Model", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    int pool_size = 100;
    WaterSurface water_surface(pool_size, 2.0f);
    Cube cube(glm::vec3(0.0f, 0.02f, 0), glm::vec3(0.1f));
    water_surface_ptr = &water_surface;
    cube_ptr = &cube;
    mat = new glm::mat4(1.0f);

    // build and compile our shader zprogram
    // ------------------------------------
//    Shader water_shader("/home/leo/simple_shallow_wave_model/shader/pipeline_test.vs", "/home/leo/simple_shallow_wave_model/shader/pipeline_test.fs");
//    Shader height_map_shader("/home/leo/simple_shallow_wave_model/shader/height_map.vs", "/home/leo/simple_shallow_wave_model/shader/height_map.fs");
//    Shader cube_shader("/home/leo/simple_shallow_wave_model/shader/cube.vs", "/home/leo/simple_shallow_wave_model/shader/cube.fs");

    Shader water_shader("../../shader/pipeline_test.vs", "../../shader/pipeline_test.fs");
    Shader height_map_shader("/home/leo/simple_shallow_wave_model/shader/height_map.vs", "/home/leo/simple_shallow_wave_model/shader/height_map.fs");
    Shader cube_shader("../../shader/cube.vs", "../../shader/cube.fs");

    // water surface
    // -----------------------------
    float *water_surface_vertices = water_surface.GetVertices();
    unsigned int *water_surface_indices = water_surface.GetIndices();
    int water_surface_triangle_count = water_surface.GetTriangleCount();
    int water_surface_vertices_count = water_surface.GetVerticesCount();

    unsigned int water_VBO, water_VAO, water_EBO;
    glGenVertexArrays(1, &water_VAO);
    glGenBuffers(1, &water_VBO);
    glGenBuffers(1, &water_EBO);

    glBindVertexArray(water_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, water_VBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * water_surface_vertices_count * sizeof(float), water_surface_vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, water_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * water_surface_triangle_count * sizeof(unsigned int), water_surface_indices, GL_STATIC_DRAW);

    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);

    // normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));

    glBindVertexArray(0);


    // height map
    // -----------------------
    // unsigned int height_map;
    // unsigned char height_map_data[pool_size * pool_size * 3];

    // glGenTextures(1, &height_map);
    // glBindTexture(GL_TEXTURE_2D, height_map);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // for (int i = 0; i < pool_size * pool_size * 3; i++)
    // {
    //     height_map_data[i] = rand() * 255;
    // }

    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pool_size, pool_size, 0, GL_RGB, GL_UNSIGNED_BYTE, height_map_data);

    // cube
    // --------------------------------------
    float* cube_vertices = cube.GetVertices();
    int cube_vertices_count = cube.GetVerticesCount();

    unsigned int cube_VBO, cube_VAO;
    glGenVertexArrays(1, &cube_VAO);
    glGenBuffers(1, &cube_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * cube_vertices_count * sizeof(float), cube_vertices, GL_STATIC_DRAW);

    glBindVertexArray(cube_VAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // height map
    // -----------------------

    // float height_map_vertices[] =
    //     {
    //         -1.0f, -1.0f, 0.0f, 0.0f,
    //         -1.0f, -0.5f, 0.0f, 1.0f,
    //         -0.5f, -1.0f, 1.0f, 0.0f,
    //         -1.0f, -0.5f, 0.0f, 1.0f,
    //         -0.5f, -0.5f, 1.0f, 1.0f,
    //         -0.5f, -1.0f, 1.0f, 0.0f};
    // unsigned int height_map_VBO, height_map_VAO;
    // glGenVertexArrays(1, &height_map_VAO);
    // glGenBuffers(1, &height_map_VBO);

    // glBindVertexArray(height_map_VAO);
    // glBindBuffer(GL_ARRAY_BUFFER, height_map_VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(height_map_vertices), &height_map_vertices, GL_STATIC_DRAW);

    // // position attribute
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);

    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));



    // height_map_shader.use();
    // height_map_shader.setInt("height_map", 0);
    //glBindVertexArray(0);

    // framebuffer configuration
    // -------------------------
    // unsigned int framebuffer;
    // glGenFramebuffers(1, &framebuffer);
    // glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // // create a color attachment texture
    // unsigned int height_map_buffer[3];
    // int buffer_index = 1;
    // glGenTextures(3, height_map_buffer);
    // for (int i = 0; i < 3; i++)
    // {
    //     glBindTexture(GL_TEXTURE_2D, height_map_buffer[i]);
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pool_size, pool_size, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // }

    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, height_map_buffer[buffer_index], 0);
    // // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    // unsigned int rbo;
    // glGenRenderbuffers(1, &rbo);
    // glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    // if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //     std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // MVP matrix
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        view = glm::lookAt(camera_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        *mat = projection * view * model;
        // light
        static glm::vec3 light_color(1.0f, 1.0f, 1.0f);
        static glm::vec3 light_pos(0.0f, 5.0f, 7.0f);

        // render water
        water_shader.use();
        water_shader.setVec3("objectColor", 11.0f / 255.0f, 45.0f / 255.0f, 100.0f / 255.0f);
        water_shader.setVec3("lightColor", light_color);
        water_shader.setVec3("lightPos", light_pos);
        water_shader.setVec3("viewPos", camera_pos);
        water_shader.setMat4("model", model);
        water_shader.setMat4("view", view);
        water_shader.setMat4("projection", projection);
        
        glBindVertexArray(water_VAO);
        glDrawElements(GL_TRIANGLES, water_surface_triangle_count * 3, GL_UNSIGNED_INT, 0);

        water_surface.Update(cube);
        glBindBuffer(GL_ARRAY_BUFFER, water_VBO);
        glBufferData(GL_ARRAY_BUFFER, 6 * water_surface_vertices_count * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 6 * water_surface_vertices_count * sizeof(float), water_surface_vertices);

        // render cube
        cube_shader.use();
        cube_shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        cube_shader.setVec3("lightColor", light_color);
        cube_shader.setVec3("lightPos", light_pos);
        cube_shader.setVec3("viewPos", camera_pos);
        cube_shader.setMat4("model", model);
        cube_shader.setMat4("view", view);
        cube_shader.setMat4("projection", projection);
        glBindVertexArray(cube_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);
        glBufferData(GL_ARRAY_BUFFER, 6 * cube_vertices_count * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 6 * cube_vertices_count * sizeof(float), cube_vertices);

        // show height map
        // height_map_shader.use();
        // glBindTexture(GL_TEXTURE_2D, height_map);
        // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, pool_size, pool_size, GL_RGB, GL_UNSIGNED_BYTE, height_map_data);
        // glBindVertexArray(height_map_VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 6);


        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Shallow Wave Model"); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        // ImGui::SetWindowFontScale(2.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        //ImGui::Image((void *)(intptr_t)height_map, ImVec2(pool_size * 2, pool_size * 2));

        ImGui::SliderFloat("Ripples Height", &height, 0.005f, 0.02f);
        ImGui::SliderFloat("Light X", &light_pos.x, 0.0f, 10.0f);
        ImGui::SliderFloat("Light Y", &light_pos.y, 0.0f, 10.0f);
        ImGui::SliderFloat("Light Z", &light_pos.z, 0.0f, 10.0f);
        if (ImGui::Button("Random Ripples"))
        {
            water_surface.CreateRandomRipples(height);
            // glm::vec4 world_point(-1.0f, 0.0f, 1.0f, 1.0f);
            // glm::vec4 point = projection * view * model * world_point;
            // std::cout<<point.x / point.w<< ", "<<point.y / point.w<< ", "<<point.z / point.w<<", "<<point.w<<std::endl;
        }

        if (ImGui::Button("Cube Move"))
        {
            std::cout<<"move"<<std::endl;
            cube.SetPosition(cube.GetPosition()+glm::vec3(0.1f));
        }
        // ImGui::Checkbox("start", &simulating);
        ImGui::End();

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();

        // update
        // if (simulating)
        // {
        //     water_surface.Update(height_map_data);
        // }
    }

    glDeleteVertexArrays(1, &water_VAO);
    glDeleteBuffers(1, &water_VBO);
    glDeleteBuffers(1, &water_EBO);

    glDeleteVertexArrays(1, &cube_VAO);
    glDeleteBuffers(1, &cube_VBO);

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    static bool moving = false;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    

    
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        double x = 0;
        double y = 0;
        glfwGetCursorPos(window, &x, &y);
        Ray ray = Ray::ScreenPointToRay(*mat, glm::vec2(SCR_WIDTH, SCR_HEIGHT), glm::vec2(x, SCR_HEIGHT - y), camera_pos);
                glm::vec3 hit_point;
        if (moving)
        {
            glm::vec3 surface_hit_point;
            if(water_surface_ptr->Hit(ray, surface_hit_point))
            {
                cube_ptr->SetPosition(surface_hit_point);
            }
        }

        if (cube_ptr->Hit(ray, hit_point))
        {
            moving = true;
        }
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        moving = false;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
