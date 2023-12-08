#include "MofuWindow.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include "Mesh.h"
#include "Model.h"
#include "Shader.h"

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

    if (glewInit() != GLEW_OK) {
        return;
    }

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    Shader our_shader("..\\..\\..\\..\\shader\\default_shader.vs",
        "..\\..\\..\\..\\shader\\default_shader.fs");

    // Model our_model("..\\..\\..\\..\\resources\\object\\car.obj");

    // mesh test
    auto texture_id = Model::TextureFromFile("texture.png", "..\\..\\..\\..\\resources\\image");
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

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        our_shader.Use();

        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        our_shader.SetMat4("projection", projection);
        our_shader.SetMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        our_shader.SetMat4("model", model);
        // our_model.Draw(our_shader);
        our_mesh.Draw(our_shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return;
}
