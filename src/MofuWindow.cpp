#include "MofuWindow.h"

#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Mesh.h"
#include "Model.h"
#include "Shader.h"

bool MofuWindow::mouse_pressed_ = false;
float MofuWindow::last_x_ = SCR_WIDTH / 2.0f;
float MofuWindow::last_y_ = SCR_HEIGHT / 2.0f;
Camera MofuWindow::camera_ = Camera(glm::vec3(0.0f, 0.0f, 13.0f));

void MofuWindow::ShowWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, WINDOW_NAME, nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            mouse_pressed_ = true;
            double xpos_in = 0.0;
            double ypos_in = 0.0;
            glfwGetCursorPos(window, &xpos_in, &ypos_in);
            float xpos = static_cast<float>(xpos_in);
            float ypos = static_cast<float>(ypos_in);
            last_x_ = xpos;
            last_y_ = ypos;
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
            mouse_pressed_ = false;
        }
    });
    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
        camera_.ProcessMouseScroll(static_cast<float>(yoffset));
    });

    if (glewInit() != GLEW_OK) {
        return;
    }

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    Shader our_shader("..\\..\\..\\..\\shader\\default_shader.vs",
        "..\\..\\..\\..\\shader\\default_shader.fs");

    Model our_model;
    our_model.SetFixedTexturePath("..\\..\\..\\..\\resources\\texture\\car_texture1.png");
    our_model.LoadModel("..\\..\\..\\..\\resources\\object\\car.blend");

    // mesh test
    /*
    auto texture_id = Model::TextureFromFile("test_texture.png", "..\\..\\..\\..\\resources\\texture");
    Mesh our_mesh(
        std::vector<Vertex>{
            Vertex{ glm::vec3{0.5f, 0.5f, 0.0f}, glm::vec3{}, glm::vec2{1.0f, 1.0f} },
            Vertex{ glm::vec3{0.5f, -0.5f, 0.0f}, glm::vec3{}, glm::vec2{1.0f, 0.0f} },
            Vertex{ glm::vec3{-0.5f, -0.5f, 0.0f}, glm::vec3{}, glm::vec2{0.0f, 0.0f} },
            Vertex{ glm::vec3{-0.5f, 0.5f, 0.0f}, glm::vec3{}, glm::vec2{0.0f, 1.0f} },
        },
        std::vector<unsigned int>{
            0, 1, 3,
            1, 2, 3,
        },
        std::vector<Texture>{
            { texture_id, "texture_diffuse", "" },
        }
    );
    */

    while (!glfwWindowShouldClose(window)) {
        float current_time = static_cast<float>(glfwGetTime());
        delta_time_ = (current_time - last_time_) * 10.0f;
        last_time_ = current_time;
        ProcessInput(window);

        if (mouse_pressed_) {
            double xpos_in = 0.0;
            double ypos_in = 0.0;
            glfwGetCursorPos(window, &xpos_in, &ypos_in);
            float xpos = static_cast<float>(xpos_in);
            float ypos = static_cast<float>(ypos_in);
            float xoffset = xpos - last_x_;
            float yoffset = last_y_ - ypos;
            last_x_ = xpos;
            last_y_ = ypos;
            camera_.ProcessMouseMovement(xoffset, yoffset);
        }

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        our_shader.Use();

        glm::mat4 projection = glm::perspective(camera_.Zoom(),
            static_cast<float>(SCR_WIDTH) / SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera_.GetViewMatrix();
        our_shader.SetMat4("projection", projection);
        our_shader.SetMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        our_shader.SetMat4("model", model);
        our_model.Draw(our_shader);
        // our_mesh.Draw(our_shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return;
}

void MofuWindow::ProcessInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera_.ProcessKeyboard(CameraMovement::FORWARD, delta_time_);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera_.ProcessKeyboard(CameraMovement::BACKWARD, delta_time_);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera_.ProcessKeyboard(CameraMovement::LEFT, delta_time_);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera_.ProcessKeyboard(CameraMovement::RIGHT, delta_time_);
    }
}
