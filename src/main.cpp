#include <iostream>
#include <cmath>

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
  "layout (location = 0) in vec3 aPos;\n" // the position variable has attribute position 0
  "out vec4 vertexColor;\n" // Color output to fragment shader
  "void main()\n"
  "{\n"
  "  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
  "  vertexColor = vec4(0.5, 0.0, 0.0, 1.0);\n"
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
  "uniform vec4 cpuColor;\n"
  "void main()\n"
  "{\n"
  "  FragColor = cpuColor;\n"
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

  unsigned int vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  float vertices[] = {
    -0.5f, -0.5f, 0.0f, // Left triangle, bottom left
     0.5f, -0.5f, 0.0f, // Left triangle, bottom right
     0.0f,  0.5f, 0.0f, // Left triangle, top
  };

  unsigned int vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
  glEnableVertexAttribArray(0); // Enable vertex attribute zero (position)

  

  int numAttributes; // Each input variable to a vertex shader is referred to as a vertex attribute
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numAttributes);
  std::cout<< "Maximum number of vertex attributes (aka vertex shader inputs): " << numAttributes << std::endl;

  while (!glfwWindowShouldClose(window)) {
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      float time = glfwGetTime();
      float green = sin(time) / 2.0f + 0.5f;
      int colorLocation = glGetUniformLocation(shaderProgram, "cpuColor");
      
      glUseProgram(shaderProgram);
      glUniform4f(colorLocation, 0.0f, green, 0.0f, 1.0f);
      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 3);
      
      glfwSwapBuffers(window);
      glfwPollEvents();
  }

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
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