#include <program.hpp>

#define SHADER_FILENAME_VERTEX "src/shaders/phongVertexShader.glsl"
#define SHADER_FILENAME_FRAGMENT "src/shaders/phongFragmentShader.glsl"

#define SHADER_FILENAME_SHADOW_MAP_VERTEX "src/shaders/shadowMapVertexShader.glsl"
#define SHADER_FILENAME_SHADOW_MAP_FRAGMENT "src/shaders/shadowMapFragmentShader.glsl"

#define SHADER_FILENAME_SINGLE_COLOR_VERTEX "src/shaders/singleColorVertexShader.glsl"
#define SHADER_FILENAME_SINGLE_COLOR_FRAGMENT "src/shaders/singleColorFragmentShader.glsl"

#define SHADER_FILENAME_DRAWING_VERTEX "src/shaders/drawingVertexShader.glsl"
#define SHADER_FILENAME_DRAWING_FRAGMENT "src/shaders/drawingFragmentShader.glsl"

#define SHADER_FILENAME_DRAWING_WORLD_VERTEX "src/shaders/drawingWorldVertexShader.glsl"
#define SHADER_FILENAME_DRAWING_WORLD_FRAGMENT "src/shaders/drawingWorldFragmentShader.glsl"

Program * Program::instancePhong = nullptr;
Program * Program::instanceShadowMap = nullptr;
Program * Program::instanceSingleColor = nullptr;
Program * Program::instanceDrawing = nullptr;
Program * Program::instanceDrawingWorld = nullptr;

Program * Program::getInstancePhong() {
    if(instancePhong  == nullptr) {
        instancePhong = new Program();
        instancePhong->vertexShaderFilename = SHADER_FILENAME_VERTEX;
        instancePhong->fragmentShaderFilename = SHADER_FILENAME_FRAGMENT;
        instancePhong->initProgram();
    }
    return instancePhong;
}

Program * Program::getInstanceShadowMap() {
    if(instanceShadowMap  == nullptr) {
        instanceShadowMap = new Program();
        instanceShadowMap->vertexShaderFilename = SHADER_FILENAME_SHADOW_MAP_VERTEX;
        instanceShadowMap->fragmentShaderFilename = SHADER_FILENAME_SHADOW_MAP_FRAGMENT;
        instanceShadowMap->initProgram();
    }
    return instanceShadowMap;
}

Program * Program::getInstanceSingleColor() {
    if(instanceSingleColor  == nullptr) {
        instanceSingleColor = new Program();
        instanceSingleColor->vertexShaderFilename = SHADER_FILENAME_SINGLE_COLOR_VERTEX;
        instanceSingleColor->fragmentShaderFilename = SHADER_FILENAME_SINGLE_COLOR_FRAGMENT;
        instanceSingleColor->initProgram();
    }
    return instanceSingleColor;
}

Program * Program::getInstanceDrawing() {
    if(instanceDrawing  == nullptr) {
        instanceDrawing = new Program();
        instanceDrawing->vertexShaderFilename = SHADER_FILENAME_DRAWING_VERTEX;
        instanceDrawing->fragmentShaderFilename = SHADER_FILENAME_DRAWING_FRAGMENT;
        instanceDrawing->initProgram();
    }
    return instanceDrawing;
}

Program * Program::getInstanceDrawingWorld() {
    if(instanceDrawingWorld  == nullptr) {
        instanceDrawingWorld = new Program();
        instanceDrawingWorld->vertexShaderFilename = SHADER_FILENAME_DRAWING_WORLD_VERTEX;
        instanceDrawingWorld->fragmentShaderFilename = SHADER_FILENAME_DRAWING_WORLD_FRAGMENT;
        instanceDrawingWorld->initProgram();
    }
    return instanceDrawingWorld;
}

void Program::subscribe() {
    if(programUserCount == 0) {
        initProgram();
    }
    programUserCount += 1;
}

void Program::unsubscribe() {
    programUserCount -= 1;
    if(programUserCount == 0 && glIsProgram(program)) {
        glDeleteProgram(program);
    }
}

void Program::loadShader(GLuint program, GLenum type, const std::string &shaderFilename) {
  GLuint shader = glCreateShader(type);
  std::string shaderSourceString = file2String(shaderFilename);
  const GLchar *shaderSource = (const GLchar *)shaderSourceString.c_str();
  glShaderSource(shader, 1, &shaderSource, NULL);
  glCompileShader(shader);
  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << "ERROR in compiling " << shaderFilename << "\n\t" << infoLog << std::endl;
    assert(false);
  }
  glAttachShader(program, shader);
  glDeleteShader(shader);
  getOpenGLError("loading and compiling shaders");
}

void Program::initProgram() {
    program = glCreateProgram();
    loadShader(program, GL_VERTEX_SHADER, vertexShaderFilename);
    loadShader(program, GL_FRAGMENT_SHADER, fragmentShaderFilename);
    glLinkProgram(program);
    getOpenGLError("linking program");
}
