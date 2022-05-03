// #include <stdio.h>
// #include <iostream>
// #include <glad/glad.h>
// #include <GLFW/glfw3.h> 

// #include "imgui/imgui.h"
// #include "imgui/imgui_impl_glfw.h"
// #include "imgui/imgui_impl_opengl3.h"

// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"

// #include "glm/glm.hpp"
// #include "glm/gtc/matrix_transform.hpp"
// #include "glm/gtc/type_ptr.hpp"
// #include "shader.h"
// #include "water_surface.h"

// void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// void processInput(GLFWwindow *window);
// static void glfw_error_callback(int error, const char* description)
// {
//     fprintf(stderr, "Glfw Error %d: %s\n", error, description);
// }

// // settings
// const unsigned int SCR_WIDTH = 1280;
// const unsigned int SCR_HEIGHT = 720;

// int simulate(int, char**)
// {
//     // Setup window
//     glfwSetErrorCallback(glfw_error_callback);
//     if (!glfwInit())
//         return 1;

//     const char* glsl_version = "#version 130";
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);


//     // Create window with graphics context
//     GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Simple Shallow Wave Model", NULL, NULL);
//     if (window == NULL)
//         return 1;
//     glfwMakeContextCurrent(window);
//     glfwSwapInterval(1); // Enable vsync
//     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

//     // glad: load all OpenGL function pointers
//     // ---------------------------------------
//     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//     {
//         std::cout << "Failed to initialize GLAD" << std::endl;
//         return -1;
//     }

//     // Setup Dear ImGui context
//     IMGUI_CHECKVERSION();
//     ImGui::CreateContext();
//     ImGuiIO& io = ImGui::GetIO(); (void)io;

//     // Setup Dear ImGui style
//     ImGui::StyleColorsDark();

//     // Setup Platform/Renderer backends
//     ImGui_ImplGlfw_InitForOpenGL(window, true);
//     ImGui_ImplOpenGL3_Init(glsl_version);

//     // Our state
//     bool show_demo_window = true;
//     bool show_another_window = false;
//     ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    
//     // build and compile our shader zprogram
//     // ------------------------------------
//     Shader ourShader("/home/leo/simple_shallow_wave_model/pipeline_test.vs", "/home/leo/simple_shallow_wave_model/pipeline_test.fs");

//     int pool_size = 100;
//     WaterSurface wave_surface(pool_size);

//     // set up vertex data (and buffer(s)) and configure vertex attributes
//     // ------------------------------------------------------------------
//     float* vertices = wave_surface.GetVertices();
//     unsigned int* indices = wave_surface.GetIndices();
//     int triangle_count = wave_surface.GetTriangleCount();
//     int vertices_count = wave_surface.GetVerticesCount();

//     unsigned int VBO, VAO, EBO;
//     glGenVertexArrays(1, &VAO);
//     glGenBuffers(1, &VBO);
//     glGenBuffers(1, &EBO);

//     glBindVertexArray(VAO);

//     glBindBuffer(GL_ARRAY_BUFFER, VBO);
//     glBufferData(GL_ARRAY_BUFFER, 6 * vertices_count * sizeof(float), vertices, GL_DYNAMIC_DRAW);

//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * triangle_count * sizeof(unsigned int), indices, GL_STATIC_DRAW);

//     // position attribute
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//     glEnableVertexAttribArray(0);
//     // normal attribute
//     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//     glEnableVertexAttribArray(1);

//     // height map
//     unsigned int height_map;
//     unsigned char height_map_data[pool_size * pool_size * 3];

//     glGenTextures(1, &height_map);
//     glBindTexture(GL_TEXTURE_2D, height_map);

//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


//     for (int i = 0; i < pool_size * pool_size * 3; i++ )
//     {
//         height_map_data[i] = rand() * 255;
//     }

//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pool_size, pool_size, 0, GL_RGB, GL_UNSIGNED_BYTE, height_map_data);

//     // Main loop
//     while (!glfwWindowShouldClose(window))
//     {


//         // input
//         // -----
//         processInput(window);

//         // render
//         // ------
//         glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//         // activate shader
//         ourShader.use();

//         static float light_x = 0.0f, light_y = 5.0f, light_z = 7.0f;
//         static bool simulating = true;
//         ourShader.setVec3("objectColor", 11.0f/255.0f, 45.0f/255.0f, 100.0f/255.0f);
//         ourShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
//         ourShader.setVec3("lightPos", glm::vec3(light_x, light_y, light_z));
//         ourShader.setVec3("viewPos", glm::vec3(0.0f, 1.0f, 3.0f));

//         // create transformations
//         glm::mat4 model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
//         glm::mat4 view          = glm::mat4(1.0f);
//         glm::mat4 projection    = glm::mat4(1.0f);
//         view = glm::lookAt(glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//         projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

//         // retrieve the matrix uniform locations
//         unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
//         unsigned int viewLoc  = glGetUniformLocation(ourShader.ID, "view");

//         // pass them to the shaders (3 different ways)
//         glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//         glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

//         // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
//         ourShader.setMat4("projection", projection);

//         // render container
//         glBindVertexArray(VAO);
//         glBufferData(GL_ARRAY_BUFFER, 6 * vertices_count * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
//         glBufferSubData(GL_ARRAY_BUFFER, 0, 6 * vertices_count * sizeof(float), vertices);
//         glDrawElements(GL_TRIANGLES, triangle_count * 3, GL_UNSIGNED_INT, 0);
        
//         // Start the Dear ImGui frame
//         ImGui_ImplOpenGL3_NewFrame();
//         ImGui_ImplGlfw_NewFrame();
//         ImGui::NewFrame();

//         static float height = 0.018f;
//         ImGui::Begin("Shallow Wave Model");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
//         //ImGui::SetWindowFontScale(2.0f);
//         ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
//         ImGui::Image((void *) (intptr_t) height_map, ImVec2(pool_size * 2, pool_size * 2));

//         ImGui::SliderFloat("Ripples Height", &height, 0.005f, 0.02f);
//         ImGui::SliderFloat("Light X", &light_x, 0.0f, 10.0f);
//         ImGui::SliderFloat("Light Y", &light_y, 0.0f, 10.0f);
//         ImGui::SliderFloat("Light Z", &light_z, 0.0f, 10.0f);
//         if (ImGui::Button("Random Ripples"))
//         {
//             wave_surface.CreateRandomRipples(height);
//         }
//         ImGui::Checkbox("start", &simulating);
//         ImGui::End();

//         // Rendering
//         ImGui::Render();
//         ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

//         // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
//         // -------------------------------------------------------------------------------
//         glfwSwapBuffers(window);
//         glfwPollEvents();

//         // update
//         if (simulating)
//         {
//             wave_surface.Update(height_map_data);
//             glBindTexture(GL_TEXTURE_2D, height_map);
//             glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, pool_size, pool_size, GL_RGB, GL_UNSIGNED_BYTE, height_map_data);        
//         }
//     }

//     // Cleanup
//     ImGui_ImplOpenGL3_Shutdown();
//     ImGui_ImplGlfw_Shutdown();
//     ImGui::DestroyContext();

//     glfwDestroyWindow(window);
//     glfwTerminate();

//     return 0;
// }

// // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// // ---------------------------------------------------------------------------------------------------------
// void processInput(GLFWwindow *window)
// {
//     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//         glfwSetWindowShouldClose(window, true);
// }

// // glfw: whenever the window size changed (by OS or user resize) this callback function executes
// // ---------------------------------------------------------------------------------------------
// void framebuffer_size_callback(GLFWwindow* window, int width, int height)
// {
//     // make sure the viewport matches the new window dimensions; note that width and 
//     // height will be significantly larger than specified on retina displays.
//     glViewport(0, 0, width, height);
// }



#include <stdio.h>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h> 

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "shader.h"
#include "water_surface.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);


    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Simple Shallow Wave Model", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    
    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("/home/leo/simple_shallow_wave_model/pipeline_test.vs", "/home/leo/simple_shallow_wave_model/pipeline_test.fs");
    Shader height_map_shader("/home/leo/simple_shallow_wave_model/height_map.vs", "/home/leo/simple_shallow_wave_model/height_map.fs");

    int pool_size = 100;
    WaterSurface wave_surface(pool_size);

        //show height map
    float height_map_vertices[] = {
        -1.0f, -1.0f, 0.00f,
         1.0f, -1.0f, 0.00f,
        -1.0f,  1.0f, 0.00f,

        -1.0f, -1.0f, 0.00f,
         1.0f, -1.0f, 0.00f,
        -1.0f,  1.0f, 0.00f,
    };
    unsigned int height_map_VBO, height_map_VAO;
    glGenVertexArrays(1, &height_map_VAO);
    glGenBuffers(1, &height_map_VBO);

    glBindVertexArray(height_map_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, height_map_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(height_map_vertices), &height_map_vertices, GL_STATIC_DRAW);

    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float* vertices = wave_surface.GetVertices();
    unsigned int* indices = wave_surface.GetIndices();
    int triangle_count = wave_surface.GetTriangleCount();
    int vertices_count = wave_surface.GetVerticesCount();

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * vertices_count * sizeof(float), vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * triangle_count * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    
    // normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    

    // height map
    unsigned int height_map;
    unsigned char height_map_data[pool_size * pool_size * 3];

    glGenTextures(1, &height_map);
    glBindTexture(GL_TEXTURE_2D, height_map);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    for (int i = 0; i < pool_size * pool_size * 3; i++ )
    {
        height_map_data[i] = rand() * 255;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pool_size, pool_size, 0, GL_RGB, GL_UNSIGNED_BYTE, height_map_data);


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

        // activate shader
        ourShader.use();

        static float light_x = 0.0f, light_y = 5.0f, light_z = 7.0f;
        static bool simulating = true;
        ourShader.setVec3("objectColor", 11.0f/255.0f, 45.0f/255.0f, 100.0f/255.0f);
        ourShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("lightPos", glm::vec3(light_x, light_y, light_z));
        ourShader.setVec3("viewPos", glm::vec3(0.0f, 1.0f, 3.0f));

        // create transformations
        glm::mat4 model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);
        view = glm::lookAt(glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // retrieve the matrix uniform locations
        unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        unsigned int viewLoc  = glGetUniformLocation(ourShader.ID, "view");

        // pass them to the shaders (3 different ways)
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

        // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        ourShader.setMat4("projection", projection);

        // render container
        glBindVertexArray(VAO);
        glBufferData(GL_ARRAY_BUFFER, 6 * vertices_count * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 6 * vertices_count * sizeof(float), vertices);
        glDrawElements(GL_TRIANGLES, triangle_count * 3, GL_UNSIGNED_INT, 0);


        height_map_shader.use();
        glBindVertexArray(height_map_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static float height = 0.018f;
        ImGui::Begin("Shallow Wave Model");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        //ImGui::SetWindowFontScale(2.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Image((void *) (intptr_t) height_map, ImVec2(pool_size * 2, pool_size * 2));

        ImGui::SliderFloat("Ripples Height", &height, 0.005f, 0.02f);
        ImGui::SliderFloat("Light X", &light_x, 0.0f, 10.0f);
        ImGui::SliderFloat("Light Y", &light_y, 0.0f, 10.0f);
        ImGui::SliderFloat("Light Z", &light_z, 0.0f, 10.0f);
        if (ImGui::Button("Random Ripples"))
        {
            wave_surface.CreateRandomRipples(height);
        }
        ImGui::Checkbox("start", &simulating);
        ImGui::End();

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();

        // update
        if (simulating)
        {
            wave_surface.Update(height_map_data);
            glBindTexture(GL_TEXTURE_2D, height_map);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, pool_size, pool_size, GL_RGB, GL_UNSIGNED_BYTE, height_map_data);        
        }

    }

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
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

