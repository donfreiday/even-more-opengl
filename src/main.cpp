#include <iostream>

#include "glad/glad.h" // Retrieve platform-specific OpenGL function pointers
#include <GLFW/glfw3.h> // Acquire an OpenGL context and handle windows, input etc

// GLFW reports errors via callback
void glfw_error_callback(int error, const char *description);

// Window resize callback
void glfw_framebuffer_size_callback(GLFWwindow *window, int width, int height);

void glfw_key_callback(GLFWwindow *window, int key, int scancode, int mods);

const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;

int main(int argc, char **argv) {
  /////////////////////////////////////////////////////////////////////////////
  // Create window and OpenGL context
  /////////////////////////////////////////////////////////////////////////////
  glfwSetErrorCallback(glfw_error_callback);
  
  if (!glfwInit()) {
    std::cout << "Failed to initialize GLFW.\n";
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "window", nullptr, nullptr);
  if (!window) {
      std::cout<<"Failed to create GLFW window\n";
      glfwTerminate();
      return -1;
  }
  glfwMakeContextCurrent(window);

  /////////////////////////////////////////////////////////////////////////////
  // Initialize GLAD with the GLFW function to load the platform dependent
  // addresses of OpenGL functions
  /////////////////////////////////////////////////////////////////////////////
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
      std::cout<<"Failed to initialize GLAD\n";
      return -1;
  }
  
  // Set viewport dimensions to window size
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

  // Handle resizing window
  glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);

  /////////////////////////////////////////////////////////////////////////////
  // Shaders
  /////////////////////////////////////////////////////////////////////////////
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  const char* vertexShaderSource = 
  "#version 330 core\n"
  "layout (location = 0) in vec3 aPos;\n"
  "void main()\n"
  "{\n"
  "  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
  "}\n";
  glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
  glCompileShader(vertexShader);
  int success;
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
    std::cout<< "Failed to compile vertex shader: " << infoLog << std::endl;
  }

  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  const char* fragmentShaderSource =
  "#version 330 core\n"
  "out vec4 FragColor;\n"
  "void main()\n"
  "{\n"
  "  FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
  "}\n";
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
    std::cout<< "Failed to compile fragment shader: " << infoLog << std::endl;
  }

  // Link vertex and fragment shaders into shader program object
  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
    std::cout<< "Failed to link shader program: " << infoLog << std::endl;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  /////////////////////////////////////////////////////////////////////////////
  // Set up vertex buffers and attributes
  /////////////////////////////////////////////////////////////////////////////
  float vertices[] = {
     0.5f,  0.5f,  0.0f, // top right
     0.5f, -0.5f,  0.0f, // bottom right
    -0.5f, -0.5f,  0.0f, // bottom left
    -0.5f,  0.5f,  0.0f  // top left
  };
  unsigned int indices[] = {
    0, 1, 3, // First triangle
    1, 2, 3  // Second triangle
  };
  unsigned int vbo, vao, ebo;

  // Vertex array objects save vertex attribute state for reuse
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  // Bind VAO before setting up vertex buffers and attributes
  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Element buffer objects allow us to index into our vertice data
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


  // Specify input to the vertex shader via vertex attributes
  glVertexAttribPointer(
    0,                // Which vertex attribute to configure; position in our vertex shader is specified by layout (location = 0)
    3,                // Size of the vertex attribute; vec3 is composed of 3 values
    GL_FLOAT,         // Type of the data; vec* in GLSL consists of floating point values
    GL_FALSE,         // Normalize
    3 * sizeof(float),// Stride, the distance between consecutive vertex attributes
    (void*)0          // Offset of position data in buffer
  );
  glEnableVertexAttribArray(0); // Enable vertex attribute 0 (location)

  // glVertexAttribPointer() registered vbo as the vertex attribute's bound vertex buffer object so we can unbind it safely
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Don't unbind the EBO while a VAO is active as it will affect VAO state
  // glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Unbind VAO
  glBindVertexArray(0);

  while (!glfwWindowShouldClose(window)) {
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      
      glUseProgram(shaderProgram);
      glBindVertexArray(vao);
      glDrawElements(
        GL_TRIANGLES,   // Drawing mode
        6,              // Number of elements to draw; we have 6 indices
        GL_UNSIGNED_INT,// Type of the indices
        0               // Offset in EBO (or pass in index array if not using EBO)
        );
      
      glfwSwapBuffers(window);
      glfwPollEvents();
  }

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
  glfwTerminate();
  return 0;
}

void glfw_error_callback(int error, const char *description) {
  std::cout << "GLFW error: " << description << std::endl;
}

void glfw_framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void glfw_key_callback(GLFWwindow *window, int key, int scancode, int mods) {
    if (key == GLFW_KEY_ESCAPE)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}