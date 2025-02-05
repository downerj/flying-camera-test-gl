#include "window.hxx"

#include <stdexcept>

#include <GLFW/glfw3.h>

#include "debug.hxx"

namespace {
#ifdef DEBUG
  auto errorCallbackGLFW(int /*error*/, const char* description) -> void {
    LOG_ERROR("GLFW error: " << description << '\n');
  }
#endif // DEBUG
}

WindowHandler::WindowHandler() {
  if (!glfwInit()) {
    throw std::runtime_error{"Failed to initialize windowing library"};
  }
#ifdef DEBUG
  glfwSetErrorCallback(errorCallbackGLFW);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif // DEBUG
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glVersionMajor);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glVersionMinor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_DECORATED, true);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif // __APPLE__
  _window = glfwCreateWindow(
    _initialWidth, _initialHeight, _title,
    nullptr /*monitor*/, nullptr /*share*/
  );
  if (!_window) {
    glfwTerminate();
    throw std::runtime_error{"Failed to create window"};
  }
  glfwMakeContextCurrent(_window);
  glfwSetKeyCallback(_window, onKeyGLFW);
  glfwSetWindowUserPointer(_window, this);
}

WindowHandler::~WindowHandler() {
  glfwDestroyWindow(_window);
  glfwTerminate();
}

auto WindowHandler::getWindow() -> GLFWwindow* {
  return _window;
}

auto WindowHandler::getActions() const -> const WindowActions& {
  return _actions;
}

auto WindowHandler::getWidth() const -> int {
  return _width;
}

auto WindowHandler::getHeight() const -> int {
  return _height;
}

auto WindowHandler::isActive() const -> bool {
  return !glfwWindowShouldClose(_window);
}

auto WindowHandler::close() -> void {
  glfwSetWindowShouldClose(_window, true);
}

auto WindowHandler::resetSize() -> void {
  glfwSetWindowSize(_window, _initialWidth, _initialHeight);
}

auto WindowHandler::preRender() -> void {
  glfwGetFramebufferSize(_window, &_width, &_height);
}

auto WindowHandler::postRender() -> void {
  glfwSwapBuffers(_window);
  glfwPollEvents();
}

auto WindowHandler::resetActions() -> void {
  _actions.close = false;
  _actions.resetSize = false;
  _actions.pauseResume = false;
}

auto WindowHandler::onKeyGLFW(
  GLFWwindow* window, int key, int /*scancode*/, int action, int mods
) -> void {
  const auto windowHandler{
    static_cast<WindowHandler*>(glfwGetWindowUserPointer(window))
  };
  if (windowHandler) {
    windowHandler->onKey(key, action, mods);
  }
}

auto WindowHandler::onKey(int key, int action, int mods) -> void {
  // LOG("key=" << key << " action=" << action << " mods=" << mods << '\n');
  const bool closeKey1{
    action == GLFW_RELEASE && mods == GLFW_MOD_CONTROL && key == GLFW_KEY_Q
  };
  const bool closeKey2{
    action == GLFW_RELEASE && mods == GLFW_MOD_CONTROL && key == GLFW_KEY_W
  };
  const bool closeKey3{
    action == GLFW_RELEASE && mods == GLFW_MOD_ALT && key == GLFW_KEY_F4
  };
  const bool resetWindowKey{
    action == GLFW_RELEASE && mods == GLFW_MOD_CONTROL && key == GLFW_KEY_R
  };
  const bool pauseResumeKey{
    action == GLFW_RELEASE && mods == 0 && key == GLFW_KEY_SPACE
  };

  if (closeKey1 || closeKey2 || closeKey3) {
    _actions.close = true;
  } else if (resetWindowKey) {
    _actions.resetSize = true;
  } else if (pauseResumeKey) {
    _actions.pauseResume = true;
  }
}
