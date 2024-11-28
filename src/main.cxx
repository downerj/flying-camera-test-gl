#include <array>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

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
#define DEBUG_LOG_NEWLINE() do { std::cout << std::endl; } while (false);
#define DEBUG_ERROR_NEWLINE() do { std::cerr << std::endl; } while (false);
#else
#define DEBUG_LOG(s)
#define DEBUG_ERROR(s)
#define DEBUG_LOG_LINE(s)
#define DEBUG_ERROR_LINE(s)
#define DEBUG_LOG_NEWLINE()
#define DEBUG_ERROR_NEWLINE()
#endif

#ifdef DEBUG
void errorCallbackGLFW(int /*error*/, const char* description) {
  DEBUG_ERROR_LINE("GLFW error: " << description);
}

void debugMessageCallbackGL(
  GLenum /*source*/,
  GLenum /*type*/,
  GLuint /*id*/,
  GLenum /*severity*/,
  GLsizei /*length*/,
  const GLchar* message,
  const void* /*userParam*/
) {
  DEBUG_ERROR_LINE("GL error: " << message);
}
#endif

void keyCallback(GLFWwindow *window, int key, int /*scancode*/, int action, int mods) {
  // const int keyDown{action & GLFW_KEY_DOWN};
  const int keyUp{action & GLFW_KEY_UP};
  const bool keyQ{key == GLFW_KEY_Q};
  const bool keyW{key == GLFW_KEY_W};
  const bool keyF4{key == GLFW_KEY_F4};
  const int keyCtrl{mods & GLFW_MOD_CONTROL};
  const int keyAlt{mods & GLFW_MOD_ALT};
  if (keyUp && ((keyCtrl && (keyQ || keyW)) || (keyAlt && keyF4))) {
    glfwSetWindowShouldClose(window, true);
    return;
  }
}

std::string readFile(const char* const fileName) {
  std::ifstream streamIn{fileName};
  std::ostringstream streamOut{};
  std::string s{};
  while (std::getline(streamIn, s)) {
    streamOut << s << '\n';
  }
  return streamOut.str();
}

GLFWwindow* initializeWindow() {
  if (!glfwInit()) {
    DEBUG_ERROR_LINE("GLFW error: Initialization failed");
    return nullptr;
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
  glfwWindowHint(GLFW_DECORATED, true);
#ifdef DEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif
  // glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);
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
    return nullptr;
  }
  glfwMakeContextCurrent(window);
  gladLoadGL(glfwGetProcAddress);
#ifdef DEBUG
  if (GLAD_GL_ARB_debug_output) {
    DEBUG_LOG_LINE("GL extension GL_ARB_debug_output available");
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    glDebugMessageCallbackARB(debugMessageCallbackGL, nullptr);
  } else {
    DEBUG_LOG_LINE("GL extension GL_ARB_debug_output unavailable");
  }
#endif
  DEBUG_LOG_LINE("C++ version: " << STRING(__cplusplus));
  DEBUG_LOG_LINE("OpenGL version: " << glGetString(GL_VERSION));
  glfwSwapInterval(1);
  glfwSetKeyCallback(window, keyCallback);
  return window;
}

GLuint createShader(GLenum type, const std::string& source) {
  GLuint shader{glCreateShader(type)};
  auto sources{std::make_unique<const char*[]>(1)};
  sources[0] = source.c_str();
  glShaderSource(shader, 1, sources.get(), nullptr);
  glCompileShader(shader);
  return shader;
}

GLuint createProgram(const std::string& vertexSource, const std::string& fragmentSource) {
  GLuint vertexShader{createShader(GL_VERTEX_SHADER, vertexSource)};
  GLuint fragmentShader{createShader(GL_FRAGMENT_SHADER, fragmentSource)};
  GLuint program{glCreateProgram()};
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);
  GLint status{};
  glGetProgramiv(program, GL_LINK_STATUS, &status);
#ifdef DEBUG
  if (!status) {
    GLsizei programLogLength{};
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &programLogLength);
    std::string programLog{};
    programLog.resize(programLogLength);
    glGetProgramInfoLog(program, programLogLength, &programLogLength, programLog.data());
    DEBUG_ERROR_LINE("GL program error: " << programLog);
    GLsizei vertexLogLength{};
    glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &vertexLogLength);
    if (vertexLogLength > 0) {
      std::string vertexLog{};
      vertexLog.resize(vertexLogLength);
      glGetShaderInfoLog(vertexShader, vertexLogLength, &vertexLogLength, vertexLog.data());
      DEBUG_ERROR_LINE("GL vertex shader error: " << vertexLog);
    }
    GLsizei fragmentLogLength{};
    glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &fragmentLogLength);
    if (fragmentLogLength > 0) {
      std::string fragmentLog{};
      fragmentLog.resize(fragmentLogLength);
      glGetShaderInfoLog(fragmentShader, fragmentLogLength, &fragmentLogLength, fragmentLog.data());
      DEBUG_ERROR_LINE("GL fragment shader error: " << fragmentLog);
    }
  }
#endif
  glDetachShader(program, vertexShader);
  glDetachShader(program, fragmentShader);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  if (!status) {
    throw std::runtime_error{"Error creating GL program"};
  }
  return program;
}

struct ProgramData {
  GLuint program;
  GLuint vao;
  // GLuint vbo;
  // GLuint ibo;
  GLsizei vertexCount;

  ProgramData() = delete;
  // ProgramData(GLuint program, GLuint vao, GLuint vbo, GLuint ibo, GLsizei vertexCount) :
  //   program{program}, vao{vao}, vbo{vbo}, ibo{ibo}, vertexCount{vertexCount} {}
  ProgramData(GLuint program, GLuint vao, GLsizei vertexCount) :
    program{program}, vao{vao}, vertexCount{vertexCount} {}
};

// const std::array<GLfloat, 6> vertices{
//   1.f, -1.f,
//   -1.f, -1.f,
//   0.f, 1.f,
// };

// const std::array<GLushort, 3> indices{
//   0, 1, 2,
// };

ProgramData initializeGL() {
  std::string vertexSource{readFile("shaders/main.vert")};
  std::string fragmentSource{readFile("shaders/main.frag")};
  GLuint program{createProgram(vertexSource, fragmentSource)};
  GLuint vao{};
  // GLuint vbo{};
  // GLuint ibo{};
  glGenVertexArrays(1, &vao);
  // glGenBuffers(1, &vbo);
  // glGenBuffers(1, &ibo);
  glBindVertexArray(vao);
  // glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);
  // glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, nullptr);
  // glEnableVertexAttribArray(0);
  // glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  // return ProgramData{program, vao, vbo, ibo, indices.size()};
  return ProgramData{program, vao, 3};
}

void mainLoop(GLFWwindow* window, const ProgramData& programData) {
  while (!glfwWindowShouldClose(window)) {
    int width;
    int height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    // glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LEQUAL);
    glClearColor(0.f, .5f, 1.f, 1.f);
    // glClearDepth(1.f);
    glClear(GL_COLOR_BUFFER_BIT /*| GL_DEPTH_BUFFER_BIT*/);
    glUseProgram(programData.program);
    glBindVertexArray(programData.vao);
    glDrawArrays(GL_TRIANGLES, 0, programData.vertexCount);
    // glDrawElements(GL_TRIANGLES, programData.vertexCount, GL_UNSIGNED_SHORT, nullptr);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void cleanUp(GLFWwindow* window, ProgramData& programData) {
  glfwDestroyWindow(window);
  glDeleteVertexArrays(1, &programData.vao);
  // glDeleteBuffers(1, &programData.vbo);
  glfwTerminate();
}

int main(int, char**) {
  GLFWwindow* window{initializeWindow()};
  if (window == nullptr) {
    std::exit(EXIT_FAILURE);
  }
  ProgramData programData{initializeGL()};
  mainLoop(window, programData);
  cleanUp(window, programData);
}
