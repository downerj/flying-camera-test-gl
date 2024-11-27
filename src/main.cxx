#include <iostream>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#define QUOTE(x) #x
#define STRING(x) QUOTE(x)

#ifdef DEBUG
void errorCallbackGLFW(int /*error*/, const char* description) {
  std::cerr << "GLFW error: " << description << std::endl;
}
#endif

void keyCallback(GLFWwindow *window, int key, int /*scancode*/, int action, int mods) {
  // int keyDown{action & GLFW_KEY_DOWN};
  int keyUp{action & GLFW_KEY_UP};
  bool keyQ{key == GLFW_KEY_Q};
  bool keyW{key == GLFW_KEY_W};
  bool keyF4{key == GLFW_KEY_F4};
  int keyCtrl{mods & GLFW_MOD_CONTROL};
  int keyAlt{mods & GLFW_MOD_ALT};
  if (keyUp && ((keyCtrl && (keyQ || keyW)) || (keyAlt && keyF4))) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    return;
  }
}

int main(int, char**) {
  if (!glfwInit()) {
    std::cerr << "GLFW error: Initialization failed" << std::endl;
    std::exit(1);
  }
#ifdef DEBUG
  glfwSetErrorCallback(errorCallbackGLFW);
#endif
  const int windowWidth{640};
  const int windowHeight{480};
  const char* windowTitle{"3D Flying Camera Test"};
  GLFWmonitor* windowMonitor{nullptr};
  GLFWwindow* windowShare{nullptr};
  const int windowGLMajor{3};
  const int windowGLMinor{3};
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, windowGLMajor);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, windowGLMinor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
  // glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  GLFWwindow* window{glfwCreateWindow(windowWidth, windowHeight, windowTitle, windowMonitor, windowShare)};
  if (!window) {
    glfwTerminate();
    std::exit(1);
  }
  glfwMakeContextCurrent(window);
  gladLoadGL(glfwGetProcAddress);
#ifdef DEBUG
  std::cout << "C++ version: " << STRING(__cplusplus) << std::endl;
  std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
#endif
  glfwSwapInterval(1);
  glfwSetKeyCallback(window, keyCallback);
  while (!glfwWindowShouldClose(window)) {
    int width;
    int height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0., .5, 1., 1.);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwDestroyWindow(window);
  glfwTerminate();
}
