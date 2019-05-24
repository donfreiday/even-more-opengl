#include <iostream>

#include "glad/glad.h" // Retrieve platform-specific OpenGL function pointers
#include <GLFW/glfw3.h> // Acquire an OpenGL context and handle windows, input etc

// GLFW reports errors via callback
void glfw_error_callback(int error, const char *description);

// Window resize callback
void glfw_framebuffer_size_callback(GLFWwindow *window, int width, int height);

const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;

int main(int argc, char **argv) {
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

  // Initialize GLAD with the GLFW function to load the platform dependent
  // addresses of OpenGL functions
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
      std::cout<<"Failed to initialize GLAD\n";
      return -1;
  }

  // Set viewport dimensions to window size
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);

  while (!glfwWindowShouldClose(window)) {
      glfwSwapBuffers(window);
      glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

void glfw_error_callback(int error, const char *description) {
  std::cout << "GLFW error: " << description << std::endl;
}

void glfw_framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}