#ifndef GRAPHICS_HXX
#define GRAPHICS_HXX

#include <optional>
#include <string_view>
#include <vector>

#include <glad/gl.h>

struct ShaderData {
  GLuint program;
  GLuint vao;
  GLsizei indexCount;

  ShaderData(
    GLuint program, GLuint vao, GLsizei indexCount
  );
  ShaderData() = delete;
  ShaderData(const ShaderData&) = default;
  ShaderData(ShaderData&&) = default;
  ShaderData& operator=(const ShaderData&) = default;
  ShaderData& operator=(ShaderData&&) = default;
};

class GraphicsEngine {
public:
  GraphicsEngine(
    std::string_view vertexSource,
    std::string_view fragmentSource
  );
  GraphicsEngine() = delete;
  GraphicsEngine(const GraphicsEngine&) = delete;
  GraphicsEngine(GraphicsEngine&&) = delete;
  GraphicsEngine& operator=(const GraphicsEngine&) = delete;
  GraphicsEngine& operator=(GraphicsEngine&&) = delete;
  ~GraphicsEngine();

  static auto initializeGL() -> bool;
  auto resize(int width, int height) -> void;
  auto render() -> void;

private:
  static auto createShader(GLenum type, std::string_view source) -> GLuint;
  static auto createProgram(
    std::string_view vertexSource,
    std::string_view fragmentSource
  ) -> std::optional<GLuint>;
  static auto createShaderData(
    std::string_view vertexSource,
    std::string_view fragmentSource
  ) -> std::optional<ShaderData>;

  int _windowWidth{};
  int _windowHeight{};
  std::vector<ShaderData> _shaderDatas{};
};



#endif // GRAPHICS_HXX
