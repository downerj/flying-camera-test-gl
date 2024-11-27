#include <cstdlib>
#include <iostream>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#define QUOTE(x) #x
#define STRING(x) QUOTE(x)
#ifdef DEBUG
#define DEBUG_LOG(s) do { std::cout << s; } while (false);
#define DEBUG_ERROR(s) do { std::cerr << s; } while (false);
#define DEBUG_LOG_LINE(s) do { std::cout << s << std::endl; } while (false);
#define DEBUG_ERROR_LINE(s) do { std::cerr << s << std::endl; } while (false);
#else
#define DEBUG_LOG(s)
#define DEBUG_ERROR(s)
#define DEBUG_LOG_LINE(s)
#define DEBUG_ERROR_LINE(s)
#endif

void errorCallbackGLFW(int /*error*/, const char* description) {
  DEBUG_ERROR_LINE("GLFW error: " << description);
}

void keyCallback(GLFWwindow *window, int key, int /*scancode*/, int action, int mods) {
  // const int keyDown{action & GLFW_KEY_DOWN};
  const int keyUp{action & GLFW_KEY_UP};
  const bool keyQ{key == GLFW_KEY_Q};
  const bool keyW{key == GLFW_KEY_W};
  const bool keyF4{key == GLFW_KEY_F4};
  const int keyCtrl{mods & GLFW_MOD_CONTROL};
  const int keyAlt{mods & GLFW_MOD_ALT};
  if (keyUp && ((keyCtrl && (keyQ || keyW)) || (keyAlt && keyF4))) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    return;
  }
}

int main(int, char**) {
  if (!glfwInit()) {
    DEBUG_ERROR_LINE("GLFW error: Initialization failed");
    std::exit(EXIT_FAILURE);
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
  GLFWwindow* window{glfwCreateWindow(
    windowWidth,
    windowHeight,
    windowTitle,
    windowMonitor,
    windowShare
  )};
  if (!window) {
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);
  gladLoadGL(glfwGetProcAddress);
  DEBUG_LOG_LINE("C++ version: " << STRING(__cplusplus));
  DEBUG_LOG_LINE("OpenGL version: " << glGetString(GL_VERSION));
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
