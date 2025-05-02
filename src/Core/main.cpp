
#include "MiniEngine.h"

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    // Set OpenGL version to 3.3 Core 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
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

    // Set the viewport
    glViewport(0, 0, 800, 600);
    

    //LOADING SHADERS
    
    //Shader shader("Assets/Shaders/basic.vert", "Assets/Shaders/basic.frag");
    //Texture sandTexture("Assets/Textures/sand.jpg");
    auto shader = std::make_shared<Shader>("Assets/Shaders/basic.vert", "Assets/Shaders/basic.frag");
    auto sandTexture = std::make_shared<Texture>("Assets/Textures/sand.jpg");
    
    std::vector<float> vertices = {
         0.0f,  0.5f, 0.0f,  0.5f, 1.0f, // top
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, // bottom left
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f  // bottom right
    };

    std::vector<unsigned int> indices = { 0, 1, 2 };

    
   
    Material material(shader, sandTexture);
    Mesh triangleMesh(vertices, indices, sizeof(float) * 5);
    
    glm::mat4 model = glm::mat4(1.0f);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        material.Bind();
        shader->SetUniformMat4("u_Model", glm::value_ptr(model));
        triangleMesh.Draw();

        


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
