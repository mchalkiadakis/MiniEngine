#include "MiniEngine.h"

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    // Set OpenGL version to 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create GLFW window
    GLFWwindow* window = glfwCreateWindow(800, 600, "MiniEngine", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        return -1;
    }

    // Set the OpenGL viewport
    glViewport(0, 0, 800, 600);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE); // Optional: show backfaces if triangle is inverted

    // Load shader and texture
    auto shader = std::make_shared<Shader>("Assets/Shaders/basic.vert", "Assets/Shaders/basic.frag");
    auto sandTexture = std::make_shared<Texture>("Assets/Textures/sand.jpg");
    Material material(shader, sandTexture);

    // Vertex data (in world space at z = -1.0)
    std::vector<float> vertices = {
        // Positions          // UVs
         0.0f,  0.5f,  0.0f,   0.5f, 1.0f,  // Top (apex)
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,  // Front-left
         0.5f, -0.5f,  0.5f,   1.0f, 0.0f,  // Front-right
         0.5f, -0.5f, -0.5f,   1.0f, 1.0f,  // Back-right
        -0.5f, -0.5f, -0.5f,   0.0f, 1.0f   // Back-left
    };

    std::vector<unsigned int> indices = {
        // Side faces (4)
        0, 1, 2,  // Front
        0, 2, 3,  // Right
        0, 3, 4,  // Back
        0, 4, 1,  // Left

        // Base (2 triangles)
        1, 4, 3,  // Bottom-left
        1, 3, 2   // Bottom-right
    };
    //Mesh triangleMesh(vertices, indices, sizeof(float) * 5);
    Mesh pyramidMesh(vertices, indices, sizeof(float) * 5);
    // Camera setup
    Camera camera(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);

    camera.SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    camera.SetTarget(glm::vec3(0.0f, 0.0f, -4.0f));

    //glm::mat4 model = glm::mat4(1.0f);
    
    float lastFrame = 0.0f;
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -1.0f));
    // Render loop
    while (!glfwWindowShouldClose(window)) {

        // Time
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        float time = static_cast<float>(glfwGetTime());

        // Input
        bool w = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
        bool s = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
        bool a = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
        bool d = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
        bool q = glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS;
        bool e = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;

        camera.ProcessKeyboard(deltaTime, w, s, a, d, q, e);

        // Clear
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        model = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.0f, 2.0f, 0.0f));
        model = glm::rotate(model, time, glm::vec3(0.0f, 1.0f, 0.0f));

        // Render
        material.Bind();
        shader->Use();
        shader->SetUniformMat4("u_Model", glm::value_ptr(model));
        shader->SetUniformMat4("u_View", glm::value_ptr(camera.GetViewMatrix()));
        shader->SetUniformMat4("u_Projection", glm::value_ptr(camera.GetProjectionMatrix()));
        
       
       
       
        //model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f)); // push it back
        //model = glm::rotate(model, time, glm::vec3(0.0f, 1.0f, 0.0f)); // then rotate
        
        std::cout << "View matrix:\n" << glm::to_string(camera.GetViewMatrix()) << "\n";

        pyramidMesh.Draw();

        std::cout << "Camera: "
            << camera.GetPosition().x << ", "
            << camera.GetPosition().y << ", "
            << camera.GetPosition().z << "\n";

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
