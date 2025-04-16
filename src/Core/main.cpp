
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
    
    const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos, 1.0);
}
)";

    const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(0.0, 1.0, 0.0, 1.0); // Green
}
)";

    Shader shader(FromSource, vertexShaderSource, fragmentShaderSource);


    float triangle[] = {
     0.0f,  0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.4f, 0.0f
    };

    unsigned int triangleVAO = Renderer::CreateMesh(triangle, 9);
    
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Set background color and clear the screen
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        Renderer::Draw(triangleVAO, (size_t)3, shader);



        // Swap buffers and poll. All drawing needs to be done above
        glfwSwapBuffers(window);
        glfwPollEvents();
 
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
